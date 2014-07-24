/*
 * generator.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: cs1120255
 */
/*! \file
*/
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <math.h>
#include <signal.h>
#include <random>
#include <iomanip>

#include "globals.h"
#include "generator.h"
#include "setEnvironment.h"
#include "network.h"
#include "ipc.h"
#include "timekeeper.h"

#define NUM_THREADS 4

ofstream terminal("PartA/console.txt");  
ofstream logfile("PartA/log.txt");		// Saves the LOG
ofstream Frilog("PartA/Friend_log.txt");
pthread_t threads[NUM_THREADS];   	// Array of threads
/*!
 * List of all the mutex locks used in the program.
 */
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t friend_time_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t request_mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t friend_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fri_log_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exit_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t delete_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semester_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t round_robin_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t entry_code = PTHREAD_MUTEX_INITIALIZER;

/*!
 * List of all the conditional variables used in the program.
 */
pthread_cond_t round_robin_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t semester_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t count_threshold1=PTHREAD_COND_INITIALIZER;
pthread_cond_t count_threshold2=PTHREAD_COND_INITIALIZER;
pthread_cond_t count_threshold3=PTHREAD_COND_INITIALIZER;
pthread_cond_t count_threshold4=PTHREAD_COND_INITIALIZER;
pthread_cond_t delete_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t friend_time_cond=PTHREAD_COND_INITIALIZER;


SetEnvironment setEnvironment;	     // Object of SetEnvironment class, reads the input file and generates proper structures
UniversityRandomseedTuple tuple1;	// tuple1 contains all data read by setEnvironment
Network network;      			    // Object of Network class, stores the vector of Student nodes and faculty nodes.
Generator generator1; 			    // Object of Generator class, manages all the threads
float friend_count=0; 			    // current time of friends generation
default_random_engine rS,rFa,rC,rFd;
uniform_int_distribution <int> dis(0,10000000);

void killProcesses(); 			// Function to kill all processes

/*!
 * List of the arrays of the first, middle and last names of the students and faculties to be selected from.
 */
string student_firstnames[] = {"Sahil","Aakash","Aashish","Aastha","Abhay","Abhijeet","Abhimanyu ","Abhinav","Aditya","Aishwarya",
		"Akash","Akshay","Akula ","Amritansh","Arpit","Ashutosh","Banmanlang","Bhavesh","Bhavya","Sai","Devansh","Divyansh",
		"Edubilli","Himanshu","Kaushal","Kothinti","Kunal ","Sumanth","Mohit","Samheeth","Mukesh","Neel","Nikhil",
		"Nishant","Poojan","Parul ","Prateek","Raghuvansh","Rahul","Rayala","Rishi","Ritesh","Sachin","Sajal","Saket","Saurabh",
		"Shivam","Shubham","Shubhankar","Siddarth","Swapnil","Tata","Tushar","Valaval","Veerendra","Vijender","ZalaRonak",
		"Vansh","Tajveer","Paras","Namita","Prateek","Akshat","Bishwanath","Shagun","Ankita","Manas","Anupriya"};

string student_lastnames[] = {"Loomba","Kumar","Goyal","Tripathi","Mitra","Joshi","Kumar","Singh","Verma","Shakya","Rai","Paroda",
		"Bhat","Prakash","Sharma","Agrawal","Singh","Warr Lyndem","Deepak","Chouhan","Deep","Dalal","Avinash","Gangwar","Hooda",
		"Reddy","Singhal","Sumanth","Agarwal","Mathur","Kumar","Kamal","Jain","Mehta","Chaudhry","Saxena","Patidar","Bharadwaj",
		"Mundhra","Noothigattu","Yadav","Gupta","Garg","Solanki","Palash","Radhakrishna","Marda","Shyam","Nikhil","Vijender",
		"Prakash","Ratanlal","Pahwa","Sandhu","Narang","Atri","Kaushik","Saxena","Mandal","Kush","Brahmchari"};

string faculty_firstnames[] = {"Huzur", "Amithabh", "M.", "Subhasish", "Sorav", "K.K.", "Naveen", "Shyam", "B.", "Ragesh", "Pankaj",
		"Prem", "Saroj", "Amit", "Anshul", "S.", "Subhodh", "S.N", "Preeti", "Kolin", "Sanjeeva", "Vinay", "Maya", "Smruti",
		"Aaditeshwar", "Sandeep", "Parag"};

string faculty_middlenames[] ={"James","John","William","Thomas","Alexander","Robert","Michael","David","Andrew","Peter","Joseph",
		"Jack","Edward","Anthony","Christopher","Daniel","Henry","Matthew","Oliver","Jay"};

