#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>
#include <kern/syscall.h>


static int cant_llamadas_scheduler;

void sched_halt(void);
void round_robin(int indice_inicial);
void priority_sched(int indice_inicial, int ticket_ganador);
int obtener_ticket_ganador();
int calcular_numero_random();
int calcular_tickets_totales();

// Choose a user environment to run and run it.
void
sched_yield(void)
{
	cant_llamadas_scheduler++;
	int indice_inicial = curenv ? ENVX(curenv->env_id) : 0;

#ifdef SCHED_ROUND_ROBIN
	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running. Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.

	round_robin(indice_inicial);

#endif

#ifdef SCHED_PRIORITIES
	// Implement simple priorities scheduling.
	//
	// Environments now have a "priority" so it must be consider
	// when the selection is performed.
	//
	// Be careful to not fall in "starvation" such that only one
	// environment is selected and run every time.

	int ticket_ganador = obtener_ticket_ganador();
	priority_sched(indice_inicial, ticket_ganador);

#endif

	// Without scheduler, keep runing the last environment while it
	// exists if (curenv) { 	env_run(curenv);
	// }

	// sched_halt never returns
	sched_halt();
}

void
round_robin(int indice_inicial)
{
	int indice;
	struct Env *proceso_actual;
	for (int i = 0; i < NENV; i++) {
		indice = (indice_inicial + i) % NENV;
		proceso_actual = &envs[indice];
		if (proceso_actual->env_status == ENV_RUNNABLE) {
			env_run(proceso_actual);
		}
	}

	if (curenv && curenv->env_status == ENV_RUNNING) {
		env_run(curenv);
	}
}

int
obtener_ticket_ganador()
{
	int numero_random = calcular_numero_random();
	int tickets_totales = calcular_tickets_totales();
	return numero_random % (tickets_totales + 1);
}

void
priority_sched(int indice_inicial, int ticket_ganador)
{
	int contador = 0;
	int indice;
	struct Env *proceso_actual;

	for (int i = 0; i < NENV; i++) {
		indice = (indice_inicial + i) % NENV;
		proceso_actual = &envs[indice];
		contador += sys_get_tickets(proceso_actual);

		if (contador > ticket_ganador &&
		    proceso_actual->env_status == ENV_RUNNABLE) {
			sys_set_tickets(proceso_actual);
			env_run(proceso_actual);
		}
	}

	if (curenv && curenv->env_status == ENV_RUNNING) {
		sys_set_tickets(curenv);
		env_run(curenv);
	}
}

int
calcular_tickets_totales()
{
	int tickets_totales = 0;

	for (int i = 0; i < NENV; i++) {
		if (envs[i].env_status == ENV_RUNNABLE) {
			tickets_totales += envs[i].env_tickets;
		}
	}

	return tickets_totales;
}

void
imprimir_estadisticas()
{
	cprintf("Estadisticas del Scheduler \n");

	cprintf("Cantidad de llamadas al Scheduler: %d \n",
	        cant_llamadas_scheduler);

	cprintf("Estadisticas de los procesos. \n");

	for (int i = 0; i < NENV; i++) {
		struct Env *proceso_actual = &envs[i];
		if (proceso_actual->env_runs > 0) {
			cprintf("id: %d \t, ejecuciones: %d \t, tickets: %d "
			        "\n, cantidad reseteo de la prioridad: %d \n",
			        proceso_actual->env_id,
			        proceso_actual->env_runs,
			        proceso_actual->env_tickets,
			        proceso_actual->env_cant_reseteos);
		}
	}
}

int
calcular_numero_random()
{
	int numero_random = 0;

	for (int i = 0; i < NENV; i++) {
		struct Env *proceso_actual = &envs[i];
		if (proceso_actual->env_status == ENV_RUNNABLE) {
			numero_random += proceso_actual->env_id;
		}
	}

	return numero_random;
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");
		imprimir_estadisticas();
		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Once the scheduler has finishied it's work, print statistics
	// on performance. Your code here

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile("movl $0, %%ebp\n"
	             "movl %0, %%esp\n"
	             "pushl $0\n"
	             "pushl $0\n"
	             "sti\n"
	             "1:\n"
	             "hlt\n"
	             "jmp 1b\n"
	             :
	             : "a"(thiscpu->cpu_ts.ts_esp0));
}
