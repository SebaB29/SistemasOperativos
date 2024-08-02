#include <inc/lib.h>


void
umain(int argc, char **argv)
{
	envid_t env;
	cprintf("----------------- TEST REDUCCION DE PRIORIDAD "
	        "-----------------\n");
	int parent_priority = thisenv->env_tickets;
	cprintf("\nPrioridad padre: %d\n", parent_priority);
	int f = fork();
	if (f < 0) {
		cprintf("Error in fork\n");
	} else if (f == 0) {
		int child_priority = thisenv->env_tickets;
		cprintf("\nPrioridad hijo: %d\n", thisenv->env_tickets);

		for (int i = 0; i < 3000; i++)  // wait para el hijo
			cprintf("");

		if (child_priority > thisenv->env_tickets)
			cprintf("\nBIEN: El hijo redujo su prioridad pero no "
			        "cumplio un ciclo.\n");
	}
}
