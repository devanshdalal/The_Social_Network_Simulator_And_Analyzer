/*
 * timekeeper.h
 *
 *  Created on: Oct 12, 2013
 *      Author: cs1120211
 */

#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


/*! \mainpage Generator and Timekeeper
* \section Generator
* The generator module basically calls four threads which are
* 1. generateFaculties
* 2. generateStudents
* 3. generateCourses
* 4. generateFriends
* These threads actively send and recieve messages from the timekeeper module and run accordingly.
* After all the threads get completed, a graph gets generated which is stored in graphml format.
*\section Timekeeper
* This module manages the request calls from the generator threads and sends wake up calls accordingly
* 
*/





/*! \brief Initializes the timekeeper process
*/
int timeInit();

#endif /* TIMEKEEPER_H_ */
