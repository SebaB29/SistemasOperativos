#include <inc/lib.h>


void
umain(int argc, char **argv)
{
	envid_t env;
	cprintf("----------------- TEST RESETEO DE PRIORIDAD "
	        "-----------------\n");
	int parent_priority = thisenv->env_tickets;
	cprintf("\nPrioridad padre: %d\n", parent_priority);
	int f = fork();
	if (f < 0) {
		cprintf("Error in fork\n");
	} else if (f == 0) {
		int child_priority = thisenv->env_tickets;
		cprintf("\nPrioridad hijo: %d\n", thisenv->env_tickets);

		if (child_priority == thisenv->env_tickets)
			cprintf("\nBIEN: El hijo no cambio su prioridad.\n");
	} else {
		for (int i = 0; i < 8000; i++)  // wait para el padre
			cprintf("");

		if (parent_priority <= thisenv->env_tickets) {
			cprintf("\nBIEN: El padre redujo su prioridad "
			        "y se "
			        "reinicio la misma.\n");
		}
	}
}
