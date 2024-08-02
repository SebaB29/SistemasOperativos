#include "exec.h"

static void
fork_error(pid_t pid)
{
	if (pid < 0) {
		printf_debug("Error en fork");
		exit(EXIT_FAILURE);
	}
}

static void
pipe_error(pid_t pipe_exec)
{
	if (pipe_exec < 0) {
		printf_debug("Error en pipe");
		exit(EXIT_FAILURE);
	}
}

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++) {
		key[i] = arg[i];
	}

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void
set_environ_vars(char **eargv, int eargc)
{
	if (eargc < 1)
		return;

	char *key;
	char *value;
	int key_size = 0;

	for (int i = 0; i < eargc; i++) {
		if ((key_size = block_contains(eargv[i], '=')) > 0) {
			key = (char *) malloc(sizeof(key_size));
			value = (char *) malloc(
			        sizeof(strlen(eargv[i]) - key_size));
			get_environ_key(eargv[i], key);
			get_environ_value(eargv[i],
			                  value,
			                  block_contains(eargv[i], '='));
			setenv(key, value, 0);
			free(key);
			free(value);
		}
	}
}

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int
open_redir_fd(char *file, int flags)
{
	if (flags == STDIN_FILENO) {
		int fd = open(file, O_RDONLY | O_CLOEXEC, S_IWUSR | S_IRUSR);
		if (fd == -1) {
			printf_debug("Error en open STDIN");
		}
		return fd;
	}

	if (flags == STDOUT_FILENO || STDERR_FILENO) {
		int fd = open(file,
		              O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
		              S_IWUSR | S_IRUSR);
		if (fd == -1) {
			if (flags == STDOUT_FILENO) {
				printf_debug("Error en open STDOUT");
			} else {
				printf_debug("Error en open STDERR");
			}
			exit(EXIT_FAILURE);
		}
		return fd;
	}

	printf_debug("Error flags");
	exit(EXIT_FAILURE);
}

static void
rederict_fd1_to_fd2(int fd_1, int fd_2)
{
	if (dup2(fd_1, fd_2) == -1) {
		printf_debug("Error en dup2");
		exit(EXIT_FAILURE);
	}
}

static void
redirect_stdin(struct execcmd *r)
{
	if (strlen(r->in_file) > 0) {
		int fd_in = open_redir_fd(r->in_file, STDIN_FILENO);
		if (fd_in == -1) {
			free_command((struct cmd *) r);
			exit(EXIT_FAILURE);
		}
		rederict_fd1_to_fd2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
}

static void
redirect_stdout(struct execcmd *r)
{
	if (strlen(r->out_file) > 0) {
		int fd_out = open_redir_fd(r->out_file, STDOUT_FILENO);
		rederict_fd1_to_fd2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
}

static void
redirect_stderr(struct execcmd *r)
{
	if (strlen(r->err_file) > 0) {
		if (!strcmp(r->err_file, COMBINAR_SALIDA_Y_ERRORES)) {
			rederict_fd1_to_fd2(STDOUT_FILENO, STDERR_FILENO);
		} else {
			int fd_err = open_redir_fd(r->err_file, STDERR_FILENO);
			rederict_fd1_to_fd2(fd_err, STDERR_FILENO);
			close(fd_err);
		}
	}
}


// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option
void
exec_cmd(struct cmd *cmd)
{
	// To be used in the different cases
	struct execcmd *e;
	struct backcmd *b;
	struct execcmd *r;
	struct pipecmd *p;

	switch (cmd->type) {
	case EXEC:
		e = (struct execcmd *) cmd;

		if (e->eargc) {
			set_environ_vars(e->eargv, e->eargc);
			break;
		}

		execvp(e->argv[0], e->argv);
		free_command(cmd);  // En caso de que falle el execvp

		_exit(-1);
		break;

	case BACK: {
		b = (struct backcmd *) cmd;
		e = (struct execcmd *) b->c;

		execvp(e->argv[0], e->argv);

		free_command(cmd);  // En caso de que falle el execvp

		_exit(-1);
		break;
	}

	case REDIR: {
		r = (struct execcmd *) cmd;

		redirect_stdin(r);
		redirect_stdout(r);
		redirect_stderr(r);

		execvp(r->argv[0], r->argv);

		free_command(cmd);  // En caso de que falle el execvp

		_exit(-1);
		break;
	}

	case PIPE: {
		p = (struct pipecmd *) cmd;
		int status;
		int fd[2];

		pipe_error(pipe(fd));

		int left_pid = fork();
		fork_error(left_pid);

		setpgid(0, 0);

		if (left_pid == 0) {
			close(fd[READ]);
			rederict_fd1_to_fd2(fd[WRITE], STDOUT_FILENO);
			close(fd[WRITE]);
			exec_cmd(p->leftcmd);
		}

		int right_pid = fork();
		fork_error(right_pid);

		setpgid(0, 0);

		if (right_pid == 0) {
			close(fd[WRITE]);
			rederict_fd1_to_fd2(fd[READ], STDIN_FILENO);
			close(fd[READ]);
			exec_cmd(p->rightcmd);
		}
		close(fd[READ]);
		close(fd[WRITE]);
		waitpid(left_pid, &status, 0);
		waitpid(right_pid, &status, 0);

		free_command(parsed_pipe);
		exit(EXIT_SUCCESS);  // Seria para el hijo de la shell que en este caso no realiza execvp
		break;
	}
	}
}