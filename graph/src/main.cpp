#include "node.h"
#include <iostream>
#include "moves/allowedmove.h"
#include <deque>
#include <set>
#include <unordered_set>
#include "nodetracker.h"


std::deque<Node> queue;
NodeTracker tracker;
// std::deque<const Node*> queue;
size_t contained = 0;

size_t nodes = 0;
size_t edges = 0;

// TODO : use a heap based on the structure of the node

void push(const Node& node) {
    ++edges;
    if(tracker.contains(node)) {
        contained++;
        return;
    }
    ++nodes;
    tracker.push(node);
    queue.push_back(node);
    // queue.push_back(&storage.back());
}

Node pop() {
    Node n = queue.front();
    queue.pop_front();
    return n;
}

int main() {

    Node node("/BKR.P..p.rkb//0");

    size_t iterlog = 1e5;
    size_t iter = 0;

    push(node);
    while(!queue.empty()) {
        ++iter;
        Node np = pop();
        if(iter % iterlog == 0) {
            std::cout << queue.size() << "  " << np.age << "  " << np.toString() << "  " << contained << "  " << tracker.count() << "  " << tracker.toString() << std::endl;
        }
        auto successors = np.successors<1>();
        for(const Node& n : successors) {
            push(n);
        }
    }

    std::cout << nodes << "  " << edges << std::endl;

}