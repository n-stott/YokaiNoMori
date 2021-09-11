#ifndef MCTS_H
#define MCTS_H

#include "quickrand.h"
#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include <cassert>
#include <iostream>
#include <algorithm>

template<typename NodeData>
struct MctsNode {
    MctsNode* root;
    MctsNode* parent;
    std::vector<MctsNode> children;
    size_t wins0;
    size_t wins1;
    size_t draws;
    size_t simulations;
    NodeData data;
    Quickrand& qr;
    static constexpr double c = 1.414;

    MctsNode(Quickrand& qr) :
            root(nullptr),
            parent(nullptr),
            children(),
            wins0(0),
            wins1(0),
            draws(0),
            simulations(0),
            data(),
            qr(qr)
    { }

    MctsNode(Quickrand& qr, MctsNode* root, MctsNode* parent, const NodeData& data) :
            root(root),
            parent(parent),
            children(),
            wins0(0),
            wins1(0),
            draws(0),
            simulations(0),
            data(data),
            qr(qr)
    { }

    void backPropagate(int winningPlayer) {
        ++simulations;
        draws += (winningPlayer == -1);
        wins0 += (winningPlayer == 0);
        wins1 += (winningPlayer == 1);
        if(parent) parent->backPropagate(winningPlayer);
    }

    static double rlog(double v) {
        if(v <= 1) return 0;
        if(v <= 10) return 1;
        if(v <= 100) return 2;
        if(v <= 1000) return 3;
        if(v <= 10000) return 4;
        if(v <= 100000) return 5;
        if(v <= 1000000) return 6;
        return 7;
    }

    double UCT(int player) const {
        // if(simulations == 0) return std::numeric_limits<double>::infinity();
        // assert(simulations > 0);
        double exploitation = ((player == 0 ? wins0 : wins1) + 0.5*draws) / (double)(1+simulations);
        double exploration = ((!!parent) ? c * std::sqrt( rlog(1+parent->simulations) / (1+simulations) ) : 0.0);
        return (exploitation + exploration);
    }

    MctsNode* bestChild() {
        using score_pair = std::pair<MctsNode*, double>;
        static std::vector<score_pair> scores;
        scores.clear();
        scores.reserve(children.size());
        for(auto& child : children) {
            scores.emplace_back(&child, child.UCT(data.player()));
        }
        auto best = std::max_element(scores.begin(), scores.end(), [](const score_pair& a, const score_pair& b) { return a.second < b.second; });

        MctsNode* bestchild = nullptr;
        if(best != scores.end()) bestchild = (*best).first;
        return bestchild;

        // auto best = std::max_element(
        //         children.begin(),
        //         children.end(),
        //         [&](const MctsNode& child1, const MctsNode& child2) {
        //             return child1.UCT(data.player) < child2.UCT(data.player);
        //         }
        // );
        // MctsNode* bestchild = nullptr;
        // if(best != children.end()) bestchild = &*best;
        // return bestchild;
    }

    void createChildren() {
        assert(children.size() == 0);
        for(auto& newdata : data.allNext()) {
            children.push_back( MctsNode(qr, root, this, newdata) );
        }
    }

    void simulate() {
        int winningPlayer = data.rollout(qr);
        backPropagate(winningPlayer);
    }

    std::string toString(std::string prefix = "", int maxdepth = 2, int depth = 0) const {
        std::string s;
        // s += prefix + "simulations : " + std::to_string(simulations) + '\n';
        // s += prefix + "wins : " + std::to_string(wins1) + "   " + std::to_string(wins2) + '\n';
        // s += prefix + "draws: " + std::to_string(draws) + '\n';
        // s += prefix + "uct : " + std::to_string(UCT(1)) + "   " + std::to_string(UCT(2)) + '\n';

        s += prefix + "simulations : " + std::to_string(simulations) + "sim ";
        s += std::to_string(wins0) + " w0 " + std::to_string(wins1) + " w1 ";
        s += std::to_string(UCT(0)) + " uct0  " + std::to_string(UCT(1)) + " uct1\n";

        s += prefix + data.toString() + '\n';
        if(depth == maxdepth) return s;
        for(auto& c : children) {
            // s += c.toString();
            s += c.toString(prefix+"  ", maxdepth, depth+1);
        }
        return s;
    }

};

template<typename NodeData>
struct MctsGraph {

    using Node = MctsNode<NodeData>;

    Node root;

    MctsGraph(Quickrand& qr, const NodeData& data) :
            root(qr, &root, nullptr, data)
    { }

    ~MctsGraph() {
        // Node::releaseSubtree(&root);
    }

    Node* chooseNode() {
        Node* current = &root;
        while(true) {
            if(current->simulations == 0) return current;
            Node* best = current->bestChild();
            if(!best) return current;
            current = best;
        };
    }

    void runOnce() {
        Node* node = chooseNode();
        if(!node->data.gameOver()) {
            node->createChildren();
        }
        node->simulate();
    }

    void run(size_t iterations) {
        for(size_t i = 0; i < iterations; ++i) {
            runOnce();
        }
    }

    NodeData* best() {
        Node* best = nullptr;
        if(root.simulations == 0) return nullptr;
        size_t maxSim = 0;
        for(auto& child : root.children) {
            size_t sims = child.simulations;
            if(sims > maxSim || (sims == maxSim && root.qr.next(2))) {
                maxSim = sims;
                best = &child;
            }
        }
        return &best->data;
    }

    std::string toString() const {
        std::string s = "root\n";
        s += root.toString("   ", 1, 0);
        return s;
    }

};

#endif