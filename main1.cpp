/*
 * main1.cpp
 *
 *  Created on: Nov 30, 2019
 *      Author: Lucas Abella
 */

#include <list>
#include "mbed.h"       // Librería mbed
#include "FreeRTOS.h"   // Definiciones principales de FreeRTOS
#include "task.h"       // Funciones para el control de las tareas
#include "definitions.h"
#include "STR.h"
#include "STRTask.h"

// Funciones
void thread1(void*);
void eatCpu(TickType_t ticks);

// Variables globales
Serial pc(USBTX, USBRX); // tx, rx

std::list<Task> selectedTasks;

int main() {
	// Creacion de tareas
	std::list<Task> tasks1;
	tasks1.push_back(Task("t1", 3, 2, 4, 4));
	tasks1.push_back(Task("t2", 2, 1, 5, 5));
	tasks1.push_back(Task("t3", 1, 1, 6, 6));

	std::list<Task> tasks2;
	tasks2.push_back(Task("t1", 3, 3, 5, 5));
	tasks2.push_back(Task("t2", 2, 1, 7, 7));
	tasks2.push_back(Task("t3", 1, 2, 10, 10));

	std::list<Task> tasks3;
	tasks3.push_back(Task("t1", 5, 2, 5, 5));
	tasks3.push_back(Task("t2", 4, 1, 6, 6));
	tasks3.push_back(Task("t3", 3, 1, 7, 7));
	tasks3.push_back(Task("t4", 2, 2, 10, 10));
	tasks3.push_back(Task("t5", 1, 1, 15, 15));

	// Creacion de sistemas
	STR systems[] = { STR(tasks1, 3), STR(tasks2, 3), STR(tasks3, 5) };

	// Obtencion del sistema elegido
	STR selected = systems[SELECTED_STR];

	// Obtencion de las tareas del sistema elegido
	selectedTasks = selected.getTasks();

	// Initializes the trace recorder, but does not start the tracing.
	vTraceEnable( TRC_INIT);

	int index = 0;

	pc.printf("Cargando tareas...\n\r");
	// Creacion de tareas
	for (Task task : selectedTasks) {
		xTaskCreate(thread1, task.getName(), 256, (void *) index, task.getPriority(), NULL);

		index++;
	}

	pc.printf("Tareas cargadas...\n\r");

	// Starts the tracing.
	vTraceEnable( TRC_START);

	pc.printf("Comenzando Scheduler...\n\r");

	vTaskStartScheduler();

	for (;;)
		;
}

void thread1(void *params) {
	int index = (int) params;

	// Recuperar la tarea en ejecucion
	std::list<Task>::const_iterator it = selectedTasks.begin();
	std::advance(it, index);
	const Task* currentTask = &(*it);

	TickType_t previous = 0;
	TickType_t start = 0;
	TickType_t end = 0;
	// Indicador del numero de instancia
	int instance = 0;
	for(;;) {
		// Obtener el tick cuando arranca
		start = xTaskGetTickCount();

		// Hacer el trabajo
		eatCpu(currentTask->getC() * 1000);

		// Obtener el tick en el que finaliza
		end = xTaskGetTickCount();
		// Imprimir informacion
		pc.printf("Tarea %s [%d, %d, %d]\n\r", currentTask->getName(), start, end, instance);
		// Bloquearse hasta la proxima instancia
		vTaskDelayUntil(&previous, currentTask->getT() * 1000);
		// Contabilizar que arranca una nueva instancia
		instance++;
	}
}

void eatCpu(TickType_t ticks) {
	TickType_t current = 0;
	TickType_t previousTicks = 0;
	TickType_t currentTicks = xTaskGetTickCount();

	while(current < ticks){
		currentTicks = xTaskGetTickCount();
		if(previousTicks < currentTicks){
			current++;
			previousTicks = currentTicks;
		}
	}
}
