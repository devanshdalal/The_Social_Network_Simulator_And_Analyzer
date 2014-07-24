/*
 * network.h
 *
 *  Created on: Oct 11, 2013
 *      Author: cs1120255
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>



using namespace std;


/*! \brief This the data structure for the graph (the node of the graph) that contains all the data read from the .graphml file
*/
typedef struct GraphNode{	
	/*! \brief Name of the student or the faculty
	*/
	string name;
	/*! \brief University it belongs to 
	*/	
	string university_name;		
	string entry_code;			
	vector<GraphNode*> friend_list;
	/*! \brief The ID of the node (not to be confused with entry code)
	*/ 
	int number;						
	/*! \brief This field is used by the dfs and bfs algorithms (the distance from the starting point measured along the traversal)
	*/
	int distance;					
	/*! \brief This field is used by the dfs and bfs algorithms (whether it is visited during the traversal)
	*/
	bool is_visited;		
		
	/*! \brief This field is used by the dfs and bfs algorithms (the shortest distance from the starting point)
	*/
	int distance_till_here;			
public:
	~GraphNode()
	{}

}GraphNode;


/*! \brief Network module for part B
*/
class Network{			
	public:
	// construction methods

	/*! \brief Adds a node into the vector G(containing all nodes)
	*@param GraphNode* pointer to the node to be added
	* @return void
	*/
	void addNode(GraphNode*);	

	/*! \brief Makes the given nodes friends i.e. adds each person to adjacency list of other
	*@param GraphNode* pointer to the first node
	*@param GraphNode* pointer to the second node
	* @return void
	*/
					
	void makeFriends(GraphNode*, GraphNode*);	

	/*! \brief generates the graph made at last (just a check function)
	*@param vector<GraphNode*> the graph to be checked
	* @return void
	*/

	void generateGraph(vector<GraphNode*>);		

	/*! \brief Reads graph, returns vector of pointers to nodes.
	*@param void
	* @return vector<GraphNode*> vector of pointers to nodes
	*/
	
	vector<GraphNode*> readGraph();				
	
	/*! \brief Gives the name of the input node
	*/
	string giveName(GraphNode);			
	
	/*! \brief Gives the entry number of the input node
	*/
	string entryNumber(GraphNode);

	/*! \brief Gives the employee code of the input node	
	*/			
	string employeeCode(GraphNode);

	/*! \brief Gives the university name of the input node
	*/			
	string giveUniversityName(GraphNode);		

};


#endif /* NETWORK_H_ */
