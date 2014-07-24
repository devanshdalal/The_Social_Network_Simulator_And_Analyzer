
#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include "network.h"
#include <algorithm>
#include <set>


/*! \mainpage
* \section Analyzer
* This modules when first called reads the graph from .graphml file and stores it into a data structure. Then it runs the flyod-warshall algorithm * and generates a shortest disatnce matrix accordingly 
* This modules recives the requests from the gyani module and then according to the query, it calls appropriate Algorithm functions to answer the *	query.
*\section Gyani
*It first calls the analyzer and waits till the pre-processing is done.
*This is a perl script which reads simple english queries from std::in and then makes an appropriate guess. Then it sends this message to the *analyzer and waits till it gives the answer.
*
*/

using namespace std;

/*!
* This class exposes all the algorithms such as dfs, bfs, dijkstra and floydWarshall to the analyser.
*/

class Algorithms{
	public:

/*! \brief Breadth first search on graph starting from the input node and traverse the whole connected component of the graph. 
* @param GraphNode* pointer to the starting node
* @return void
*/
	void bfs(GraphNode*); 
                        
/*! \brief Depth first search on graph starting from the input node and traverse the whole connected component of the graph.
* @param GraphNode* pointer to the starting node
* @return void
*/
	void dfs(GraphNode*);  

/*! \brief Breadth first search on graph starting from the first node and traverse till second node is found or whole connected component of graph is traversed.
* @param GraphNode* pointer to the starting node
* @param GraphNode* pointer to the ending node
* @return int returns shortest distance between the above two nodes.
*/                       
	int bfs_shortest(GraphNode*,GraphNode*);	

/*! \brief Depth first search on graph starting from the first node and traverse till second node is found or whole connected component of graph is traversed.
* @param GraphNode* pointer to the starting node
* @param GraphNode* pointer to the ending node
* @return int returns shortest distance between the above two nodes.
*/    
	int dfs_shortest(GraphNode*,GraphNode*);  

/*! \brief Marks the nodes of graph as unVisisted and distance to positive infinity.
* @param GraphNode* pointer to the node to be marked unvisited as
* @return void
*/   
	void init(GraphNode*);			

/*! \brief Finds the node corresponding to the given name in graph(if multiple instances exists then return the one formed earlier)
* @param string the name to be searched.
* @return GraphNode* pointer to the searched node.
*/			 
	GraphNode* findByName(string);       

/*! \brief Finds the node corresponding to the given ID in graph(if multiple instances exists then return the one formed earlier)
* @param string the ID to be searched.
* @return GraphNode* pointer to the searched node.
*/         
	GraphNode* findByID(string);

/*! \brief It constructs shortest distance matrix
* @param void
* @return void
*/                  
	void FloydWarshall();

/*! \brief Finds the clique of the input node and store it in the given set.
* @param set<GraphNode*> growing set of persons in the clique of the person.
* @param GraphNode* the node corresponding to the person which we are finding clique.  
* @param int index upto which we have searched in the friendlist of the person
* @return set<GraphNode*> 
*/								
	set<GraphNode* > clique(set<GraphNode*>,GraphNode*,int); 

/*! \brief  To find the shortest path and its length from first node to second, inbuilt priority queue is used with greedy implementation.O(|V|log|E|)
* @param GraphNode* starting node of dijkstra.
*@param GraphNode* ending node of dijkstra.
*@return void
*/
   
	void dijkstra(GraphNode* ,GraphNode*);        
	private:

/*! \brief This function actually performs the dfs using all the above mentioned functions. 
* @param GraphNode* the node on which dfs is applied
* @return void
*/
	void mainDfs(GraphNode*);                     

};

/*! \brief Graph G containg all the nodes.
*
*/
extern vector<GraphNode*> G; 

/*! \brief This object exposes the graph reader function for XML format.
*
*/            
extern Network network;        

/*! \brief Number of vertices in the graph. 
*/          
extern int vertices;                     

/*! \brief  Shortest distance Matrix in the graph
*/
extern int ** D;                              
#endif /* ALGORITHMS_H_ */
