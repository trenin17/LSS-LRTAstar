#ifndef NODE_H
#define NODE_H

//That's the data structure for storing a single search node.
//You MUST store all the intermediate computations occuring during the search
//incapsulated to Nodes (so NO separate arrays of g-values etc.)

struct Node
{
    int     i, j; //grid cell coordinates
    double  F, g, H; //f-, g- and h-values of the search node
    Node    *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)
    Node(int i, int j): i(i), j(j), F(0), g(0), H(0), parent(nullptr) {}
    Node(int i, int j, double g): i(i), j(j), F(g), g(g), H(0), parent(nullptr) {}
    Node(std::pair<int, int> p): i(p.first), j(p.second), F(0), g(0), H(0), parent(nullptr) {}
    Node(): i(-1), j(-1), F(0), g(0), H(0), parent(nullptr) {}
    Node(const Node &other) {
        i = other.i;
        j = other.j;
        F = other.F;
        g = other.g;
        H = other.H;
        parent = other.parent;
    }
//    Node(Node &other) {
//        i = other.i;
//        j = other.j;
//        F = other.F;
//        g = other.g;
//        H = other.H;
//        pari = other.pari;
//        parj = other.parj;
//        parent = other.parent;
//    }
//    Node& operator = (Node &other) {
//        i = other.i;
//        j = other.j;
//        F = other.F;
//        g = other.g;
//        H = other.H;
//        pari = other.pari;
//        parj = other.parj;
//    }
};
#endif
