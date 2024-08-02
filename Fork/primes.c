#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define FD_READ 0
#define FD_WRITE 1
#define NERROR -1
#define PID_HIJO 0

void
verificar_pipe(int n)
{
	if (n == NERROR) {
		perror("Error al crear pipe\n");
		exit(EXIT_FAILURE);
	}
}

void
verificar_write(int n)
{
	if (n == NERROR) {
		perror("Error al intentar escribir a través de pipe\n");
		exit(EXIT_FAILURE);
	}
}

void
verificar_fork(int pid)
{
	if (pid == NERROR) {
		perror("Error al ejecutar el fork con main_pid\n");
		exit(EXIT_FAILURE);
	}
}

void
filtrar_primos(int file_descriptors[2])
{
	int numero_leido = 0;

	if (read(file_descriptors[FD_READ], &numero_leido, sizeof(numero_leido)) >
	    0) {
		printf("primo %d\n", numero_leido);
		int pipe_filtro[2];
		verificar_pipe(pipe(pipe_filtro));

		pid_t pid_filtro = fork();
		verificar_fork(pid_filtro);

		if (pid_filtro == PID_HIJO) {
			close(file_descriptors[FD_READ]);
			close(pipe_filtro[FD_WRITE]);
			filtrar_primos(pipe_filtro);
		} else {
			close(pipe_filtro[FD_READ]);
			int numero = 0;
			while (read(file_descriptors[FD_READ],
			            &numero,
			            sizeof(numero))) {
				if (numero % numero_leido != 0) {
					verificar_write(write(pipe_filtro[FD_WRITE],
					                      &numero,
					                      sizeof(numero)));
				}
			}

			close(pipe_filtro[FD_WRITE]);
			wait(NULL);
		}
	}
	close(file_descriptors[FD_READ]);
}

void
primos(int n)
{
	int main_pipe[2];
	verificar_pipe(pipe(main_pipe));

	pid_t main_pid = fork();
	verificar_fork(main_pid);

	if (main_pid == PID_HIJO) {
		close(main_pipe[FD_WRITE]);
		filtrar_primos(main_pipe);

	} else {
		close(main_pipe[FD_READ]);
		for (int i = 2; i <= n; i++) {
			verificar_write(write(main_pipe[FD_WRITE], &i, sizeof(i)));
		}

		close(main_pipe[FD_WRITE]);
		wait(NULL);
	}
}

int
main(int argc, char *argv[])
{
	if (!(argc > 1)) {
		perror("No se ha ingresado ningún parámetro.\n");
		exit(EXIT_FAILURE);
	}

	primos(atoi(argv[1]));

	return EXIT_SUCCESS;
}