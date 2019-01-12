#ifndef NDRNP_GRAPH_H
#define NDRNP_GRAPH_H

#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <climits>    // INT_MAX
#include <cfloat>     // DBL_MAX

#include "node.h"     // is_neighbor()

namespace ndrnp {
// data type predeclarations.
    template <class D> class Vertex;
    template <class D> class Edge;
    template <class D> class AdjacencyList;
    enum class vertex_type: uint8_t;
    enum class vertex_status: uint8_t;

// function declarations.
    template <class D>
    std::ostream& operator<<(std::ostream&, const Edge<D>&);
    template <class D>
    std::ostream& operator<<(std::ostream&, const Vertex<D>&);
    template <class D>
    std::ostream& operator<<(std::ostream&, const AdjacencyList<D>&);

// data type definitions.
    /* @enum vertex_type
     * Type identifying vertex type, i.e.:
     * 0 - a destination vertex,
     * 1 - a source vertex,
     * 2 - a mediate vertex.
     */
    enum class vertex_type: uint8_t {
        DESTINATION,
        SOURCE,
        MEDIATE
    };

    /* @enum vertex_status
     * Type denoting the status of this vertex, i.e.:
     * 0 - selected vertex,
     * 1 - unselected vertex.
     */
    enum class vertex_status: uint8_t {
        SELECTED,
        UNSELECTED
    };

    /* @class Edge
     * Type denoting an edge joining two vertices.
     */
    template <class D>
    class Edge {
    public:
        typedef Vertex<D>     vertex_type;
        typedef double        weight_type;

        Edge() = delete;
        Edge(vertex_type* e, const weight_type& w = weight_type())
        : _end(e), _weight(w) {}
        Edge(const Edge&);
        Edge(Edge&&);
        ~Edge() { _end = nullptr; }

        Edge& operator=(const Edge&);
        Edge& operator=(Edge&&);

        bool operator==(const Edge& e) const { 
            return _end == e._end;
        }
        bool operator!=(const Edge& e) const { return !(*this == e); }

        bool operator>(const Edge& e) const { return _weight > e._weight; }
        bool operator>=(const Edge& e) const { return _weight >= e._weight; }
        bool operator<(const Edge& e) const { return _weight < e._weight; }
        bool operator<=(const Edge& e) const { return _weight <= e._weight; }

        vertex_type* end() const { return _end; }
        weight_type  weight() const { return _weight; }

        void set_end(vertex_type* e) { _end = e; }
        void set_weight(const weight_type& w) { _weight = w; }
    private:
        // one end incident with this edge, and the other
        // end is the vertex storing this edge.
        vertex_type*      _end;
        weight_type       _weight;
    };
/*
    template <class D>
    Edge<D>::Edge(vertex_type* e, const weight_type& w = weight_type())
    : _end(e), _weight(w) {}
*/
    template <class D>
    Edge<D>::Edge(const Edge& e)
    : _end(e._end), _weight(e._weight) {}

    template <class D>
    Edge<D>::Edge(Edge&& e)
    : _end(std::move(e._end)), _weight(std::move(e._weight)) {
        e._end = nullptr;
    }

    template <class D>
    Edge<D>&
    Edge<D>::operator=(const Edge& e) {
        _end = e._end; _weight = e._weight;
        return *this;
    }

    template <class D>
    Edge<D>&
    Edge<D>::operator=(Edge&& e) {
        _end = std::move(e._end);
        _weight = std::move(e._weight);
        e._end = nullptr;

        return *this;
    }

    /* @class Vertex
     * Type denoting a vertex in graph.
     */
    template <class D>
    class Vertex {
    public:
        typedef D                                                    data_type;
        typedef int32_t                                              weight_type;
        typedef typename std::vector<Edge<data_type>>::size_type     size_type;
        typedef int32_t                                             id_type;

