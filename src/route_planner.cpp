#include "route_planner.h"
#include <algorithm>

using std::cout;
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;
using std::abs;
using std::endl; 


RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
  
  start_node = &m_Model.FindClosestNode(start_x,start_y);
  end_node = &m_Model.FindClosestNode(end_x,end_y);
  

}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  return node->distance(*end_node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  	current_node->FindNeighbors();
  	for(RouteModel::Node * elem : current_node->neighbors){
      	if(elem->visited == false){
         	elem->h_value = CalculateHValue(elem);
      		elem->g_value = current_node->g_value + current_node->distance(*elem);
      		elem->parent = current_node;
          	elem->visited = true;
          	this->open_list.emplace_back(elem);
        }
    }
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

bool Compare(const RouteModel::Node* a, const RouteModel::Node* b){
  int f1 = a->g_value + a->h_value;
  int f2 = b->g_value + b->h_value;
  return f1 < f2;
}

void SortOpenList(std::vector<RouteModel::Node*> & open_list){
  	sort(open_list.begin(),open_list.end(),Compare);
}

RouteModel::Node *RoutePlanner::NextNode() {
  std::sort(open_list.begin(),open_list.end(),[](const auto &_first,const auto &_second){
    return _first->h_value + _first->g_value < _second->h_value + _second->g_value;
  }); 
  //SortOpenList(open_list);
  auto pointer = open_list[0];
  open_list.erase(open_list.begin() + 0);
  return pointer;
}

// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
  distance = 0.0f;
  std::vector<RouteModel::Node> path_found;
  RouteModel::Node * temp = current_node;
  
  // TODO: Implement your solution here.
  while(temp != start_node){
    path_found.emplace_back(*temp);
   	distance += temp->distance(*(temp->parent));
    temp = temp->parent;
  }  
  if(temp == start_node){
    path_found.emplace_back(*start_node); 
  }else{
    cout << "There's something wrong" << std::endl;
  }
  std::reverse(path_found.begin(),path_found.end());
  
//   RouteModel::Node *node = current_node;
//   while(node->parent != nullptr){
//     path_found.insert(path_found.begin(),(*node));
//     node = node->parent;
//   }
//   start_node->visited = true;
//   path_found.insert(path_found.begin(),(*start_node));
//   distance = end_node->distance(*start_node);
  distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
  return path_found;
}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
//   start_node->visited = true;
//   RouteModel::Node * current_node = start_node;
//   open_list.emplace_back(start_node);
//   while(!open_list.empty() && current_node != nullptr){
//     if(current_node == end_node){
//       m_Model.path = ConstructFinalPath(current_node);
//       break;
//     }else{
//       AddNeighbors(current_node);
//       current_node = NextNode();
//     }
//   }  
    RouteModel::Node *current_node = nullptr;
    // TODO: Implement your solution here.
  	current_node = start_node;
  	current_node->g_value = 0.0;
  	current_node->h_value = CalculateHValue(current_node);
  	current_node->visited = true;
  	current_node->parent = nullptr;
  	open_list.push_back(current_node);
  
  while(open_list.size() > 0){
  	    current_node = this->NextNode();    
      if(current_node == end_node){
        m_Model.path = this->ConstructFinalPath(current_node);
        return;
      }else{
        AddNeighbors(current_node);
      }
    }
}