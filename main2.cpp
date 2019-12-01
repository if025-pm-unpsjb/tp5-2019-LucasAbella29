/*
 * main2.cpp
 *
 *  Created on: Dec 1, 2019
 *      Author: lucas
 */

#include <list>
#include "mbed.h"       // Librería mbed
#include "FreeRTOS.h"   // Definiciones principales de FreeRTOS
#include "task.h"       // Funciones para el control de las tareas
#include "semphr.h"		// Semaforos y mutex
#include "definitions.h"
#include "STR.h"
#include "STRTask.h"

// Funciones
void thread1(void*);
void eatCpu(TickType_t ticks);
void useSharedResource(TickType_t duration);

// Variables globales
Serial pc(USBTX, USBRX); // tx, rx

std::list<Task> selectedTasks;

SemaphoreHandle_t xSemaphore = 0;

int main() {
	// Creacion de tareas
	selectedTasks.push_back(Task("t1", 3, 2, 4, 4));
	selectedTasks.push_back(Task("t2", 2, 1, 5, 5));
	selectedTasks.push_back(Task("t3", 1, 1, 6, 6));

	// Initializes the trace recorder, but does not start the tracing.
	vTraceEnable( TRC_INIT);

	// Creacion del mutex
	xSemaphore = xSemaphoreCreateMutex();

	int index = 0;

	pc.printf("Cargando tareas...\n\r");
	// Creacion de tareas
	for (Task task : selectedTasks) {
		xTaskCreate(thread1, task.getName(), 256, (void*) index,
				task.getPriority(), NULL);

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
	const Task *currentTask = &(*it);

	TickType_t previous = 0;

	for (;;) {
		pc.printf("Ejecutando tarea %s\n\r", currentTask->getName());

		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
			pc.printf("La tarea %s hace uso del recurso compartido\n\r",
					currentTask->getName());
			useSharedResource(currentTask->getC() * 1000);

			xSemaphoreGive(xSemaphore);
		}

		// Bloquearse hasta la proxima instancia
		vTaskDelayUntil(&previous, currentTask->getT() * 1000);
	}
}

void eatCpu(TickType_t ticks) {
	TickType_t current = 0;
	TickType_t previousTicks = 0;
	TickType_t currentTicks = xTaskGetTickCount();

	while (current < ticks) {
		currentTicks = xTaskGetTickCount();
		if (previousTicks < currentTicks) {
			current++;
			previousTicks = currentTicks;
		}
	}
}

void useSharedResource(TickType_t duration) {
	eatCpu(duration);
}
