/*! \file
*/

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
#include <signal.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <vector>
#include <set>
#include <stdlib.h>
#include "network.h"
#include "algorithms.h"
#include <pthread.h>
#define NUM_THREADS 2

using namespace std;

/*! Array of threads
*/
pthread_t threads[NUM_THREADS];   	

/*! \brief The data structure that stores the message send by gyani
*/
struct query{
	
	/*! ID of the message (Here, It is arbitrary however different from the ID of queryCompleted)
	*/
	long ID;
	/*! The char array recieved from gyani
	*/
	char query_message[100];
};

/*! \brief The data structure that is used for sending message to gyani when the requested query gets completed.
*/
struct queryCompleted{

	/*! ID of the message (Here, It is arbitrary however different from the ID of queryCompleted)
	*/
	long ID;
	/*! if the requested query gets completed. 
	*/
	int isQueryCompleted;
};


bool finishedFloyed=false;

Network network;
Algorithms Search;

vector<GraphNode*> G;
int vertices;
int ** D;
set<GraphNode*> data;


/*! \fn  string recieveFromGyani()
* \brief It recievs the message from gyani
* @param void
* @return string the recieved message
*/
string recieveFromGyani(){			// Recieve From Gyani
	 query a;
    int msqid;
    key_t key = 123456;


    if ((msqid = msgget(key, 0644)) == -1) { /* connect to the  "SAME" queue */
	        perror("msgget");
	        exit(1);
    }
    if (msgrcv(msqid, &a, sizeof(query)-sizeof(long), 1,0) == -1) {
	    	perror("msgrcv");
	    	exit(1);
    }

	//msgctl(msqid, IPC_RMID, NULL);
	string ans(a.query_message);

	return ans;

}

/*! \fn  void sendToGyani()
* \brief It sends the completion message to gyani
* @param void
* @return void
*/
void sendToGyani(){			// Send To Gyani
	queryCompleted a;
	a.ID = 2;
	a.isQueryCompleted =1;

	int msqid;
	key_t key = 123456;


    if ((msqid = msgget(key, 0644)) == -1) {
        perror("error in msgget");
        exit(1);
    }

    if (msgsnd(msqid, &a, 4, IPC_NOWAIT) == -1) {
		perror("msgsnd");
    }

}


void * cppMain(void* threadID){	// Pre-processing functions 1. Read Graph  2. Floyd-Warshall
		G = network.readGraph();
		vertices=G.size();
		Search.FloydWarshall();    
		finishedFloyed = true;
}

/*! \fn  void * loadFn(void * threaid)
* \brief The thread that displays the text "Processing..." in the terminal
*/
void * loadFn(void * threaid){		// Processing......

	cerr << "Processing";
	while(finishedFloyed != true)
	{
		sleep(1);cerr<<".";
		sleep(1);cerr<<".";
		sleep(1);cerr<<".";
		sleep(1);cerr<<".";
		sleep(1);cerr<<".";
		sleep(1);cerr<<".";
		sleep(1);cerr << "\b\b\b\b\b\b\033[K";
	}
	cerr<<"\b\b\b\b\b\b\b\b\b\b\033[K";
	sendToGyani();

}

