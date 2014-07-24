/*
 * network.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: cs1120255
 */
#include <iomanip>
#include "network.h"
#include "setEnvironment.h"

/*! \file
*/
void Network::addNode(GraphNode* a){
	student_list.push_back(a);
}
void Network::makeFriends(GraphNode* a, GraphNode* b){
	a->friend_list.push_back(b);
	b->friend_list.push_back(a);
}
// access methods
bool  Network::isStudent(GraphNode a){
	return a.isStudentNode;
}
bool Network:: isFaculty(GraphNode a){
	return !isStudent(a);
}
vector<Interest*> Network:: giveInterests(GraphNode a){
	return a.interest_array;
}
Department* Network:: giveDepartment(GraphNode a){
	return (a.department);
}
string Network:: giveName(GraphNode a){
	return a.name;
}
string  Network::entryNumber(GraphNode a){
	return a.entry_code;
}
string  Network::employeeCode(GraphNode a){
	return a.entry_code;
}
string Network:: giveHostelName(GraphNode a){
	return a.hostel_name;
}
string giveHouseName(GraphNode a){
	return a.house_name;
}
int Network:: giveHouseNumber(GraphNode a){
	return a.house_number;
}

/*!
 * \brief It generates the graph into graphML format.
 * @param vector<GraphNode*> The vector containing all the faculties.
 * @param vector<GraphNode*> The vector containing all the students.
 * @return void
 */
void Network::generateGraph(vector<GraphNode*> a,vector<GraphNode*> b){		// Generate graph
	ofstream guide("guide.txt");

	guide<< "\t\t\t QUESTIONS ANSWERED BY GYANI:"<<endl<<endl;
	guide<<"1. Size of Clique of a given person (identified by university name and ID)"<<endl;
	guide<<"2. The length of the shortest path between two given people."<<endl;
	guide<<"3. The list of people on the shortest path between two given people."<<endl;
	guide<<"4. The shortest path in the graph (between any pair of people)."<<endl;
	guide<<"5. The importance of the given person (the importance is the number of the all-pair shortest paths that include that person)"<<endl;
	guide<<"6. Is any of the friends of a given person more important that him/her."<<endl<<endl;
	guide<<"	Type 'q', 'exit' , 'bye' , 'quit' to stop application"<<endl;

	guide<<"Below are the list of people with their friends:"<<endl<<endl;
	int no=0;
	for(int i=0; i< a.size(); i++)
		if(a[i]->friend_list.size() > 9){
			no++;
			guide<<no<<" NAME: "<<setw(35)<<left<<a[i]->name<<" ID: "<<a[i]->entry_code<<"          University:"<<a[i]->department->dep_univ->university_name<< endl;
			guide<<"\t\t\tFRIENDS:"<<endl;
			for(int j=0; j<a[i]->friend_list.size(); j++)
				guide<<"\t\t\t\t"<<"NAME: "<<setw(35)<<left<<a[i]->friend_list[j]->name<<" ID:"<<setw(5)<<right<<a[i]->friend_list[j]->entry_code<<"           University:"<<a[i]->friend_list[j]->department->dep_univ->university_name<< endl;
			guide<<endl;
		}
	ofstream graph("PartA/graph.graphml");
	int A = a.size();
	graph<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
	graph<<"<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\""<<endl;
	graph<<"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""<<endl;
	graph<<"xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns	     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">"<<endl;
	graph<<"<graph id=\"G\" edgedefault=\"undirected\">"<<endl;

	graph<<"<key id=\"d0\" for=\"node\" attr.name=\"name\" attr.type=\"string\"/>"<<endl;
	graph<<"<key id=\"d1\" for=\"node\" attr.name=\"entry_number\" attr.type=\"string\"/>"<<endl;
	graph<<"<key id=\"d2\" for=\"node\" attr.name=\"Univ_name\" attr.type=\"string\"/>"<<endl;
	for(int i=0; i<a.size();i++){
		graph<<"<node id=\"n"<<a[i]->number<<"\"><data key=\"d0\">"<<a[i]->name<<"</data><data key=\"d1\">"<<a[i]->entry_code<<"</data><data key=\"d2\">"<<a[i]->department->dep_univ->university_name<<"</data></node>"<<endl;
	}
	for(int i=0; i<b.size();i++){
		graph<<"<node id=\"n"<<(b[i]->number+A)<<"\"><data key=\"d0\">"<<b[i]->name<<"</data><data key=\"d1\">"<<b[i]->entry_code<<"</data><data key=\"d2\">"<<b[i]->department->dep_univ->university_name<<"</data></node>"<<endl;
	}
	int count =0;
	for(int i=0; i<a.size();i++){
		for(int j=0;j<a[i]->friend_list.size();j++)	{
			if(a[i]->friend_list[j]->isStudentNode){
				if(a[i]->number < (a[i]->friend_list[j]->number+A)){
					graph<<"<edge id =\"e"<<count<<"\" source=\"n"<<a[i]->number<<"\" target=\"n"<<(a[i]->friend_list[j]->number+A)<<"\"/>"<<endl;
					count++;
				}
			}
			else if(a[i]->number < a[i]->friend_list[j]->number){
				graph<<"<edge id =\"e"<<count<<"\" source=\"n"<<a[i]->number<<"\" target=\"n"<<a[i]->friend_list[j]->number<<"\"/>"<<endl;
				count++;
			}
		}
	}
	for(int i=0; i<b.size();i++){
		for(int j=0;j<b[i]->friend_list.size();j++)	{
			if(b[i]->friend_list[j]->isStudentNode){
				if(b[i]->number < (b[i]->friend_list[j]->number)){
					graph<<"<edge id =\"e"<<count<<"\" source=\"n"<<(b[i]->number+A)<<"\" target=\"n"<<(b[i]->friend_list[j]->number+A)<<"\"/>"<<endl;
					count++;
				}
			}
		}
	}
	graph<<"</graph>"<<endl;
	graph<<"</graphml>"<<endl;
}