string faculty_lastnames[] = {"Saran", "Bagchi", "Balakrishna", "Banerjee", "Bansal", "Biswas", "Garg", "Gupta", "Jain", "Jaiswal",
		"Jalote", "Kalra", "Kaushik", "Kumar", "Arun", "Maheshwari", "Panda", " Paul", "Prasad", "Ribeiro", "Ramanath", "Sarangi",
		"Seth", "Sen", "Singla"};

/*!
 *  Number of names in respective arrays
 */
int stud_f,stud_l,facu_f,facu_m,facu_l;
int	num_of_sems=0;

/*!
 * Available number of students and faculty for assigning courses
 */
int available_stud=0,available_facu=0;

/*!
 * Boolean values for implementation of conditional variables.
 */
bool stud_finished=false,facu_finished=false,student_available=false,faculty_available=false;

/*!
 * Days of simulation from the argument
 */
int days_of_simulation=0;

/*!
 * Inbuilt function for generating random numbers.
 */
default_random_engine generator(tuple1.randomseed.friend_random);


/*!
 * Boolean values to check whether the respective thread has completed
 */
bool isGenStudFinished=false,isGenFacFinished=false,isGenCoursesFinished=false,isGenFriFinished=false,deleting_studs=false;


/*! \fn void sendRequest(Request struct_to_send)
 * \brief Sends a request (a structure of type Request) on the message queue
 * @param Request the required message to send
 * @return void
 */
void sendRequest(Request struct_to_send){
    int msqid;
    key_t key1 =ftok("./PartA/setEnvironment.h", 'A');

    if (key1 == -1){
        perror("error in ftok");
        exit(1);
    }
    // connect to a message queue and get its ID using the unique key created above.
    if ((msqid = msgget(key1, 0666)) == -1) {
        //perror("error in msggettt");
        exit(1);
    }

    if (msgsnd(msqid, &struct_to_send, sizeof(Request)-sizeof(long), IPC_NOWAIT) == -1) {
        //perror("msgsnd");
		exit(1);
    }
}

/*! \fn WakeUp recieveWakeUpCall(long a)
 * \brief Receives a wake up call (a structure of type WakeUp) from the message queue
 * @param long the unique ID number to get a specific message from the queue
 * @return WakeUp the required message received from the queue
 */
WakeUp recieveWakeUpCall(long a){
	struct WakeUp stored_struct;
	int msqid1;

	key_t key2 = ftok("./PartA/setEnvironment.h", 'B');

	if (key2 == -1) {
		perror("ftok");
		exit(1);
	}

	if ((msqid1 = msgget(key2, 0666)) == -1) {
		//perror("msggetrrtt");
		exit(1);
	}

	if (msgrcv(msqid1, &stored_struct, sizeof(WakeUp)-sizeof(long), a,0) == -1) {
	   	//perror("msgrcv");
		exit(1);
	}

	return stored_struct;
}

/*! \fn void stud_lock()
 * \brief Implements a wait till the boolean value stud_finished becomes true
 * @param void
 * @retrun void
 */
void stud_lock(){
	pthread_mutex_lock(&mutex1);
	if(!stud_finished)
			pthread_cond_wait(&count_threshold1, &mutex1);
	pthread_mutex_unlock(&mutex1);
}

/*! \fn void facu_lock()
 *\brief Waits till the boolean value facu_finished becomes true
 * @param void
 * @retrun void
 */
void facu_lock(){
	pthread_mutex_lock(&mutex2);
	if(!facu_finished)
			pthread_cond_wait(&count_threshold2, &mutex2);
	pthread_mutex_unlock(&mutex2);
}
int num_nodes=0;
/*!
 * \brief It generates all the faculty at the beginning of the simulation and fills a vector with objects of type 
 * GraphNode containing the data about that faculty.
 * @param void* threadid Since this is a pthread, this parameter is required
 * @return void* Since this is a pthread, this return value is necessary
 */

