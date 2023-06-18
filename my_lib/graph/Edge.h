#ifndef EDGE_H
#define EDGE_H

#include "Vertex.h"

class Edge {
    public:
        Edge(Vertex *v_0, Vertex *v_1) : outgoing(v_0), incoming(v_1) {}
        Edge(Vertex *v_0, Vertex *v_1, int weight) : outgoing(v_0), incoming(v_1), _weight(weight) {}
        Vertex *get_outgoing_vertex() const { return outgoing;}
        Vertex *get_incoming_vertex() const { return incoming;}
        /**
         * @brief Method returns the adjacent vertex if it belongs to the edge.
         * @note Example:
         * @note    vertex = (1) and edge = (1) <-----> (2)
         * @note    than method return (2)
         * 
         * @param vertex 
         * @return Vertex* 
         */
        Vertex *get_mate(Vertex *vertex) const {
            // We can not simplify this to checking just one condition because
            // vertex does not need to belong to this Edge.
            if (vertex->get_index() == outgoing->get_index()) return incoming;
            else if(vertex->get_index() == incoming->get_index()) return outgoing;

            return nullptr;
        }

        int get_weight() const { return _weight;}
    protected:
        Vertex *outgoing = nullptr; // from vertex v_0
        Vertex *incoming = nullptr; // to vertex v_1
    private:
        int _weight = 0;
};
#endif