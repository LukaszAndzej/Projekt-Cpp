#ifndef GRAPH_AS_MATRIX_H
#define GRAPH_AS_MATRIX_H

#include "Graph.h"
#include <vector>
#include <set>
#include <algorithm>
#include <stack>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace Color {
    enum Code {
        RESET       =  0,
        BOLD        =  1,
        FG_GREEN    = 32,
        FG_L_YELLOW = 33,
        FG_DEFAULT  = 39
    };

    class Modifier {
            Code code;
        public:
            Modifier(Code pCode) : code(pCode) {}
            friend std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
                return os << "\033[" << mod.code << "m";
            }
    };
}

class GraphAsMatrix : public Graph {
    
    public:
        GraphAsMatrix(const int n);

        GraphAsMatrix(const int n, const std::string& filename) : GraphAsMatrix(n) {
            readData(filename);
        }

        ~GraphAsMatrix() { clear();}

        void clear();
        void add_edge(int v_outgoing, int v_incoming, int weight) override;
        void add_edge(int v_outgoing, int v_incoming) override { add_edge(v_outgoing, v_incoming, 0);}
        int get_all_vertex() { return numberOfAllVertex.size();}
        bool is_edge(int v_outgoing, int v_incoming) override { return (select_edge(v_outgoing, v_incoming)) ? true : false;}
        Vertex* select_vertex(int idx) { if (idx < this->number_of_vertices) return vertices_list[idx];}
        Edge* select_edge(int v_outgoing, int v_incoming) const {
            return (v_outgoing < this->number_of_vertices && v_incoming < this->number_of_vertices) ?
                    adjacency_matrix[v_outgoing][v_incoming] : nullptr;
        }
        std::vector<std::vector<int>> find_cycles();
        class Log;
    private:
        std::vector<Vertex *> vertices_list;
        std::vector<std::vector<Edge*>> adjacency_matrix;
        std::vector<Edge*> edgesContainer;
        std::set<int> numberOfAllVertex;

        VertexIterator& vertices() override;
        EdgeIterator& edges() override;
        //! zwraca iterator przeglądający wszystkie krawędzie wychodzące z podanego wierzchołka
        EdgeIterator& emanating_edges(const int vertex) override;
        //! zwraca iterator przeglądający wszystkie krawędzie wchodzące do podanego wierzchołka
        EdgeIterator& incident_edges(const int vertex) override;
        VertexIterator& begin();
        EdgeIterator begin(int raw_idx);
        VertexIterator& end();
        EdgeIterator& end(int i);
        
        void displayEdges();
        void readData(const std::string& filename);
        void dfs(std::vector<std::vector<Edge*>>& adjacency_matrix_copy, int v,
                std::vector<int>& path, std::vector<bool>& visited,
                Edge* lastEdge, std::vector<std::vector<int>>& cycles);
};

class GraphAsMatrix::Log {

    public:
        static void Info(std::string message);

    private:
        static std::string currentDateTime();
};

void GraphAsMatrix::Log::Info(std::string message) {
    // Otwieramy plik do zapisu w trybie dopisywania.
    std::ofstream file("Logi.txt", std::ios::app);

    if (!file) {
        // W przypadku błędu podczas otwierania pliku, wypisujemy komunikat.
        std::cout << "Błąd podczas otwierania pliku." << std::endl;
        return;
    }

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();

    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);
    Color::Modifier yellow(Color::FG_L_YELLOW);
    Color::Modifier bold(Color::BOLD);
    Color::Modifier reset(Color::RESET);

    //* wyświetla w konsoli
    std::cout<<"["<<yellow<<currentDateTime()<<'.'
        <<std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000 
            <<def<<"]"<<green<<" [Info] "<<def<<bold<<message<<reset<<'\n';

    //* zapisuje do pliku logi.txt
    file << "[" << currentDateTime() << '.'
        << std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000
        << "]" << " [Info] " << message << '\n';

    file.close();
}

std::string GraphAsMatrix::Log::currentDateTime() {
    time_t now = time(0);
    struct tm time_struct;
    char buf[80];
    time_struct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &time_struct);

    return buf;
}