void * Generator::generateFaculty(void*threadid){
	//srand(tuple1.randomseed.faculty_random);
	rFa.seed(tuple1.randomseed.faculty_random);

	Request r;
	r.threadID=1;
	r.request_time=0; // in minutes
	sendRequest(r);

	terminal<<"genFac sends time 0 request"<<endl;

	WakeUp w = recieveWakeUpCall(1);

	terminal<<"generateFaculty receives wake up call"<<endl;
	terminal<<"GENERATEFACULTY: begins"<<endl;
	for(unsigned int i=0;i<tuple1.university_array.size();i++){  // for all universities
		University *unv=&(tuple1.university_array[i]);
		for(unsigned int j=0;j<unv->department_array.size();j++){	// for all departments
			Department *dep=&unv->department_array[j];
			for(int k=0;k<dep->num_faculty;k++){	// generate all faculties
				GraphNode* facu=new GraphNode;
				facu->isStudentNode=false;
				facu->year = 0;
				facu->name=faculty_firstnames[dis(rFa)%facu_f]+" "+faculty_middlenames[dis(rFa)%facu_m]+" "+faculty_lastnames[dis(rFa)%facu_l];
				facu->department=(dep);
				pthread_mutex_lock(&entry_code);
					num_nodes++;
					ostringstream ss1;ss1<<num_nodes;string a=ss1.str();
					facu->entry_code=a;
				pthread_mutex_unlock(&entry_code);
				facu->house_name=unv->locality_name[ dis(rFa) % unv->locality_name.size()];

				for(unsigned int l=0;l<unv->interest_array.size();l++){
					Interest* my=&(unv->interest_array[l]);
					if(float(1+dis(rFa)%100)<=(*my).popularity*100){
						facu->interest_array.push_back(my);
					}
				}

				pthread_mutex_lock(&log_lock);
					logfile<<"Year "<<setw(11)<<left<<0<<"generateFaculty: "<<setw(35)<<left<<facu->name<<" Dep: "<<setw(3)<<left<<dep->dept_name<<" ID: "<<facu->entry_code<<" Unv: "<<unv->university_name<<endl;
				pthread_mutex_unlock(&log_lock);

				pthread_mutex_lock(&mutex3);
					facu->department->faculties.push_back(facu);
				pthread_mutex_unlock(&mutex3);

				pthread_cond_signal(&count_threshold3);

				pthread_mutex_lock(&mutex2);
					network.faculty_list.push_back(facu);
					available_facu=network.faculty_list.size();
					pthread_cond_signal(&count_threshold2);
					pthread_cond_signal(&count_threshold4);
				pthread_mutex_unlock(&mutex2);
			}
		}
	}

	facu_finished=true;
	pthread_cond_broadcast(&count_threshold2);
	pthread_cond_broadcast(&count_threshold3);

	terminal<<"GENERATEFACULTY: exits"<<endl;

	pthread_mutex_lock(&round_robin_lock);
		isGenFacFinished=true;
		pthread_cond_signal(&round_robin_cond);
	pthread_mutex_unlock(&round_robin_lock);

	pthread_mutex_lock(&exit_lock);
		if ((isGenStudFinished)&&(isGenFacFinished)&&(isGenCoursesFinished)&&(isGenFriFinished)){
			terminal<<"generate process gets killed by 1"<<endl;
			killProcesses(); // exit the process
		}
	pthread_mutex_unlock(&exit_lock);

	void *ans;
	return ans;
}

/*!
 * \brief It generates all the students at proper time of the simulation and fills a vector with objects of type
 * GraphNode containing the data about that student.
 * @param void* threadid Since this is a pthread, this parameter is required
 * @return void* Since this is a pthread, this return value is necessary
 */

