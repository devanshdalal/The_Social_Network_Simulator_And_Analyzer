/*
 * network.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: cs1120255
 */
#include "network.h"


	void Network::makeFriends(GraphNode* a, GraphNode* b){
		a->friend_list.push_back(b);
		b->friend_list.push_back(a);
	}

	
	void Network::generateGraph(vector<GraphNode*> a){
		ofstream graph("PartB/../PartA/graphx.graphml");
		graph<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
		graph<<"<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\""<<endl;
		graph<<"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""<<endl;
		graph<<"xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns	     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">"<<endl;
		graph<<"<graph id=\"G\" edgedefault=\"undirected\">"<<endl;

		graph<<"<key id=\"d0\" for=\"node\" attr.name=\"name\" attr.type=\"string\"/>"<<endl;
		graph<<"<key id=\"d1\" for=\"node\" attr.name=\"entry_number\" attr.type=\"string\"/>"<<endl;
		graph<<"<key id=\"d2\" for=\"node\" attr.name=\"Univ_name\" attr.type=\"string\"/>"<<endl;
		for(int i=0; i<a.size();i++)
		{
			graph<<"<node id=\"n"<<a[i]->number<<"\"><data key=\"d0\">"<<a[i]->name<<"</data><data key=\"d1\">"<<a[i]->entry_code<<"</data><data key=\"d2\">"<<a[i]->university_name<<"</data></node>"<<endl;
		}
		int count=0;
		for(int i=0; i<a.size();i++)
		{
			for(int j=0;j<a[i]->friend_list.size();j++)
			{
				if(a[i]->number<a[i]->friend_list[j]->number)
				{
					graph<<"<edge id =\"e"<<count<<"\" source=\"n"<<a[i]->number<<"\" target=\"n"<<a[i]->friend_list[j]->number<<"\"/>"<<endl;
					count++;
				}
			}
		}
		graph<<"</graph>"<<endl;
		graph<<"</graphml>"<<endl;
	}

	vector<GraphNode*> Network::readGraph(){

		 vector <GraphNode*> G;

		 int a,b;
		 string line ;
		 ifstream myfile ("PartB/../PartA/graph.graphml");
		 while (!myfile.eof()){
			 string buffer;
			 getline(myfile,buffer);
			 if ((!myfile.eof())&&(buffer.length()==0)){
				 getline(myfile,buffer);
			 }
			line = line + buffer+"!";
		 }
		 if (myfile.eof()){
			 line=line+"!!!!!!!!!!";
		 }

		 int i = 0,j=0;
		 while(line.substr(i,5).compare("<node")!=0)
		 {
			 i++;
		 }

		 while(i<line.length())
		 {
			 if(line.substr(i,5).compare("<node") == 0 )
			 {
				 while(line.substr(i,2).compare("\"n")!=0)
					 i++;
				 j=i+1;
				 i=i+2;
				 while(line[j]!='"')
					 j++;
				 GraphNode* N = new GraphNode;
				 N->number = atoi(line.substr(i,j-i).c_str());
				 i=j+17;
				 j=i+1;
				 while(line[j]!='<')
					 j++;
				 N->name = line.substr(i,j-i);
				 i=j+22;
				 j=i+1;
				 while(line[j]!='<')
					 j++;
				 N->entry_code = line.substr(i,j-i);
				 i=j+22;
				 j=i+1;
				 while(line[j]!='<')
				 	 j++;
				 N->university_name = line.substr(i,j-i);

				 G.push_back(N);
				 i=j+15;

			 }
			 else if(line.substr(i,5).compare("<edge") == 0 )
			 {
				 while(line.substr(i,2).compare("\"n")!=0)
					 i++;
				 j=i+1;
				 i=i+2;
				 while(line[j]!='"')
					 j++;
				 a = atoi(line.substr(i,j-i).c_str());
				 while(line.substr(i,2).compare("\"n")!=0)
					 i++;
				 j=i+1;
				 i=i+2;
				 while(line[j]!='"')
					 j++;
				 b = atoi(line.substr(i,j-i).c_str());


				 G[a]->friend_list.push_back(G[b]);
				 G[b]->friend_list.push_back(G[a]);

				 i=j+4;
			 }
			 else
				 i++;
		}
		return G;
	}

// access methods

	string Network:: giveName(GraphNode a){
		return a.name;
	}
	string  Network::entryNumber(GraphNode a){
		return a.entry_code;
	}
	string  Network::employeeCode(GraphNode a){
		return a.entry_code;
	}

	string  Network::giveUniversityName(GraphNode a){
		return a.university_name;
	}


