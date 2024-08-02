#include "builtin.h"
#include "utils.h"

// returns true if the 'exit' call
// should be performed
//
// (It must not be called from here)
int
exit_shell(char *cmd)
{
	return strcmp(cmd, "exit") == 0;
}

static int
chdir_error(int result)
{
	if (result < 0) {
		printf_debug("Error, no se pudo cambiar de "
		             "directorio\n");
		return 1;
	}
	return 0;
}

// returns true if "chdir" was performed
//  this means that if 'cmd' contains:
// 	1. $ cd directory (change to 'directory')
// 	2. $ cd (change to $HOME)
//  it has to be executed and then return true
//
//  Remember to update the 'prompt' with the
//  	new directory.
//
// Examples:
//  1. cmd = ['c','d', ' ', '/', 'b', 'i', 'n', '\0']
//  2. cmd = ['c','d', '\0']
int
cd(char *cmd)
{
	if (block_contains(cmd, 'c') != 0 || block_contains(cmd, 'd') != 1)
		return 0;

	char *current_directory = NULL;
	char *dir = NULL;
	bool solo_cd = strcmp(cmd, "cd") == 0;

	int chdir_result =
	        solo_cd ? chdir(getenv("HOME")) : chdir(split_line(cmd, ' '));

	if (chdir_error(chdir_result))
		return 0;

	if (!solo_cd)
		dir = getcwd(current_directory, PRMTLEN);

	solo_cd ? snprintf(prompt, sizeof prompt, "(%s)", getenv("HOME"))
	        : snprintf(prompt, sizeof prompt, "(%s)", dir);

	free(dir);

	return 1;
}

// returns true if 'pwd' was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
pwd(char *cmd)
{
	if (strcmp(cmd, "pwd") != 0) {
		return 0;
	}

	char *current_directory = NULL;
	char *dir = getcwd(current_directory, PRMTLEN);
	printf("%s\n", dir);
	free(dir);

	return 1;
}

// returns true if `history` was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
history(char *cmd)
{
	// Your code here

	return 0;
}