void * Generator::generateStudent(void* threadid){	
	//srand(tuple1.randomseed.faculty_random);
	rS.seed(tuple1.randomseed.faculty_random);

	Request q;
	q.threadID=2;
	q.request_time=0;
	sendRequest(q);

	terminal<<"genStud sends the time 0 request"<<endl;
	WakeUp w = recieveWakeUpCall(2);
	terminal<<"genStud receives the time 0 wake up call"<<endl;

	float count=0; //in years
	while (count < days_of_simulation/365.0){
		cout<<"Year "<<count<<endl;
		pthread_mutex_lock(&mutex1);
			stud_finished=false;
		pthread_mutex_unlock(&mutex1);
		terminal<<"GENERATESTUDENTS: wakesUp"<<endl;

		for(unsigned int i=0;i<network.student_list.size();i++){
			network.student_list[i]->year++;
		}

		if(count>0){
			pthread_mutex_lock(&delete_lock);
			deleting_studs=true;
			while(network.student_list[0]->year == 4){
				for(unsigned int j=0; j< network.student_list[0]->friend_list.size(); j++)	{
					for(unsigned int k=0; k<network.student_list[0]->friend_list[j]->friend_list.size(); k++){
						if(network.student_list[0]->entry_code.compare(network.student_list[0]->friend_list[j]->friend_list[k]->entry_code)==0){
							pthread_mutex_lock(&log_lock);
								logfile<<"Year "<<setw(11)<<left<<count<<"Friendship broken!!! "<<network.student_list[0]->name<< " & "<< network.student_list[0]->friend_list[j]->name<<endl;
							pthread_mutex_unlock(&log_lock);
							network.student_list[0]->friend_list[j]->friend_list.erase(network.student_list[0]->friend_list[j]->friend_list.begin()+k);
						}
					}
				}
				pthread_mutex_lock(&log_lock);
					logfile<<"Year "<<setw(11)<<left<<count<<"Student "<<network.student_list[0]->name<<" successfully passed out four years"<<endl;
				pthread_mutex_unlock(&log_lock);
				network.student_list[0]->~GraphNode();
				network.student_list.erase(network.student_list.begin());
			}
			deleting_studs=false;
			pthread_cond_signal(&delete_cond);
			pthread_mutex_unlock(&delete_lock);
		}

		for(unsigned int i=0;i<tuple1.university_array.size();i++){	// for all universities
			University * unv= &tuple1.university_array[i];
			for(unsigned int j=0;j<unv->department_array.size();j++){	// for all departments
				Department *dep=&unv->department_array[j];
				for(int k=0;k<dep->num_students_per_year;k++){	// generate all students
					GraphNode* node=new GraphNode;
					node->isStudentNode=true;
					node->year = 0;
					node->name=student_firstnames[dis(rS)%stud_f] +" "+ student_lastnames[dis(rS)%stud_l];
					node->department=(dep);
					pthread_mutex_lock(&entry_code);
						num_nodes++;
						ostringstream ss1;ss1<<num_nodes;string a=ss1.str();
						node->entry_code= a;
					pthread_mutex_unlock(&entry_code);
					node->hostel_name=unv->hostel_name[ dis(rS) % unv->hostel_name.size()];
					node->floor = ((int)count)%4;
					node->room_number=dis(rS)%100;

					for(unsigned int l=0;l<unv->interest_array.size();l++){	// assign interests
						Interest* my=&(unv->interest_array[l]);
						if(float(1+dis(rS)%100)<=my->popularity*100){
							node->interest_array.push_back(my);
						}
					}

					pthread_mutex_lock(&log_lock);
						logfile<<"Year "<<setw(11)<<left<<(count)<<"generateStudent: "<<setw(25)<<left<<node->name<<" Dep: "<<setw(3)<<left<<dep->dept_name<<" ID: "<<node->entry_code<<" Unv: "<<unv->university_name<< " floor: "<<node->floor<<" room no: "<<node->room_number<<endl;
					pthread_mutex_unlock(&log_lock);

					pthread_mutex_lock(&mutex1);
						network.student_list.push_back(node);
						available_stud=network.student_list.size();
						pthread_cond_signal(&count_threshold1);
						pthread_cond_signal(&count_threshold4);
					pthread_mutex_unlock(&mutex1);

				}
			}
		}

		pthread_mutex_lock(&mutex1);
			stud_finished=true;
		pthread_mutex_unlock(&mutex1);
		pthread_cond_broadcast(&count_threshold1);
		terminal<<"GENERATESTUDENTS: sleeps"<<endl;

		count+=1;
		q.threadID=2;
		q.request_time=count*365*24*60;

		if (count < days_of_simulation/365.0){
			pthread_mutex_lock(&semester_lock);
			/*if(num_of_sems%2==0)*/pthread_cond_wait(&semester_cond, &semester_lock);
			pthread_mutex_unlock(&semester_lock);

			pthread_mutex_lock(&friend_time_mutex);                   // generate friend should not jump
				while(count*365*24*60>=friend_count)pthread_cond_wait(&friend_time_cond, &friend_time_mutex);
			pthread_mutex_unlock(&friend_time_mutex);
			sendRequest(q);
			terminal<<"genStud sends next request"<<endl;

			w = recieveWakeUpCall(2);
			terminal<<"genStud receives next wake up call"<<endl;
		}
	}
	isGenStudFinished=true;

	pthread_mutex_lock(&exit_lock);
	if ((isGenStudFinished)&&(isGenFacFinished)&&(isGenCoursesFinished)&&(isGenFriFinished)){
		terminal<<"generate process gets killed by 2"<<endl;
		 killProcesses(); // exit the process
	}

	pthread_mutex_unlock(&exit_lock);
	void *ans;
	return ans;
}

/*!
 * \brief It generates all the courses at proper time of the simulation, clears the previously assigned courses to
 * students and faculties and finally assigns the generated courses to students and faculties in the given manner
 * @param void* threadid Since this is a pthread, this parameter is required
 * @return void* Since this is a pthread, this return value is necessary
 */

