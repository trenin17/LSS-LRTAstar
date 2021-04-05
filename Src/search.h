#ifndef SEARCH_H
#define SEARCH_H
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <list>
#include <vector>
#include <math.h>
#include <limits>
#include <chrono>
#include <set>
#include <unordered_map>

class Search
{
    public:
        Search();
        ~Search(void);
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);
        void countHeuristicFunc(Node *v, const Map &map, const EnvironmentOptions &options);
        std::list<std::pair<int,int>> returnSuccessors(const Node *v, const Map &Map, const EnvironmentOptions &options);
        void OPEN_insert(const Node* v, bool is_new);
        void Astar(const Map &map, const EnvironmentOptions &options, Node* st);
        void OPEN_erase(const Node* v, bool perm);
        void OPEN_clear();
        void updateHeuristic(const Map &map, const EnvironmentOptions &options);
    
    protected:
        //CODE HERE

        //Hint 1. You definetely need class variables for OPEN and CLOSE

        //Hint 2. It's a good idea to define a heuristic calculation function, that will simply return 0
        //for non-heuristic search methods like Dijkstra

        //Hint 3. It's a good idea to define function that given a node (and other stuff needed)
        //will return it's sucessors, e.g. unordered list of nodes

        //Hint 4. working with OPEN and CLOSE is the core
        //so think of the data structures that needed to be used, about the wrap-up classes (if needed)
        //Start with very simple (and ineffective) structures like list or vector and make it work first
        //and only then begin enhancement!
    std::set< std::pair< std::pair<double, double>, std::pair<int, int> > > OPEN;
    std::set<std::pair<int, std::pair<int, int>>> OPEN_H;
    struct hasher {
        size_t operator () (const std::pair<int, int>& p) const {
            return 1ll * INT32_MAX * p.first + p.second;
        }
    };
    std::unordered_map<std::pair<int, int>, Node*, hasher> OPEN_nodes;
    std::unordered_map<std::pair<int, int>, Node*, hasher> CLOSED;
    std::unordered_map<std::pair<int, int>, Node, hasher> gen;
    Node *start;
    Node *goal;
//    std::list<Node> successors(const Node *v, const Map &Map) {
//
//    }

        SearchResult                    sresult; //This will store the search result
        std::list<Node>                 lppath, hppath; //

        //CODE HERE to define other members of the class
};
#endif
