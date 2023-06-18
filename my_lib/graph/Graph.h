#ifndef GRAPH_H
#define GRAPH_H

#include "Edge.h"
#include "Iterator.h"

class Graph {
    public:
        using EdgeIterator = Iterator<Edge>;
        using VertexIterator = Iterator<Vertex>;

        Graph(const int vertex) : number_of_vertices(vertex) {};
        ~Graph() {};
        int get_number_of_vertices() { return number_of_vertices;} //* number of vertices in Graph
        int get_number_of_edges() { return number_of_edges;} //* number of edges in Graph

        virtual void add_edge(int v_outgoing, int v_incoming) = 0; //* create new edge from vertex v_outgoing to v_incoming
        virtual void add_edge(int v_outgoing, int v_incoming, int weight) = 0; //* create new edge from vertex v_outgoing to v_incoming
        virtual bool is_edge(int v_outgoing, int v_incoming) = 0; //* return true if graph has a edge
        virtual Edge* select_edge(int v_outgoing, int v_incoming) const = 0; //* return pointer to edge which has v_outgoing and v_incoming vertices
        virtual VertexIterator& vertices() = 0; //* return Iterator that goes through all the vertices
        virtual EdgeIterator& edges() = 0; //* return Iterator that goes through all the edges
        virtual EdgeIterator& emanating_edges(const int vertex) = 0; // zwraca iterator przeglądający wszystkie krawędzie wychodzące z podanego wierzchołka
        virtual EdgeIterator& incident_edges(const int vertex) = 0; // zwraca iterator przeglądający wszystkie krawędzie wchodzące do podanego wierzchołka
        EdgeIterator& emanating_edges(Vertex &vertex) { return emanating_edges(vertex.get_index());}
        EdgeIterator& incident_edges(Vertex &vertex) { return incident_edges(vertex.get_index());}
    protected:
        int number_of_vertices = 0;
        int number_of_edges = 0;
};
#endif