void * Generator::generateCourses(void*threadid){
	//srand(tuple1.randomseed.course_random);
	rC.seed(tuple1.randomseed.course_random);

	Request q;
	q.threadID=3;
	q.request_time=0;
	sendRequest(q);

	terminal<<"generatecourses sends time 0 request"<<endl;

	WakeUp w = recieveWakeUpCall(3);

	terminal<< "generatecourses receives time 0 wake up call"<<endl;

	float count=0; //in years
	while (count<days_of_simulation/365.0){
		terminal<<"GENERATECOURSES: wakesUp"<<endl;
		for(unsigned int i=0;i<tuple1.university_array.size();i++){
			University *unv=&tuple1.university_array[i];
			for(unsigned int j=0;j<unv->department_array.size();j++){
				Department *dep=&unv->department_array[j];
				dep->floated_courses.clear();
				for(unsigned int k=0;k<dep->course_array.size();k++){
					Course * pt= &dep->course_array[k];
					if(1+dis(rC)%100<=pt->frequency_per_year*50){
						pthread_mutex_lock(&log_lock);
							logfile<<"Year "<<setw(11)<<left<<(count)<<"generateCourses: Course "<<setw(6)<<left<<pt->course_name<<" added to "<<dep->dept_name<<endl;
						pthread_mutex_unlock(&log_lock);
						dep->floated_courses.push_back(pt);
					}
				}
			}
		}

		//Clear already registered courses
		for(unsigned int i=0;((i<network.faculty_list.size())or(network.faculty_list.size()==0));i++){
			facu_lock();
			pthread_mutex_lock(&log_lock);
				logfile<<"Year "<<setw(11)<<left<<(count)<<"generateCourses: Clearing faculties' registeredCourses"<<endl;
			pthread_mutex_unlock(&log_lock);
			GraphNode *temp=network.faculty_list[i];
			temp->registered_courses.clear();
		}

		for(unsigned int i=0;((i<network.student_list.size())or(network.student_list.size()==0));i++){					//Clears already registered courses
			stud_lock();
			pthread_mutex_lock(&log_lock);
				logfile<<"Year "<<setw(11)<<left<<(count)<<"generateCourses: clearing students' registeredCourses"<<endl;
			pthread_mutex_unlock(&log_lock);
			GraphNode *temp=network.student_list[i];
			temp->registered_courses.clear();
		}

		pthread_mutex_lock(&round_robin_lock);
			if(!isGenFacFinished)pthread_cond_wait(&round_robin_cond, &round_robin_lock);
		pthread_mutex_unlock(&round_robin_lock);

		// Assign Faculties
		for(unsigned int i=0;i<tuple1.university_array.size();i++){
			University *unv=&tuple1.university_array[i];
			for(unsigned int j=0;j<unv->department_array.size();j++){
				Department* dep=&unv->department_array[j];
				int num=0;
				for(unsigned int k=0;k<dep->floated_courses.size();){
					Course *ptr=dep->floated_courses[k];
					pthread_mutex_lock(&mutex3);
						while(dep->faculties.size()==0){
							pthread_cond_wait(&count_threshold3, &mutex3);
						}
					pthread_mutex_unlock(&mutex3);
					ptr->instructor=dep->faculties[num%(dep->faculties.size())];
					num++,k++;
					ptr->instructor->registered_courses.push_back(ptr);
					pthread_mutex_lock(&log_lock);
						logfile<<"Year "<<setw(11)<<left<<(count)<<"generateCourses: "<<setw(6)<<left<<ptr->course_name<<" given to "<<setw(25)<<left<<ptr->instructor->name<<" ID: "<<ptr->instructor->entry_code<<endl;
					pthread_mutex_unlock(&log_lock);
				}
			}
		}
		// Assign courses to Students
		for(unsigned int i=0;i<network.student_list.size();i++){
			stud_lock();
			GraphNode* stud=network.student_list[i];
			if(stud->department->floated_courses.size()==0)continue;							
			int dep_floated=(int)stud->department->semester_dept_courses;
			float ambigious=stud->department->semester_dept_courses-dep_floated;
			if(1+dis(rC)%100<=ambigious*100)dep_floated++;
			for(int k=0;k<dep_floated;k++){
				Course* pt=stud->department->floated_courses[dis(rC)%stud->department->floated_courses.size()];
				stud->registered_courses.push_back(pt);
				pthread_mutex_lock(&log_lock);
					logfile<<"Year "<<setw(11)<<left<<(count)<<"generateCourses: "<<setw(6)<<left<<pt->course_name<<" alloted to "<<setw(25)<<left<<stud->name<<" ID: "<<stud->entry_code<<endl;
				pthread_mutex_unlock(&log_lock);
			}
			int other_floated=(int)stud->department->semester_nondept_courses;
			ambigious=stud->department->semester_nondept_courses-other_floated;
			University *unv=stud->department->dep_univ; 					
			if(1+dis(rC)%100<=ambigious*100)other_floated++;
			for(int k=0;k<other_floated;){
				Department* dep= &unv->department_array[ dis(rC) % unv->department_array.size()];
				if(dep==stud->department or dep->floated_courses.size()==0){
					continue;
				}else{
					Course *pt=dep->floated_courses[dis(rC)%dep->floated_courses.size()];
					stud->registered_courses.push_back(pt);
					k++;
				}
			}
		}
		terminal<<"GENERATECOURSES: sleeps"<<endl;
		count+=0.5;
		num_of_sems++;

		q.threadID=3;
		q.request_time=count*365*24*60;

		if (count < days_of_simulation/365.0){
			pthread_mutex_lock(&semester_lock);
				if(num_of_sems%2==0)pthread_cond_signal(&semester_cond);
			pthread_mutex_unlock(&semester_lock);

			pthread_mutex_lock(&friend_time_mutex);                          // generate friend should not jump
				while(count*365*24*60>=friend_count)pthread_cond_wait(&friend_time_cond, &friend_time_mutex);
			pthread_mutex_unlock(&friend_time_mutex);

			sendRequest(q);
			terminal<<"genCourses sends NEXT request"<<endl;
				w = recieveWakeUpCall(3);
			terminal<<"generateCourses receives NEXT wakeup call"<<endl;
		}
	}
	isGenCoursesFinished=true;

	pthread_mutex_lock(&semester_lock);
		pthread_cond_broadcast(&semester_cond);
	pthread_mutex_unlock(&semester_lock);

	pthread_mutex_lock(&exit_lock);
	if ((isGenStudFinished)&&(isGenFacFinished)&&(isGenCoursesFinished)&&(isGenFriFinished)){

		terminal<<"generate process gets killed by 3"<<endl;
		killProcesses(); // exit the process
	}
	pthread_mutex_unlock(&exit_lock);

	void *ans;
	return ans;
}