/*! \fn int main(void)
* \brief The main function that handles the recieved query from gyani and calls appropriate functions 
* from Algorithm class and then sends the completion message to gyani
*/
int main(void)	// main function of Analyzer
{
	int rc;
	rc=pthread_create(&threads[0], NULL, loadFn, NULL);
	if(rc){
		printf("Could not create thread\n");
		exit(-1);
	}
	rc=pthread_create(&threads[1], NULL, cppMain , NULL);
	if(rc){
		printf("Could not create thread\n");
		exit(-1);
	}
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	

	string query_message;
	while (true){			// runs and returns queries.
		GraphNode* nod1;
		GraphNode* nod2;
		query_message = recieveFromGyani();
		if(query_message[0] == '7')
			break;
		cout<<"#############################################################################"<<endl<<endl;
        char q_no = query_message[0];
		if(q_no=='4'){
			//The shortest path in the graph (between any pair of people)
			// max D[i][j] matrix
			int shortestPath=0,p1, p2;
			for(int i=0; i<vertices; i++)
				for(int j=0; j<vertices; j++){
					if(D[i][j] == vertices+1){
						cout<<"The graph is not connected. There exist two persons who are not connected by any path. "<<endl;
						goto outerloop;
					}	
					else if(shortestPath<D[i][j]){
						p1 = i;
						p2 = j;
						shortestPath=D[i][j];
					}	
				}

			cout<<"Shortest path length: "<<shortestPath<<endl;
			nod1=G[p1];
			nod2=G[p2];
			Search.dijkstra(nod2,nod1);

			outerloop:;
			
		}
		else
		{
			int iter = 2;
			while(query_message[iter] != '!')
				iter++;
			string entry1 = query_message.substr(2,iter-2);
			GraphNode * n1 =  Search.findByID(entry1),*n2;
			int p1;
			if(n1 != NULL)
			{
				p1 = n1->number;
				int p2=0;
				string entry2;
				if(q_no!='1' && q_no!='5' && q_no!='6'){
					entry2 = query_message.substr(iter+1,query_message.length()-iter-2);
					n2 = Search.findByID(entry2);
					if(n2 != NULL)
						p2 = n2->number;
					else{
						cerr<<entry2<<" NOT FOUND"<<endl;
						goto exit;
					}
				}
				if(q_no=='2' || q_no=='3')
				{
					if(p1==p2)
					  cout<<"same persons!"<<endl;
					else
					{
						if(q_no=='2')
						{
							cout<<"Shortest distance between "<<n1->name<<"("<<entry1<<") and "<<n2->name<<"("<<entry2<<") is " <<D[p1][p2]<<endl;
						}
						if(q_no=='3'){
							string person1,person2;
							n1=Search.findByID(entry1),n2=Search.findByID(entry2);
							if(D[n1->number][n2->number] == vertices+1)
								cout<<"There exist no path between "<<n1->name<<"("<<entry1<<") and "<<n2->name<<"("<<entry2<<")"<<endl;
							else{
								cout<<"Peoples on shortest path: ";  //Use Dijkstras'
								Search.dijkstra(n2,n1);
							}
						}
					}
				}
				else
				{
					if(q_no== '1'){
						//Size of Clique of a given person (identified by university name and ID)
						data.clear();
						cout<<"clique size of "<<n1->name<<"("<<entry1<<") is " << Search.clique(data,n1,-1).size()+1<<endl;
					}
					if(q_no=='5'){
						//cout<<Importance[p1]<<" is the importance number of "<<p1<<endl;
						int importance=0;
						for(int i=0; i<vertices; i++)
							for(int j=i+1; j<vertices; j++)
								if(D[i][j]==D[i][p1]+D[p1][j] && D[i][j]!=(vertices+1)){
									importance++;
								}
						cout<<importance<<" is the importance number of "<<n1->name<<"("<<entry1<<")"<<endl;
					}
					if(q_no=='6'){
						int importance=0;
						for(int i=0; i<vertices; i++)
							for(int j=i+1; j<vertices; j++)
								if(D[i][j]==D[i][p1]+D[p1][j] && D[i][j]!=(vertices+1)){
									importance++;
								}
						cout<<"Importance number of "<<n1->name<<"("<<entry1<<") is: "<<importance<<endl;
						int ia,num=0;
						bool flag=false;
						for(ia=0; ia<G[p1]->friend_list.size();ia++){
							int importance2=0;
							for(int i=0; i<vertices; i++)
								for(int j=i+1; j<vertices; j++)
									if(D[i][j]==D[i][G[p1]->friend_list[ia]->number]+D[G[p1]->friend_list[ia]->number][j] && D[i][j]!=(vertices+1)){
										importance2++;
									}
							if(importance < importance2){
								cout<<num+1<<". Friend "<<G[p1]->friend_list[ia]->name<<"("<<G[p1]->friend_list[ia]->entry_code <<") has higher Importance number("<<importance2<<")."<<endl<<endl;
								num++;
								flag = true;
							}
						}
						if(flag == false){
							cout<<n1->name<<"("<<entry1<<")"<<" has maximum importance among his friends"<<endl;
						}
					}
				}
			}
			else{
				cerr<<entry1<<" NOT FOUND"<<endl;
				goto exit;
			}
		}

		exit:
		cout<<"#############################################################################"<<endl;
		sendToGyani();


	}

	while(G.size() > 0 )
	{
		//G[0]-> ~GraphNode();
		delete G[0];
		G.erase(G.begin());
	}
	delete D;

	key_t key = 123456;
	int msqid = msgget(key, 0644);
	msgctl(msqid, IPC_RMID, NULL);
   	kill(getppid(),SIGINT);					// kill processes
	exit(getpid());
	return 0;
}

