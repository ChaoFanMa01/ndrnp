#ifndef NDRNP_MISC_H
#define NDRNP_MISC_H

#include <vector>
#include <set>

#include "header.h"
#include "node.h"
#include "graph.h"
#include "graph_misc.h"
#include "prr.h"

namespace ndrnp {
    void
    restore_nodes(const std::vector<Node*>& nds,
                  const std::vector<Node::power_type>& ps,
                  const std::vector<Node::power_type>& pr,
                  const std::vector<hop_type>& hs) {

        nds[0]->set_power(5.0);
        for (size_type i = 1; i < sensor_num + 1; ++i) {
            nds[i]->set_hop(hs[i - 1]);
            nds[i]->set_power(ps[i - 1]);
        }
        for (size_type i = sensor_num + 1; i < nds.size(); ++i) {
            nds[i]->set_hop(9999);
            nds[i]->set_power(pr[i - sensor_num - 1]);
        }
    }

    hop_type max_hop(const AdjacencyList<Node*>& res,
                     const std::vector<size_type>& dests) {
        hop_type max = 0;
        for (auto &d : dests)
            if (max < res[d].data()->hop())
                max = res[d].data()->hop();
        return max;
    }

    bool meet_hop(const AdjacencyList<Node*>& al, const size_type& src,
                  const std::vector<size_type>& dests) {
        for (auto &i : dests)
            if (al[i].weight() > al[i].data()->hop())
                return false;
        return true;
    }

    int
    total_delta(const std::vector<Node*>& nds) {
        int total = 0;
        for (auto &n : nds)
            if (n->type() == NodeType::SENSOR)
                total += n->hop();
        return total;
    }

    double
    average_hop(const AdjacencyList<Node*>& al,
                const std::vector<size_type>& dests) {
        double    hop = 0.0; 
        for (auto &d : dests)
            for (size_type p = d; al[p].parent() != 0; 
                 p = al[p].parent())
                ++hop;
        hop += 10.0;
        return hop / dests.size();
    }
    double 
    average_energy(const AdjacencyList<Node*>& al, 
                   const std::vector<size_type>& dests,
                   int i) {
        double hop = 0.0, energy;
        std::set<size_type>   num;
        for (auto &d : dests)
            for (size_type p = d; al[p].parent() != 0;
                 p = al[p].parent()) {
                ++hop; num.insert(p);
            }
        hop += 10.0;
        switch (i) {
            case 0:
                energy = hop * 26; break;
            case 1:
                energy = hop * 25; break;
            case 2:
                energy = hop * 24; break;

        }
        energy += hop * 24;
        return energy / (num.size() + 1.0);
    }

    double
    average_delay(const AdjacencyList<Node*>& al,
                  const std::vector<size_type>& dests) {
        return average_hop(al, dests) * LINK_DELAY;
    }

    double
    average_prr(const AdjacencyList<Node*>& al,
                const std::vector<size_type>& dests) {
      double pr = 0.0;
      for (auto& d : dests) {
          double p_prr = 1.0, tmp;
          for (size_type p = d; p != 0; p = al[p].parent()) {
              tmp = prr(al[p].data()->power(), 
                       distance(*(al[p].data()), *(al[al[p].parent()].data())));
              p_prr *= tmp;
          }
          pr += p_prr;
      }
      return pr / dests.size();
  }
}

#endif