/*!
 * \brief It generates the friends by sending requests between two randomly picked people generated till that time.
 * @param void* threadid Since this is a pthread, this parameter is required
 * @return void* Since this is a pthread, this return value is necessary
 */

void * Generator::generateFriends(void*threadid){
	//srand(tuple1.randomseed.friend_random);
	rFd.seed(tuple1.randomseed.friend_random);
	friend_count=0; //time in minutes
	float nextinterval=10;
	Request r;
	WakeUp w;
	int no_of_fri = 0;

	send_again:
	if(friend_count < days_of_simulation*60*24.0)
	{
		pthread_mutex_lock(&delete_lock);
			if(deleting_studs)pthread_cond_wait(&delete_cond,&delete_lock);
		pthread_mutex_unlock(&delete_lock);
		r.threadID = 4;
		r.request_time = friend_count;
		sendRequest(r);
		//terminal<<"genFriends sends request"<<endl;
		w = recieveWakeUpCall(4);
		//terminal<<"genFriends receives wakeup call"<<endl;


		if(w.request_accepted == 0){
			//terminal<<"request of past time, so rejected"<<endl;
			friend_count = w.current_time + 50*nextinterval;
			//terminal<<nextinterval<<endl;
			//terminal<<"sending new request"<<endl;
			pthread_mutex_lock(&friend_time_mutex);
				pthread_cond_signal(&friend_time_cond);
			pthread_mutex_unlock(&friend_time_mutex);
			goto send_again;
		}
		else{
			//terminal<<"GENERATEFRIENDS: begins"<<endl;
			//CHECK FOR LOCK
			pthread_mutex_lock(&friend_lock);
			while(network.faculty_list.size()+network.student_list.size()<2)
				pthread_cond_wait(&count_threshold4, &friend_lock);
			pthread_mutex_unlock(&friend_lock);
			int fl=network.faculty_list.size(),sl=network.student_list.size();
			int num = 1+dis(rFd) % (fl+sl);    // STEP1: Select a random person
			GraphNode* temp;
			if(num>fl){
				num -= fl;
				temp = network.student_list[num-1];
			}else
				temp = network.faculty_list[num-1];
			int num2 = 1+dis(rFd) % (fl+sl);   // STEP2: Select 2nd random person
			GraphNode* temp2;
			if(num2>fl){
				num2 -= fl;
				temp2 = network.student_list[num2-1];
			}else
				temp2 = network.faculty_list[num2-1];

			if(num!=num2){
				//CHECK IF ALREADY FRIENDS
				bool friends=false,a=false,b=false;
				for(unsigned int i=0;i<temp->friend_list.size();i++){
					if(temp->friend_list[i]->entry_code.compare(temp2->entry_code)==0){
						a=true;
					}
				}
				for(unsigned int j=0;j<temp2->friend_list.size();j++){
					if(temp2->friend_list[j]->entry_code.compare(temp->entry_code)==0){
						b=true;
						break;
					}
				}
				friends=a&b;
				bool send_request=false,accept_request=false;
				if(!friends){
					//CHECK PERSON 1.COURSE              temp1.registered_courses
					//	           2.INTEREST	         temp1.interest_array
					//             3.DEPARTMENT          temp1.department
					//             4.NEIGHBOURHOOD       same floor, same hostel, same Univ
					//             5.FRIENDS CAN ALSO INTRODUCE FRIEND
					// CALCULATE PROBABILITY
					float prob=0;
					float out_probability = temp->department->dep_univ->openness;
					if(temp->department->dep_univ->university_name.compare(temp2->department->dep_univ->university_name)==0){
						//DEPARTMENT
						if(temp->department->dept_name.compare(temp2->department->dept_name)==0 ){
							prob +=0.25*(1-out_probability);
						}
						//COURSES
						for(unsigned int i=0; i<temp->registered_courses.size();i++){
							unsigned int j;
							for(j=0; j<temp2->registered_courses.size();j++){
								if(temp->registered_courses[i]->course_name.compare(temp2->registered_courses[j]->course_name)==0){
									prob += 0.25*(1-out_probability);
									break;
								}
							}
							if(j!=temp2->registered_courses.size())
								break;
						}
						//NEIGHBOURHOOD
						if(temp->isStudentNode){
							if(temp2->isStudentNode){
								if(temp->hostel_name.compare(temp2->hostel_name)==0){
									if(temp->floor==temp2->floor){
										prob += 0.25*(1-out_probability);
									}
								}
							}
						}else{
							if(!temp2->isStudentNode){
								if(temp->house_name.compare(temp2->house_name)==0){
									prob += 0.25*(1-out_probability);
								}
							}
						}
					}
					for(unsigned int i=0; i<temp->interest_array.size();i++){
						unsigned int j;
						for(j=0; j<temp2->interest_array.size();j++){
							if(temp->interest_array[i]->interest_name.compare(temp2->interest_array[j]->interest_name)==0){
								prob += 0.25*(1-out_probability);
								break;
							}
						}
						if(j!=temp2->interest_array.size())
							break;
					}
					//SHOULD WE SEND REQUEST??
					int b;
					send_request=false;accept_request=false;
					if(prob == 0){
						b = 1+dis(rFd)%100;
						if(b<100*out_probability){
							send_request=true;
						}
					}
					b = 1+dis(rFd) % (100);
					if(b<100*prob)
					send_request=true;
					//STEP3: SHOULD WE ACCEPT REQUEST??
					if(send_request==true){
						int c = 1+dis(rFd)%100;
						if(c<temp2->department->dep_univ->friendliness*100){
							accept_request=true;
						}
					}
					//MAKE CONNECTIONS
					if(accept_request == true){
						temp->friend_list.push_back(temp2);
						temp2->friend_list.push_back(temp);
					}
				}
				if(friends){
					pthread_mutex_lock(&log_lock);
						logfile<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp->name<<     "is already friend of            "<<setw(35)<<left<<temp2->name<<endl;
					pthread_mutex_unlock(&log_lock);
					pthread_mutex_lock(&fri_log_lock);
						Frilog<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp->name<<     "is already friend of            "<<setw(35)<<left<<temp2->name<<endl;
					pthread_mutex_unlock(&fri_log_lock);
				}
				if(send_request)
				{
					pthread_mutex_lock(&log_lock);
						logfile<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp->name<<     "sent friend request to          "<<setw(35)<<left<<temp2->name<<endl;
					pthread_mutex_unlock(&log_lock);
					pthread_mutex_lock(&fri_log_lock);
						Frilog<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp->name<<     "sent friend request to          "<<setw(35)<<left<<temp2->name<<endl;
					pthread_mutex_unlock(&fri_log_lock);

					if(accept_request){
						no_of_fri++;
						pthread_mutex_lock(&log_lock);
							logfile<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp2->name<<"accepted friend request of      "<<setw(35)<<left<<temp->name<<endl;
						pthread_mutex_unlock(&log_lock);
						pthread_mutex_lock(&fri_log_lock);
							Frilog<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp2->name<<"accepted friend request of      "<<setw(35)<<left<<temp->name<<endl;
						pthread_mutex_unlock(&fri_log_lock);
					}
					if(!accept_request){
						pthread_mutex_lock(&log_lock);
							logfile<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp2->name<<"didn't accept friend request of "<<setw(35)<<left<<temp->name<<endl;
						pthread_mutex_unlock(&log_lock);
						pthread_mutex_lock(&fri_log_lock);
							Frilog<<"Mins "<<setw(11)<<((int)(friend_count*100))/100.0<<"generateFriends: "<<setw(35)<<left<<temp2->name<<"didn't accept friend request of "<<setw(35)<<left<<temp->name<<endl;
						pthread_mutex_unlock(&fri_log_lock);

					}
				}
			}

			//terminal<<"GENERATEFRIENDS: sleeps"<<endl;
			exponential_distribution<float> distribution(temp->department->dep_univ->friendship_rate);
			nextinterval = distribution(generator);//in minutes
			friend_count += nextinterval*200;
			pthread_mutex_lock(&friend_time_mutex);
				pthread_cond_signal(&friend_time_cond);
			pthread_mutex_unlock(&friend_time_mutex);
			goto send_again;
		}
	}
	isGenFriFinished = true;

	pthread_mutex_lock(&exit_lock);
	if ((isGenStudFinished)&&(isGenFacFinished)&&(isGenCoursesFinished)&&(isGenFriFinished)){
			terminal<<"generate process gets killed by 4"<<endl;
		killProcesses(); // exit the process
	}
	pthread_mutex_unlock(&exit_lock);

	void *ans;
	return ans;
}



