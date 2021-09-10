#ifndef NODETRACKER_H
#define NODETRACKER_H

#include "node.h"
#include "piece.h"
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
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

    size_t count() const {
        return std::accumulate(trackers.begin(), trackers.end(), 0, [](size_t c, const Tracker& t) { return c + t.count(); });
    }

    std::string toString() const {
        std::string s;
        for(const Tracker& t : trackers) {
            std::string ts = t.toString();
            if(ts.size() > 0) s += ts + " ";
        }
        return s;
    }
};

struct TrivialTracker {
    std::unordered_set<size_t> hashes;

    bool contains(const Node& node) const { return hashes.contains(node.hash()); }

    void push(const Node& node) { hashes.insert(node.hash()); }

    size_t count() const { return hashes.size(); }

    std::string toString() const {
        std::string s;
        if(count() > 0) s += std::to_string(count());
        return s;
    }
};




struct NodeTracker {

    using Tracker = PosTracker<0, 0, 
                        PosTracker<1, 0, 
                            TrivialTracker
                        >
                    >;
    Tracker tracker;

    bool contains(const Node& node) const { return tracker.contains(node); }

    void push(const Node& node) { tracker.push(node); }

    size_t count() const { return tracker.count(); }

    std::string toString() const { return tracker.toString(); }

};


#endif