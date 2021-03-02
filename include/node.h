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
    std::vector<Action> actions;
    std::vector<Node> children;

    explicit Node(const State& s) :
        value(s),
        tree(nullptr),
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