void printNodes();	// check function

/*! \fn  void killProcesses()
*   \brief It kills the generator process and its parent process after clearing the message queue and freeing its space.
 */
void killProcesses(){
	terminal <<"KILL PROCESS CALLED"<<endl;
	for(int a=0;a<network.faculty_list.size(); a++){
		network.faculty_list[a]->number=a;
	}
	for(int a=0;a<network.student_list.size(); a++){
		network.student_list[a]->number=a;
	}
	network.generateGraph(network.faculty_list,network.student_list);

	while(network.student_list.size() > 0 )
	{
		network.student_list[0]->~GraphNode();
		network.student_list.erase(network.student_list.begin());
	}
	while(network.faculty_list.size() > 0 )
	{
		network.faculty_list[0]->~GraphNode();
		network.faculty_list.erase(network.faculty_list.begin());
	}

	key_t key1 =ftok("./PartA/setEnvironment.h", 'A');
	int msqid1 = msgget(key1, 0666);
	key_t key2 =ftok("./PartA/setEnvironment.h", 'B');
	int msqid2 = msgget(key2, 0666);

	if ((msgctl(msqid1, IPC_RMID, NULL) == -1)) {
		perror("msgctl");
		exit(1);
	}
	if ((msgctl(msqid2, IPC_RMID, NULL) == -1)) {
		perror("msgctl");
		exit(1);
	}
	terminal<<"GENERATOR KILLS ITSELF AND TIMEKEEPER"<<endl;
	kill(getppid(),SIGINT);
	exit(getpid());
}
void* generateStudent(void* threadid){
	generator1.generateStudent(threadid);
	void *ans;
	return ans;
}

