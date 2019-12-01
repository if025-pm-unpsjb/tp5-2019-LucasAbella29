/*
 * STR.h
 *
 *  Created on: Nov 14, 2019
 *      Author: lucas
 */

#ifndef STR_H_
#define STR_H_

#include <list>

#include "STRTask.h"

class STR {
private:
	std::list<Task> tasks;
	int size;
public:
	STR(std::list<Task> tasks, int size){
		this->tasks = tasks;
		this->size = size;
	}

	int getSize() const {
		return size;
	}

	const std::list<Task>& getTasks() const {
		return tasks;
	}
};

#endif /* STR_H_ */