GraphAsMatrix::GraphAsMatrix(const int n) : vertices_list(n), adjacency_matrix(n), Graph(n) {
    Log::Info("Create Graph with size = " + std::to_string(n));
    for (unsigned int i = 0; i < n; i++) {
        vertices_list[i] = new Vertex(i);
    }

    for (unsigned int i = 0; i < n; i++) {
        adjacency_matrix[i].resize(n);
    }

    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n; j++) {
            adjacency_matrix[i][j] = nullptr; 
        }
    }

    // czyszczenie pliku Logi.txt
    std::ofstream file("Logi.txt", std::ios::trunc);

    if (!file) {
        std::cout << "Błąd podczas otwierania pliku." << std::endl;
        return;
    }

    file.close();
}

void GraphAsMatrix::clear() {
    for (Vertex *vertex : vertices_list) {
        delete vertex;
    }
    vertices_list.clear();

    for (std::vector<Edge *> &row : adjacency_matrix) {
        for (Edge *edge : row) {
            delete edge;
        }
        row.clear();
    }
    adjacency_matrix.clear();

    if (edgesContainer.size() != 0) {
        for (Edge *edge : edgesContainer) {
            delete edge;
        }
        edgesContainer.clear();
    }
    numberOfAllVertex.clear();
}

void GraphAsMatrix::add_edge(int v_outgoing, int v_incoming, int weight) {
    Log::Info("Adding edge (" + std::to_string(v_outgoing) + ", " + std::to_string(v_incoming) + ")");
    if (v_outgoing < this->number_of_vertices && v_incoming < this->number_of_vertices) {
        if (!adjacency_matrix[v_outgoing][v_incoming]) {
            adjacency_matrix[v_outgoing][v_incoming] =
                    new Edge(vertices_list[v_outgoing], vertices_list[v_incoming], weight);
            this->number_of_edges++;
            numberOfAllVertex.insert(v_outgoing);
            numberOfAllVertex.insert(v_incoming);
        }
    }  
}

std::vector<std::vector<int>> GraphAsMatrix::find_cycles() {
    std::vector<std::vector<Edge*>> adjacency_matrix_copy = adjacency_matrix;
    std::vector<std::vector<int>> cycles;

    for (int v = 0; v < number_of_vertices; v++) {
        std::vector<int> path;
        std::vector<bool> visited(number_of_vertices, false);
        Edge *lastEdge = nullptr;

        dfs(adjacency_matrix_copy, v, path, visited, lastEdge, cycles);
    }

    Log::Info("Display cycles");
    std::ofstream file("Logi.txt", std::ios::app);

    if (!file) {
        // W przypadku błędu podczas otwierania pliku, wypisujemy komunikat.
        std::cout << "Błąd podczas otwierania pliku." << std::endl;
    }

    int idx = 0;
    for (std::vector<int> cycle: cycles) {
        std::cout << std::setw(33) << "";
        file << std::setw(33) << "";

        std::cout<<"Cycle "<<idx<<": ";
        file << "Cycle "<<idx<<": ";

        idx++;

        for (int c: cycle) {
            std::cout<<c + 1<<", ";
            file <<c + 1<<", ";
        }
        std::cout<<'\n';
        file <<'\n';
    }

    file.close();

    return cycles;
}

        
GraphAsMatrix::VertexIterator& GraphAsMatrix::vertices() {
    VertexIterator *itr = new VertexIterator(&vertices_list[0]);
    return *itr;
}

GraphAsMatrix::EdgeIterator& GraphAsMatrix::edges() {
    edgesContainer.clear();
    for (std::vector<Edge*> vector : adjacency_matrix) {
        for (Edge *edge : vector) {
            if (edge) edgesContainer.push_back(edge);
        }
    }

    EdgeIterator *itr = new EdgeIterator(edgesContainer.data());
    return *itr;
}

//! zwraca iterator przeglądający wszystkie krawędzie wychodzące z podanego wierzchołka
GraphAsMatrix::EdgeIterator& GraphAsMatrix::emanating_edges(const int vertex) {

    edgesContainer.clear();
    for (Edge* edge : adjacency_matrix[vertex]) {
        if (edge) edgesContainer.push_back(edge);
    }

    EdgeIterator *itr = new EdgeIterator(edgesContainer.data());
    return *itr;
}

