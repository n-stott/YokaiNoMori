#ifndef NODE_H
#define NODE_H

#include <vector>
#include "state.h"
#include "enums.h"
#include "action.h"

struct Tree;

struct Node {

    State value;
    Tree* tree;
    int depth;
    ActionSet actions;
    std::vector<Node> children;


    Node(Tree* tree) :
        value(),
        tree(tree),
        depth(0),
        actions(),
        children()
    { }

    Node(Tree* tree, const State& s, int depth) :
        value(s),
        tree(tree),
        depth(depth),
        actions(),
        children()
    { }

    void computeChildren();

};

#endif
