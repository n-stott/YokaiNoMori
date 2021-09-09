#ifndef NODETRACKER_H
#define NODETRACKER_H

#include "node.h"
#include "piece.h"
#include <array>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>

template<int location, int pos, typename Tracker>
struct PosTracker {
    std::array<Tracker, 11> trackers;

    bool contains(const Node& node) const {
        if(location == 0) {
            short p = static_cast<short>(node.res0[pos]);
            return trackers[p].contains(node);
        }
        if(location == 1) {
            short p = static_cast<short>(node.res1[pos]);
            return trackers[p].contains(node);
        }
        if(location == 2) {
            short p = static_cast<short>(node.board[pos]);
            return trackers[p].contains(node);
        }
        return false;
    }

    void push(const Node& node) {
        if(location == 0) {
            short p = static_cast<short>(node.res0[pos]);
            trackers[p].push(node);
        }
        if(location == 1) {
            short p = static_cast<short>(node.res1[pos]);
            trackers[p].push(node);
        }
        if(location == 2) {
            short p = static_cast<short>(node.board[pos]);
            trackers[p].push(node);
        }
    }
};

struct TrivialTracker {
    std::set<size_t> hashes;

    bool contains(const Node& node) const { return hashes.contains(node.hash()); }

    void push(const Node& node) { hashes.insert(node.hash()); }
};




struct NodeTracker {

    using Tracker = PosTracker<0, 0, 
                        PosTracker<0, 1, 
                            TrivialTracker
                        >
                    >;
    Tracker tracker;

    bool contains(const Node& node) const { return tracker.contains(node); }

    void push(const Node& node) { tracker.push(node); }


};


#endif