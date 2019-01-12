#ifndef NDRNP_NODE_H
#define NDRNP_NODE_H

#include <iostream>
#include <utility>
#include <cstdlib>
#include <vector>
#include <cmath>       // isnan
#include <cstdint>     // uintx_t

#include "header.h"
#include "coordinate.h"
#include "prr.h"

namespace ndrnp {
// type declarations.
    class Node;
    class Sensor;
    class CDL;
    class Sink;
    class Nodes;
    enum class NodeType: uint8_t;
    enum class CDLStatus: uint8_t;

    const double PRR_CONSTRAINT = 0.95;

// function declarations.

    /* @enum NodeType
     * Enumerate type idendifying the type of a node:
     * 0: sensor node,
     * 1: Candidate Deployment Location ( used to place relay node),
     * 2: sink node.
     */
    enum class NodeType: uint8_t {
        SENSOR, CDL, SINK
    };

    /* @enum CDLStatus
     * Enumerate type identifying the status of a CDL:
     * 0: unselected to place a relay node,
     * 1: selected to place a relay node.
     */
    enum class CDLStatus: uint8_t {
        UNSELECTED, SELECTED
    };

    /* class Node
     * Base class for a wireless node.
     */
    class Node {
    public:
        typedef Coordinate::coordinate_type  coordinate_type;
        typedef double                       power_type;
        typedef int32_t                      id_type;

        Node(const Coordinate&, const NodeType&,
             const power_type&, const hop_type&,
             const id_type&);
        virtual ~Node() = default;
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;

        bool operator==(const Node& rhs) const { return _id == rhs._id; }
        bool operator!=(const Node& rhs) const { return !(*this == rhs); }

        Coordinate coordinate() const { return _coordinate; }
        power_type power() const { return _power; }
        id_type    id() const { return _id; }
        hop_type   hop() const { return _hop; }

        virtual NodeType type() const = 0;
        virtual bool isSelected() const {}

        void set_coordinate(const Coordinate& co) { _coordinate = co; }
        void set_x(const coordinate_type& x) { _coordinate.setX(x); }
        void set_y(const coordinate_type& y) { _coordinate.setY(y); }
        void set_z(const coordinate_type& z) { _coordinate.setZ(z); }
        void set_power(const power_type& p) { _power = p; }
        void set_id(const id_type& id) { _id = id; }
        void set_hop(const hop_type& h) { _hop = h; }

        bool isSensor() const { return _type == NodeType::SENSOR; }
        bool isCDL() const { return _type == NodeType::CDL; }
        bool isSink() const { return _type == NodeType::SINK; }

    protected:
        Coordinate    _coordinate;    /* coordinate of this node */
        NodeType      _type;          /* type of this node */
        /*
         * Transmit power of this node in dBm, it is worth noting
         * that this field stores the absolute value, e.g.,
         * if the transmit power is -15 dBm, _power = 15.
         */
        power_type    _power;
        hop_type      _hop;   /* hop constraint, i.e., delay constraint */
        /* 
         * The universal ID number of this node.
         */
        id_type       _id;
    };

    Node::Node(const Coordinate& co, const NodeType& t,
               const power_type& p, const hop_type& h,
               const id_type& i)
    : _coordinate(co), _type(t), _power(p), _hop(h), _id(i) {}

    /* @class Sensor
     * Wireless sensor node.
     */
    class Sensor: public Node {
    public:
        Sensor(const Coordinate& co, const power_type& p,
               const hop_type& h, const id_type& id)
        : Node(co, NodeType::SENSOR, p, h, id) {}
        ~Sensor() = default;

        NodeType type() const override { return NodeType::SENSOR; }
    };

    /* @class CDL
     * Candidate deployment location used to place a relay node.
     */
    class CDL: public Node {
    public:
        CDL(const Coordinate& co, const power_type& p,
            const hop_type& h, const id_type& id, 
            const CDLStatus& stat = CDLStatus::UNSELECTED)
        : Node(co, NodeType::CDL, p, h, id), _status(stat) {}
        ~CDL()  =default;

        NodeType type() const override { return NodeType::CDL; }
        CDLStatus status() const { return _status; }
        bool isSelected() const { return _status == CDLStatus::SELECTED; }
    private:
        CDLStatus    _status;
    };

    /* @class Sink
     * Wireless sink node.
     */
    class Sink: public Node {
    public:
        Sink(const Coordinate& co, const power_type& p,
             const hop_type& h, const id_type& id)
        : Node(co, NodeType::SINK, p, h, id) {}
        ~Sink() = default;

        NodeType type() const override { return NodeType::SINK; }
    };

    /* @class Nodes
     * A set of wireless nodes.
     * The main purpose of this class is to ensure the 
     * allocated memory will be safely released in case
     * of accident.
     */
    class Nodes {
    public:
        typedef std::vector<Node*>::value_type     value_type;
        typedef std::vector<Node*>::size_type      size_type;
        typedef std::vector<Node*>::iterator       iterator;
        typedef std::vector<Node*>::const_iterator const_iterator;

        Nodes(): nodes(std::vector<Node*>()) {}
        ~Nodes() { clear(); }

        iterator begin() noexcept { return nodes.begin(); }
        const_iterator begin() const noexcept { return nodes.begin(); }
        iterator end() noexcept { return nodes.end(); }
        const_iterator end() const noexcept { return nodes.end(); }

        size_type size() const { return nodes.size(); }
        bool empty() const { return nodes.empty(); }
        value_type& operator[](size_type n) { return nodes[n]; }
        const value_type& operator[](size_type n) const { return nodes[n]; }

        void push_back(const value_type& val) { nodes.push_back(val); }
        void push_back(value_type&& val) { nodes.push_back(std::move(val)); }
        void pop_back() { delete nodes[nodes.size() - 1]; nodes.pop_back(); }
        void clear() { while (!empty()) pop_back(); }

        Node* back() { return nodes.back(); }
    private:
        std::vector<Node*> nodes;
    };

// function definitions.
    /* @fn distance
     * Euclidean distance between two nodes.
     */
    Coordinate::coordinate_type
    distance(const Node& n1, const Node& n2) {
        return distance(n1.coordinate(), n2.coordinate());
    }

    /* @fn is_neighbor
     * This function is used to check whether two wireless
     * nodes can communicate with each other directly. If so,
     * a positive number representing link quality is returned, 
     * otherwise, a negative -1.0 is returned to indicate failure.
     */
    double
    is_neighbor(const Node* n1, const Node* n2) {
        double p;
        if (n1->power() <= 0.0 || n2->power() <= 0.0)
            return -1.0;
        p = prr(n1->power(), distance(*n1, *n2));
        if (std::isnan(p) || p < PRR_CONSTRAINT)
            return -1.0;
        return p;
    }

    std::ostream&
    operator<<(std::ostream& os, const Node& n) {
        os << "[" << (n.type() == NodeType::SENSOR ? "sensor" :
                     (n.type() == NodeType::CDL ? "cdl" : "sink"))
           << " id: " << n.id() << ", power: " << n.power()
           << " hop: " << (int)n.hop()
           << ", " << n.coordinate();
        if (n.isCDL())
            os << (n.isSelected() ? ", selected" : ", unselected");
        os << "]";
        return os;
    }
}

#endif
