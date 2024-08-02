#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#ifndef NARGS
#define NARGS 4
#endif

#define NARGUMENTOS_EXEC NARGS + 2
#define NERROR -1
#define PID_HIJO 0

void
verificar_fork(int pid)
{
	if (pid == NERROR) {
		perror("Error al ejecutar el fork con main_pid\n");
		exit(EXIT_FAILURE);
	}
}

int
empaquetar_argumentos(char *comando, char *argumentos[NARGUMENTOS_EXEC])
{
	char *linea = NULL;
	int cant_argumentos = 0;
	size_t len = 0;
	argumentos[cant_argumentos++] = comando;

	while ((cant_argumentos <= NARGS) && (getline(&linea, &len, stdin) != -1)) {
		linea[strcspn(linea, "\n")] = '\0';
		argumentos[cant_argumentos] = strdup(linea);
		cant_argumentos++;
	}

	free(linea);
	argumentos[cant_argumentos] = NULL;
	return cant_argumentos;
}

void
ejecutar_comando(char *comando)
{
	char *argumentos[NARGUMENTOS_EXEC];
	while ((empaquetar_argumentos(comando, argumentos)) > 1) {
		pid_t pid = fork();
		verificar_fork(pid);
		if (pid == PID_HIJO) {
			execvp(comando, argumentos);
		} else {
			wait(NULL);
		}

		for (int i = 1; i <= NARGS; i++) {
			free(argumentos[i]);
			argumentos[i] = NULL;
		}
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		perror("No se ha ingresado un comando\n");
		exit(EXIT_FAILURE);
	}

	ejecutar_comando(argv[1]);
	return EXIT_SUCCESS;
}
