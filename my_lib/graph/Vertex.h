#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>

class Vertex {
    protected:
        int index = -1;
    public:
        Vertex(int idx) : index(idx) {}
        int get_index() {return index;}
};
#endif