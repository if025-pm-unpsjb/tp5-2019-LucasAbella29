/*
 * Task.h
 *
 *  Created on: Nov 14, 2019
 *      Author: lucas
 */

#ifndef STRTASK_H_
#define STRTASK_H_

class Task {
private:
	char * name;
	int priority;
	int c;
	int t;
	int d;
public:
	Task(char * name, int priority, int c, int t, int d){
		this->name = name;
		this->priority = priority;
		this->c = c;
		this->t = t;
		this->d = d;
	}

	char* getName() const {
		return name;
	}

	int getPriority() const {
		return priority;
	}

	int getC() const {
		return c;
	}

	int getD() const {
		return d;
	}

	int getT() const {
		return t;
	}
};

#endif /* STRTASK_H_ */