        Vertex() = delete;
        Vertex(const data_type& d, const vertex_type& t,
               const vertex_status& s, const id_type& i,
               const weight_type& w = 9999, const id_type& p = -1,
               const std::vector<Edge<data_type>>& n = std::vector<Edge<data_type>>())
        : _data(d), _type(t), _status(s), _id(i), 
          _weight(w), _parent(p), _neighbors(n) {}
        Vertex(const Vertex&);
        Vertex(Vertex&&);
        ~Vertex() { _data = data_type(); }

        Vertex& operator=(const Vertex&);
        Vertex& operator=(Vertex&&);

        bool operator==(const Vertex& v) const { return _id == v._id; }
        bool operator!=(const Vertex& v) const { return !(*this == v); }
        bool operator>(const Vertex& v) const { return _weight > v._weight; }
        bool operator>=(const Vertex& v) const { return _weight >= v._weight; }
        bool operator<(const Vertex& v) const { return _weight < v._weight; }
        bool operator<=(const Vertex& v) const { return _weight <= v._weight; }

        data_type                     data() const { return _data; }
        vertex_type                   type() const { return _type; }
        vertex_status                 status() const { return _status; }
        id_type                       id() const { return _id; }
        weight_type                   weight() const { return _weight; }
        id_type                       parent() const { return _parent; }
        std::vector<Edge<data_type>>  neighbors() const { return _neighbors; }
        std::vector<Edge<data_type>>& neighbors() { return _neighbors; }

        void set_data(const data_type& d) { _data = d; }
        void set_type(const vertex_type& t) { _type = t; }
        void set_status(const vertex_status& s) { _status = s; }
        void set_id(const id_type& i) { _id = i; }
        void set_weight(const weight_type& w) { _weight = w; }
        void set_parent(const id_type& p) { _parent = p; }

        void push_neighbor(Vertex& v) { _neighbors.push_back(Edge<D>(&v)); }
        void push_neighbor(Vertex& v, typename Edge<D>::weight_type& w) {
            _neighbors.push_back(Edge<D>(&v, w));
        }
        void pop_neighbor() { _neighbors.pop_back(); }
        void clear_neighbor() { _neighbors.clear(); }
        size_type neighbor_size() const { return _neighbors.size(); }

    private:
        // data storing in this vertex.
        data_type                       _data;
        // type of this vertex.
        vertex_type                     _type;
        // status of this vertex.
        vertex_status                   _status;
        // id of this vertex
        id_type                         _id;
        weight_type                     _weight;
        // id of the parent of this vertex.
        id_type                         _parent;
        // edges joining this vertex.
        std::vector<Edge<data_type>>    _neighbors;
    };

    template <class D>
    Vertex<D>::Vertex(const Vertex& v)
    : _data(v._data), _type(v._type), _status(v._status),
      _id(v._id), _weight(v._weight), _parent(v._parent),
      _neighbors(v._neighbors) {}

    template <class D>
    Vertex<D>::Vertex(Vertex&& v)
    : _data(std::move(v._data)), _type(std::move(v._type)),
      _status(std::move(v._status)), _id(std::move(v._id)),
      _weight(std::move(v._weight)), _parent(std::move(v._parent)),
      _neighbors(std::move(v._neighbors)) {
        v._data = data_type();
    }

    template <class D>
    Vertex<D>&
    Vertex<D>::operator=(const Vertex& v) {
        _data = v._data; _type = v._type; _status = v._status;
        _id = v._id; _weight = v._weight; _parent = v._parent;
        _neighbors = v._neighbors;

        return *this;
    }

    template <class D>
    Vertex<D>&
    Vertex<D>::operator=(Vertex&& v) {
        _data = std::move(v._data);
        _type = std::move(v._type);
        _status = std::move(v._status);
        _id = std::move(v._id);
        _weight = std::move(v._weight);
        _parent = std::move(v._parent);
        _neighbors = std::move(v._neighbors);

        v._data = data_type();

        return *this;
    }

