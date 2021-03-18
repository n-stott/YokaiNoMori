#ifndef TREE_H
#define TREE_H

#include "node.h"
#include <vector>


struct Tree {

    Node root;
    std::vector<Node*> nodes;
    std::vector<Node*> openNodes;
    
    Tree() :
        root(this),
        nodes(),
        openNodes({&root})
    {

    }

};


#endif