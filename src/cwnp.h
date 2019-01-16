#ifndef NDRNP_CWNP_H
#define NDRNP_CWNP_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <set>

#include "header.h"
#include "node.h"
#include "graph.h"
#include "graph_misc.h"
#include "cover.h"
#include "rrnp_misc.h"

namespace ndrnp {
    std::set<size_type>
    cwnp(const std::vector<Node *>& nds, const size_type& size) {
        AdjacencyList<Node*> res(nds.begin(), nds.end());
        
        size_type src;
        std::vector<size_type> dests;
        bool flag = false;
        std::set<size_type> y_hat;
        std::vector<hop_type> deltas;
        
        // save original hop constraints for restoration.
        for (auto &n : nds)
            deltas.push_back(n->hop());

        // find the id of sink, and ensure that only
        // one sink is given.
        for (auto &n : nds)
            if (n->type() == NodeType::SINK) {
               if (flag == true)
                    throw std::range_error("Multiple sinks are given");
                flag = true;
                src = n->id();
            }

        // make all sensors as destinations.
        for (auto &n : nds)
            if (n->type() == NodeType::SENSOR)
                dests.push_back(n->id());
        // build a graph only having edges bewteen sensors
        // and sinks.
        AdjacencyList<Node*>  tmp(nds.begin(), 
                       nds.begin() + dests.size() + 1), spt;
        try {
            // check whether a connected shortest path tree
            // can be built on this graph.
            spt = dijkstra_spt(tmp, src, dests);
            // if a connected graph is built using only sensors and sink,
            // we check whether this graph meet delay constraints.
            if (meet_hop(tmp, src, dests))
                // if delay constraints are fulfilled, an empty set 
                // will be returned.
                return std::set<size_type>();
            else
                throw std::range_error("sensor tree exceeds constraints.");
        // if cannot build such graph, try to build with relays.
        } catch (std::range_error e) {
#if !defined(NDEBUG)
;//             std::cerr << e.what() << std::endl;
#endif
        }

        try {
            // build a shortest path tree from given source to
            // all given destinations on the graph, and record
            // the shortest distances (i.e., least hops) between
            // the sink and all other nodes, including sensors
            // and relays, in their weight fields.
            spt = dijkstra_spt(res, src, dests);
        } catch (std::range_error e) {
#if !defined(NDEBUG)
;//            std::cerr << e.what() << std::endl;
#endif      
            return std::set<size_type>();
        }

        if (meet_hop(res, src, dests)) {
// main step begins.
            int DELTA = max_hop(res, dests);
            int k = 0;
            std::set<size_type> ik, tmp;
            for (auto &e : dests)
                ik.insert(e);
            // main loop.
            while (!ik.empty()) {
                if (k++ > DELTA)
                    return std::set<size_type>();
                Cover<size_type, size_type> cvr;
                // for each node in u, find the node that can be effectively 
                // covered by it from ik.
                for (auto &v : res)
                    for (auto &e : v.neighbors())
                            // check whether this node (in u) is a neighbor of a node (in ik)
                        if (index_of(ik.begin(), ik.end(), e.end()->id()) != -1 &&
                            // check whether the delay constraint is met.
                            v.weight() < e.end()->data()->hop())
                            cvr.insert_family(v.id(), e.end()->id());
                for (auto &e : ik)
                    cvr.insert_set(e);
                // find minimum set cover.
                tmp = cvr.weight_set_cover(size);
                if (tmp.empty())
                    throw std::range_error("no cover is found");
                 // for each node in minimum set cover update its delay constraint.
                for (auto &e : tmp)
                    for (auto &p : cvr.family()[e])
                        if (res[e].data()->hop() > res[p].data()->hop() - 1)
                            res[e].data()->set_hop(res[p].data()->hop() - 1);
                // record the placed relay nodes.
                for (auto &e : tmp)
                    if (res[e].data()->type() == NodeType::CDL)
                        y_hat.insert(e);
                // delete the nodes that are neighbors of the src from tmp.
                tmp.erase(src);
                for (auto &e : res[src].neighbors())
                    tmp.erase(e.end()->id());
                ik = tmp;
            }
        } else {
            return std::set<size_type>();
        }

        for (auto &n : nds)
            if (n->type() == NodeType::CDL && index_of(y_hat.begin(), y_hat.end(), n->id()) == -1)
                n->set_power(0.0);

        // restore the delay constraint of each node.
        for (int i = 0; i < nds.size(); ++i)
            nds[i]->set_hop(deltas[i]);
// try to delete each selected relay node.
        for (auto &yy : y_hat) {
            coordinate_type    r = nds[yy]->power();
            nds[yy]->set_power(0.0);
            AdjacencyList<Node*>    al(nds.begin(), nds.end());
            try {
                dijkstra_spt(al, src, dests);
                if (!meet_hop(al, src, dests))
                    throw std::range_error("delay is not met");
            } catch (std::range_error e) {
//                 std::cout << e.what() << std::endl;
                 nds[yy]->set_power(r);
            }
        }

        y_hat.clear();
        for (auto &n : nds)
           if (n->type() == NodeType::CDL && n->power() > 0.0)
                y_hat.insert(n->id());
        return y_hat;
    }
}

#endif
