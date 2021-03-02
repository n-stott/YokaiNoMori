#ifndef TREE_H
#define TREE_H

#include "node.h"
#include <vector>


struct Tree {

    Node root;
    std::vector<Node*> openNodes;

    

};


#endif