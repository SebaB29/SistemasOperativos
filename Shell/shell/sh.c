#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"

char prompt[PRMTLEN] = { 0 };

static void
sigchild_handler()
{
	pid_t pid;
	int status;

	pid = waitpid(0, &status, WNOHANG);

	if (pid > 0)
		printf_debug("===> terminado: PID=%d - STATUS=%d\n", pid, status);
}

static void
configure_handler()
{
	struct sigaction signal_action;
	signal_action.sa_handler = sigchild_handler;
	sigemptyset(&signal_action.sa_mask);
	signal_action.sa_flags = SA_RESTART;  //| SA_ONSTACK;

	if (sigaction(SIGCHLD, &signal_action, NULL) == -1) {
		printf_debug("Error al configurar el manejador de señales\n");
		exit(EXIT_FAILURE);
	}
}

static void
create_alternative_stack(void **stack)
{
	*stack = malloc(SIGSTKSZ);
	if (!stack) {
		perror("Error al reservar memoria con malloc\n");
		exit(EXIT_FAILURE);
	}

	stack_t alternative_stack;
	alternative_stack.ss_sp = *stack;
	alternative_stack.ss_size = SIGSTKSZ;
	alternative_stack.ss_flags = 0;

	if (sigaltstack(&alternative_stack, NULL) == -1) {
		perror("Error al crear el stack alternativo\n");
		exit(EXIT_FAILURE);
	}
}

// runs a shell command
static void
run_shell()
{
	char *cmd;

	while ((cmd = read_line(prompt)) != NULL)
		if (run_cmd(cmd) == EXIT_SHELL)
			return;
}

// initializes the shell
// with the "HOME" directory
static void
init_shell()
{
	configure_handler();

	char buf[BUFLEN] = { 0 };
	char *home = getenv("HOME");

	if (chdir(home) < 0) {
		snprintf(buf, sizeof buf, "cannot cd to %s ", home);
		perror(buf);
	} else {
		snprintf(prompt, sizeof prompt, "(%s)", home);
	}
}

int
main(void)
{
	// void *stack = NULL;
	// create_alternative_stack(&stack);

	init_shell();

	run_shell();

	// free(stack);

	return 0;
}