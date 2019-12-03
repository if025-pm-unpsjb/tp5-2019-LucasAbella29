/*
 * main33.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: lucas
 */

#include <list>
#include "mbed.h"       // Librería mbed
#include "FreeRTOS.h"   // Definiciones principales de FreeRTOS
#include "task.h"       // Funciones para el control de las tareas
#include "queue.h"		// Colas de tareas
#include "definitions.h"
#include "STR.h"
#include "STRTask.h"

// Funciones
void thread1(void*);
void thread2(void*);
void thread3(void*);
void thread4(void*);
void thread5(void*);
void eatCpu(TickType_t ticks);

// Variables globales
Serial pc(USBTX, USBRX); // tx, rx

std::list<Task> selectedTasks;

QueueHandle_t xQueue12, xQueue25, xQueue24, xQueue35;

// Tarea 1 → Tarea 2 → Tarea 5; Tarea 2 → Tarea 4; Tarea 3 → Tarea 5

int main() {
	Task task1("t1", 1, 2, 6, 6);
	Task task2("t2", 1, 1, 6, 6);
	Task task3("t3", 1, 2, 6, 6);
	Task task4("t4", 1, 1, 6, 6);
	Task task5("t5", 1, 1, 6, 6);

	// Creacion de tareas
	selectedTasks.push_back(task1);
	selectedTasks.push_back(task2);
	selectedTasks.push_back(task3);
	selectedTasks.push_back(task4);
	selectedTasks.push_back(task5);

	// Initializes the trace recorder, but does not start the tracing.
	vTraceEnable( TRC_INIT);

	// Creacion de las colas
	xQueue12 = xQueueCreate(5, sizeof(int)); // @suppress("Function cannot be resolved")
	xQueue25 = xQueueCreate(5, sizeof(int)); // @suppress("Function cannot be resolved")
	xQueue24 = xQueueCreate(5, sizeof(int)); // @suppress("Function cannot be resolved")
	xQueue35 = xQueueCreate(5, sizeof(int)); // @suppress("Function cannot be resolved")

	pc.printf("Cargando tareas...\n\r");

	xTaskCreate(thread1, task1.getName(), 256, (void*) 0, task1.getPriority(),	NULL);
	xTaskCreate(thread2, task2.getName(), 256, (void*) 1, task2.getPriority(),	NULL);
	xTaskCreate(thread3, task3.getName(), 256, (void*) 2, task3.getPriority(),	NULL);
	xTaskCreate(thread4, task4.getName(), 256, (void*) 3, task4.getPriority(),	NULL);
	xTaskCreate(thread5, task5.getName(), 256, (void*) 4, task5.getPriority(),	NULL);

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

	int message = 1;

	TickType_t previous = 0;
	TickType_t start = 0;
	TickType_t end = 0;
	// Indicador del numero de instancia
	int instance = 0;

	for (;;) {
		// Obtener el tick cuando arranca
		start = xTaskGetTickCount();

		// Hacer el trabajo
		eatCpu(currentTask->getC() * 1000);

		// Enviar mensaje a la cola de la tarea siguiente
		if ( xQueueSend(xQueue12, (void* ) &message, portMAX_DELAY) == pdTRUE) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s envio un mensaje a la cola 12\n\r", currentTask->getName());
		} else {
			pc.printf("La tarea %s no pudo enviar un mensaje a la cola 12\n\r", currentTask->getName());
		}

		// Obtener el tick en el que finaliza
		end = xTaskGetTickCount();

		// Imprimir informacion
		pc.printf("Tarea %s [%d, %d, %d]\n\r", currentTask->getName(), start, end, instance);

		// Bloquearse hasta la proxima instancia
		vTaskDelayUntil(&previous, currentTask->getT() * 1000);

		instance++;
	}
}

