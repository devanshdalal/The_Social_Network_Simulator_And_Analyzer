/*
 * globals.h
 *
 *  Created on: Oct 8, 2013
 *      Author: cs1120255
 */
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include "setEnvironment.h"
#include "generator.h"
#include "network.h"
using namespace std;

/*!
* Object of type SetEnvironment that exposes the parsing function
*/
extern SetEnvironment setEnvironment;
/*!
* Object of type UniversityRandomseedTuple that stores all the data loaded by the parser from the input file
*/
extern UniversityRandomseedTuple tuple;
/*!
* Object of type Generator that exposes the generaor thread functions
*/
extern Generator generator1;
/*!
* the number of semesters and days of simulation as given by the user in the argument
*/
extern int num_of_semesters,days_of_simulation;

#endif /* GLOBALS_H_ */