    /* @class AdjacencyList
     * Implementation of adjacency list of a graph.
     */
    template <class D>
    class AdjacencyList {
    public:
        typedef D                                                       data_type;
        typedef typename std::vector<Vertex<data_type>>::size_type      size_type;
        typedef typename std::vector<Vertex<data_type>>::iterator       iterator;
        typedef typename std::vector<Vertex<data_type>>::const_iterator const_iterator;

        AdjacencyList(): vertices(std::vector<Vertex<data_type>>()) {}
        AdjacencyList(const AdjacencyList& al): vertices(al.vertices) {}
        AdjacencyList(AdjacencyList&& al): vertices(std::move(al.vertices)) {}
        template <class Iter> AdjacencyList(Iter, Iter);
        ~AdjacencyList() = default;

        AdjacencyList& operator=(const AdjacencyList&);
        AdjacencyList& operator=(AdjacencyList&&);

        bool operator==(const AdjacencyList&) const = delete;
        bool operator!=(const AdjacencyList&) const = delete;
        bool operator>(const AdjacencyList&) const = delete;
        bool operator>=(const AdjacencyList&) const = delete;
        bool operator<(const AdjacencyList&) const = delete;
        bool operator<=(const AdjacencyList&) const = delete;

        Vertex<D>& operator[](size_type i) { return vertices[i]; }
        const Vertex<D>& operator[](size_type i) const { return vertices[i]; }

        iterator       begin() noexcept { return vertices.begin(); }
        const_iterator begin() const noexcept { return vertices.begin(); }
        iterator       end() noexcept { return vertices.end(); }
        const_iterator end() const noexcept { return vertices.end(); }

        void push_back(const Vertex<D>& v) { vertices.push_back(v); }
        void clear() { vertices.clear(); }

        size_type size() const { return vertices.size(); }
    private:
        std::vector<Vertex<data_type>>      vertices;
    };

    template <class D>
    template <class Iter>
    AdjacencyList<D>::AdjacencyList(Iter b, Iter e)
    : vertices(std::vector<Vertex<D>>()) {
        double w;
        for (Iter iter = b; iter != e; ++iter) {
            vertices.push_back(Vertex<D>(*iter, vertex_type::MEDIATE,
                               vertex_status::UNSELECTED, vertices.size()));
        }

        for (size_type i = 0; i < vertices.size(); ++i)
            for (size_type j = 0; j < vertices.size(); ++j)
                if (i != j && (w = is_neighbor(vertices[i].data(),
                                          vertices[j].data())) != -1) {
                    vertices[i].push_neighbor(vertices[j], w);
                 }
    }

    template <class D>
    AdjacencyList<D>&
    AdjacencyList<D>::operator=(const AdjacencyList& al) {
        vertices = al.vertices;
        return *this;
    }

    template <class D>
    AdjacencyList<D>&
    AdjacencyList<D>::operator=(AdjacencyList&& al) {
        vertices = std::move(al.vertices);
        return *this;
    }

    template <class D>
    std::ostream&
    operator<<(std::ostream& os, const Edge<D>& e) {
        os << "[end: " << e.end()->id() << ", weight: "
           << e.weight() << "]";
        return os;
    }

    template <class D>
    std::ostream&
    operator<<(std::ostream& os, const Vertex<D>& v) {
        std::vector<Edge<D>> neighbors = v.neighbors();
        os << "vertex: " << v.id() << std::endl;
        if (neighbors.size() != 0) {
            os << "edges: ";
            for (typename Vertex<D>::size_type i = 0; i < neighbors.size(); ++i)
                os << neighbors[i] << " ";
            std::cout << std::endl;
        } else {
            os << "isolated" << std::endl;
        }
        return os;
    }

    template <class D>
    std::ostream&
    operator<<(std::ostream& os, const AdjacencyList<D>& al) {
        for (typename AdjacencyList<D>::size_type i = 0; i < al.size(); ++i)
            os << al[i] << std::endl;
        return os;
    }
}

#endif