void thread2(void *params) {
	int index = (int) params;

	// Recuperar la tarea en ejecucion
	std::list<Task>::const_iterator it = selectedTasks.begin();
	std::advance(it, index);
	const Task *currentTask = &(*it);

	int message = 1;

	TickType_t previous = 0;
	TickType_t start = 0;
	TickType_t end = 0;
	// Indicador del numero de instancia
	int instance = 0;

	for (;;) {
		if (xQueueReceive(xQueue12, &message, portMAX_DELAY)) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s recibio un mensaje por la cola 12\n\r", currentTask->getName());
		}

		// Obtener el tick cuando arranca
		start = xTaskGetTickCount();

		// Hacer el trabajo
		eatCpu(currentTask->getC() * 1000);

		// Enviar mensaje a la cola de la tarea siguiente
		if ( xQueueSend(xQueue24, (void* ) &message, portMAX_DELAY) == pdTRUE) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s envio un mensaje a la cola 24\n\r", currentTask->getName());
		} else {
			pc.printf("La tarea %s no pudo enviar un mensaje a la cola 24\n\r", currentTask->getName());
		}

		// Enviar mensaje a la cola de la tarea siguiente
		if ( xQueueSend(xQueue25, (void* ) &message, portMAX_DELAY) == pdTRUE) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s envio un mensaje a la cola 25\n\r", currentTask->getName());
		} else {
			pc.printf("La tarea %s no pudo enviar un mensaje a la cola 25\n\r", currentTask->getName());
		}

		// Obtener el tick en el que finaliza
		end = xTaskGetTickCount();

		// Imprimir informacion
		pc.printf("Tarea %s [%d, %d, %d]\n\r", currentTask->getName(), start, end, instance);

		// Bloquearse hasta la proxima instancia
		vTaskDelayUntil(&previous, currentTask->getT() * 1000);

		instance++;
	}
}

void thread3(void *params) {
	int index = (int) params;

	// Recuperar la tarea en ejecucion
	std::list<Task>::const_iterator it = selectedTasks.begin();
	std::advance(it, index);
	const Task *currentTask = &(*it);

	int message = 1;

	TickType_t previous = 0;
	TickType_t start = 0;
	TickType_t end = 0;
	// Indicador del numero de instancia
	int instance = 0;

	for (;;) {
		// Obtener el tick cuando arranca
		start = xTaskGetTickCount();

		// Hacer el trabajo
		eatCpu(currentTask->getC() * 1000);

		// Enviar mensaje a la cola de la tarea siguiente
		if ( xQueueSend(xQueue35, (void* ) &message, portMAX_DELAY) == pdTRUE) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s envio un mensaje a la cola 35\n\r", currentTask->getName());
		} else {
			pc.printf("La tarea %s no pudo enviar un mensaje a la cola 35\n\r", currentTask->getName());
		}

		// Obtener el tick en el que finaliza
		end = xTaskGetTickCount();

		// Imprimir informacion
		pc.printf("Tarea %s [%d, %d, %d]\n\r", currentTask->getName(), start, end, instance);

		// Bloquearse hasta la proxima instancia
		vTaskDelayUntil(&previous, currentTask->getT() * 1000);

		instance++;
	}
}

void thread4(void *params) {
	int index = (int) params;

	// Recuperar la tarea en ejecucion
	std::list<Task>::const_iterator it = selectedTasks.begin();
	std::advance(it, index);
	const Task *currentTask = &(*it);

	int message = 1;

	TickType_t previous = 0;
	TickType_t start = 0;
	TickType_t end = 0;
	// Indicador del numero de instancia
	int instance = 0;

	for (;;) {
		if (xQueueReceive(xQueue24, &message, portMAX_DELAY)) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s recibio un mensaje por la cola 24\n\r", currentTask->getName());
		}

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

		instance++;
	}
}

void thread5(void *params) {
	int index = (int) params;

	// Recuperar la tarea en ejecucion
	std::list<Task>::const_iterator it = selectedTasks.begin();
	std::advance(it, index);
	const Task *currentTask = &(*it);

	int message = 1;

	TickType_t previous = 0;
	TickType_t start = 0;
	TickType_t end = 0;
	// Indicador del numero de instancia
	int instance = 0;

	for (;;) {
		if (xQueueReceive(xQueue25, &message, portMAX_DELAY)) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s recibio un mensaje por la cola 25\n\r", currentTask->getName());
		}

		if (xQueueReceive(xQueue35, &message, portMAX_DELAY)) { // @suppress("Invalid arguments")
			pc.printf("La tarea %s recibio un mensaje por la cola 35\n\r", currentTask->getName());
		}

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

		instance++;
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
