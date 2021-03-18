#include "node.h"
#include "tree.h"

void Node::computeChildren() {
    // auto it = std::find(tree->openNodes.begin(), tree->openNodes.end(), this);
    // if(it != tree->openNodes.end()) tree->openNodes.erase(it);
    
    actions = value.allowedActions();
    children.clear();
    for(Action action : actions) {
        State s = value;
        if(action.type == Move) {
            s.move(action.p.type(), action.p.color(), action.src, action.dst);
        }
        else {
            s.drop(action.p.type(), action.p.color(), action.reservePos, action.dst);
        }
        children.emplace_back(tree, s, depth+1);
        tree->nodes.push_back(&children.back());
    }
}