void* generateFaculty(void* threadid){
	generator1.generateFaculty(threadid);
	void *ans;
	return ans;
}

void* generateCourses(void* threadid){
	generator1.generateCourses(threadid);
	void *ans;
	return ans;
}

void* generateFriends(void* threadid){
	generator1.generateFriends(threadid);
	void *ans;
	return ans;
}
/*
 * It starts all the threads
 */
int startThreads(){
	for(unsigned int i=0;i<tuple1.university_array.size();i++){
		for(unsigned int j=0;j<tuple1.university_array[i].department_array.size();j++){
			tuple1.university_array[i].department_array[j].dep_univ=&tuple1.university_array[i];
		}
	}
	int rc;
	for(int u=0;u<4;u++){	// Create all the threads
		rc=pthread_create(&threads[u], NULL, (u==0?&generateFaculty:(u==1?&generateStudent:(u==2?&generateCourses:&generateFriends))), NULL);
		if(rc){
			printf("Could not create thread\n");
			return -1;
		}
	}
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	pthread_join(threads[3], NULL);
	return 0;
}
/*!
 * \brief It initializes the generator process by first setting the environment from the input file
 * @param int number of days for simulation
 * @param const char* name of the input file
 * @return int returns 0 on successful completion.
 */
int Generator::init(const char* s,int q){
	tuple1 = setEnvironment.setEnvironment(s);
	days_of_simulation = q;
	stud_f=sizeof(student_firstnames)/sizeof(string);
	stud_l=sizeof(student_lastnames)/sizeof(string);
	facu_f=sizeof(faculty_firstnames)/sizeof(string);
	facu_m=sizeof(faculty_middlenames)/sizeof(string);
	facu_l=sizeof(faculty_lastnames)/sizeof(string);
	startThreads();

	pthread_mutex_destroy(&request_mutex1);
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
	pthread_mutex_destroy(&log_lock);
	pthread_mutex_destroy(&friend_time_mutex);
	pthread_mutex_destroy(&friend_lock);
	pthread_mutex_destroy(&fri_log_lock);
	pthread_mutex_destroy(&exit_lock);
	pthread_mutex_destroy(&delete_lock);
	pthread_mutex_destroy(&semester_lock);
	pthread_mutex_destroy(&round_robin_lock);
	pthread_mutex_destroy(&entry_code);	

	return 0;
}

int main(int argc, char** argv){
	generator1.init(argv[1],atoi(argv[2]));
	return 0;
}