//! zwraca iterator przeglądający wszystkie krawędzie wchodzące do podanego wierzchołka
GraphAsMatrix::EdgeIterator& GraphAsMatrix::incident_edges(const int vertex) {
    edgesContainer.clear();
    for (size_t i = 0; i < adjacency_matrix.size(); i++) {
        if (adjacency_matrix[i][vertex]) edgesContainer.push_back(adjacency_matrix[i][vertex]);
    }

    EdgeIterator *itr = new EdgeIterator(edgesContainer.data());
    return *itr;
}

GraphAsMatrix::VertexIterator& GraphAsMatrix::begin() {
    VertexIterator *itr = new VertexIterator(&vertices_list[0]);
    return *itr;
}

GraphAsMatrix::EdgeIterator GraphAsMatrix::begin(int raw_idx) {
    if (raw_idx < number_of_vertices) {
        return EdgeIterator(&adjacency_matrix[raw_idx][0]);
    }
    return EdgeIterator(nullptr);
}

GraphAsMatrix::VertexIterator& GraphAsMatrix::end() {
    const size_t size = vertices_list.size();
    VertexIterator *itr = new VertexIterator(&vertices_list[size]);
    return *itr;
}

GraphAsMatrix::EdgeIterator& GraphAsMatrix::end(int i) {
    EdgeIterator *itr = new EdgeIterator(&edgesContainer[edgesContainer.size()]);
    return *itr;
}

void GraphAsMatrix::displayEdges() {
    Log::Info("Display graph");
    // Otwieramy plik do zapisu w trybie dopisywania.
    std::ofstream file("Logi.txt", std::ios::app);

    if (!file) {
        // W przypadku błędu podczas otwierania pliku, wypisujemy komunikat.
        std::cout << "Błąd podczas otwierania pliku." << std::endl;
        return;
    }

    for (std::vector<Edge*> edges: adjacency_matrix) {
        std::cout << std::setw(33) << "";
        file << std::setw(33) << "";
        for (Edge *edge: edges) {
            if(edge) {
                std::cout<<"("<<(*edge).get_outgoing_vertex()->get_index()<<" - "<<(*edge).get_incoming_vertex()->get_index()<<")    ";
                file <<"("<<(*edge).get_outgoing_vertex()->get_index()<<" - "<<(*edge).get_incoming_vertex()->get_index()<<")    ";
            } else {
                std::cout<<"(# - #)"<<"    ";
                file <<"(# - #)"<<"    ";
            }
        }
        std::cout<<'\n';
        file <<'\n';
    }
    std::cout<<'\n';
    file <<'\n';
    file.close();
}

void GraphAsMatrix::dfs(std::vector<std::vector<Edge*>>& adjacency_matrix_copy, int v,
        std::vector<int>& path, std::vector<bool>& visited,
        Edge* lastEdge, std::vector<std::vector<int>>& cycles) {
    visited[v] = true;
    path.push_back(v);

    for (Edge* edge : adjacency_matrix_copy[v]) {
        if (edge && edge != lastEdge) {
            int mate = edge->get_mate(vertices_list[v])->get_index();

            if (!visited[mate]) {
                dfs(adjacency_matrix_copy, mate, path, visited, lastEdge, cycles);
            } else {
                std::vector<int>::iterator cycleStart = std::find(path.begin(), path.end(), mate);
                if (cycleStart != path.end()) {
                    std::vector<int> cycle(cycleStart, path.end());
                    cycles.push_back(cycle);
                    adjacency_matrix_copy[v][mate] = nullptr;
                }
            }
        }
    }

    path.pop_back();
    visited[v] = false;
}
        
void GraphAsMatrix::readData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Błąd podczas otwierania pliku." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int x, y = -1;
        if (!(iss >> x >> y)) {
            std::cout << "Błąd podczas odczytu danych." << std::endl;
            break;
        }

        add_edge(x, y);
    }

    displayEdges();

    file.close();
}

#endif

