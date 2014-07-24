/*
 * main.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: cs1120255
 */
/*! \file
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "setEnvironment.h"
#include "globals.h"
#include "generator.h"
#include "network.h"
#include "timekeeper.h"
#define NUM_THREADS 4

using namespace std;

/*! \fn int main(int argc, char** argv)
* \brief This functions forks into two processes, timekeeper as the parent process and generator as the child.
*/
int main(int argc, char** argv){
	
	
	key_t key1 =ftok("./PartA/setEnvironment.h", 'A');
	int msqid1 = msgget(key1, 0666| IPC_CREAT);
	key_t key2 =ftok("./PartA/setEnvironment.h", 'B');
	int msqid2 = msgget(key2, 0666| IPC_CREAT);
	
	
	pid_t processID = fork();	

	if (processID==0){
		execl("./PartA/generator","./PartA/generator",argv[1],argv[3],0);
		cout<<"GENERATOR KILLS ITSELF AND TIMEKEEPER"<<endl;
		kill(getppid(),9);
	}
	else {
		execlp("./PartA/timekeeper","./PartA/timekeeper",argv[3]);
	}
	return 0;
}

