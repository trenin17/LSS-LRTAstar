#include "search.h"

Search::Search()
{
//set defaults here
}

Search::~Search() {}

std::list<std::pair<int, int>> Search::returnSuccessors(const Node &v, const Map &Map, const EnvironmentOptions &options)
{
    std::list<std::pair<int,int>> succ;
    std::vector<int> vx = {1, 0, -1, 0};
    std::vector<int> vy = {0, 1, 0, -1};
    for (int i = 0; i < vx.size(); i++) {
        int ni, nj;
        ni = v.i + vx[i];
        nj = v.j + vy[i];
        if (Map.CellOnGrid(ni, nj) && Map.CellIsTraversable(ni, nj)) {
            std::pair<int, int> adjv = {ni, nj};
            succ.push_back(adjv);
        }
    }
    if (options.allowdiagonal != true) {
        return succ;
    }
    std::vector<int> vdx = {1, -1, -1, 1};
    std::vector<int> vdy = {1, 1, -1, -1};
    vx.push_back(1);
    vy.push_back(0);
    for (int i = 0; i < vdx.size(); i++) {
        int ni = v.i + vdx[i], nj = v.j + vdy[i];
        if (!Map.CellOnGrid(ni, nj) || Map.CellIsObstacle(ni, nj)) continue;
        int ai1 = v.i + vx[i], aj1 = v.j + vy[i];
        int ai2 = v.i + vx[i+1], aj2 = v.j + vy[i+1];
        bool f1 = Map.CellOnGrid(ai1, aj1) && Map.CellIsObstacle(ai1, aj1);
        bool f2 = Map.CellOnGrid(ai2, aj2) && Map.CellIsObstacle(ai2, aj2);
        if (!f1 && !f2) {
            std::pair<int, int> adjv = {ni, nj};
            succ.push_back(adjv);
            continue;
        }
        if (f1 && f2) {
            if (options.cutcorners && options.allowsqueeze) {
                std::pair<int, int> adjv = {ni, nj};
                succ.push_back(adjv);
            }
            continue;
        }
        if (options.cutcorners) {
            std::pair<int, int> adjv = {ni, nj};
            succ.push_back(adjv);
        }
    }
    return succ;
}


void Search::countHeuristicFunc(Node &v, const Map &map, const EnvironmentOptions &options)
{
    v.F = v.g + v.H;
    if (options.algorithm != CN_SP_ST_ASTAR) {
        v.H = 0;
        v.F = v.g;
        return;
    }
//
    if (v.H != 0) {
        return;
    }
    std::pair<int, int> goal(map.getGoal());
    double di = abs(goal.first - v.i);
    double dj = abs(goal.second - v.j);
    switch(options.metrictype) {
        case CN_SP_MT_EUCL:
            v.H = sqrt(di*di + dj*dj);
            break;
        case CN_SP_MT_MANH:
            v.H = di + dj;
            break;
        case CN_SP_MT_DIAG:
            v.H = abs(di-dj) + CN_SQRT_TWO * std::min(di, dj);
            break;
        case CN_SP_MT_CHEB:
            v.H = std::max(di, dj);
            break;
        default:
            v.H = 0;
            break;
    }
    v.H *= options.hweight;
    v.F = v.g + v.H;
}

double dist (const Node& v, const Node& u, const EnvironmentOptions &options) {
    int di = abs(v.i-u.i), dj = abs(v.j-u.j);
    switch(options.metrictype) {
        case CN_SP_MT_EUCL:
            return sqrt(di*di + dj*dj);
            break;
        case CN_SP_MT_MANH:
            return di + dj;
            break;
        case CN_SP_MT_DIAG:
            return abs(di-dj) + CN_SQRT_TWO * std::min(di, dj);
            break;
        case CN_SP_MT_CHEB:
            return std::max(di, dj);
            break;
        default:
            return 1;
            break;
    }
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    //need to implement
    auto starttime = std::chrono::high_resolution_clock::now();
    sresult.pathfound = false;
    
    std::pair<int, int> nstart = map.getStart();
    std::pair<int, int> ngoal = map.getGoal();
    
    Node start(nstart.first, nstart.second);
    countHeuristicFunc(start, map, options);
    OPEN.clear();
    OPEN.insert({{start.F, start.H}, {start.i, start.j}});
    OPEN_nodes[{start.i, start.j}] = start;
    CLOSED.clear();
    std::set<std::pair<int, int> > gen;
    gen.insert(nstart);
    Node vvgoal(0,0);
//    std::cout << "####" << '\n';
//    std::cout << options.searchtype << "####\n";
    
    int nsteps = 0;
    start.parent = nullptr;
    
    while (!OPEN.empty()) {
        nsteps++;
        std::pair<int, int> sp = OPEN.begin()->second;
        Node s = OPEN_nodes[sp];
        OPEN.erase(OPEN.begin());
        OPEN_nodes.erase(sp);
        CLOSED[sp] = s;
//        std::cout << s.i << " " << s.j << '\n';
        if (s.i == ngoal.first && s.j == ngoal.second) {
            sresult.pathfound = true;
            vvgoal = s;
            break;
        }
        std::list<std::pair<int, int>> succ = returnSuccessors(s, map, options);
        for (auto it = succ.begin(); it != succ.end(); it++) {
            std::pair<int, int> cur = *it;
            if (map.getValue(cur.first, cur.second) != 0)
                continue;
            if (gen.find(cur) == gen.end()) {
                Node ns(cur);
                ns.g = s.g + dist(ns, s, options);
                ns.parent = &(CLOSED.find({s.i, s.j})->second);
                countHeuristicFunc(ns, map, options);
                OPEN.insert({{ns.F, ns.H}, {ns.i, ns.j}});
                OPEN_nodes[{ns.i, ns.j}] = ns;
                gen.insert(cur);
            } else {
                if (CLOSED.find(cur) != CLOSED.end()) continue;
                Node &ns = OPEN_nodes[cur];
                double Dist = dist(s, ns, options);
                if (ns.g > s.g + Dist) {
                    OPEN.erase({{ns.F, ns.H}, cur});
                    ns.g = s.g+Dist;
                    ns.parent = &(CLOSED.find({s.i, s.j})->second);
                    countHeuristicFunc(ns, map, options);
                    OPEN.insert({{ns.F, ns.H}, cur});
                }
            }
        }
    }
    //auto endtime = std::chrono::steady_clock::now();
    
    sresult.nodescreated =  OPEN.size() + CLOSED.size();
    sresult.numberofsteps = nsteps;
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - starttime;
    sresult.time = duration.count();
    if (!sresult.pathfound) {
        return sresult;
    }
    
    lppath.push_back(vvgoal);
//    std::cout << "YEESSSSS  " << (*(vvgoal.parent)).i << " " << vvgoal.parent->j << " " << '\n' ;
//    std::cout << "YEESSSSS  " << (*(vvgoal.parent)).i << " " << vvgoal.parent->j << " " << '\n' ;
    while (vvgoal.parent != nullptr) {
        vvgoal = *(vvgoal.parent);
//        std::cout << "     " << vvgoal.i << " " << vvgoal.j << " " << vvgoal.pari << " " << vvgoal.parj << '\n';
        lppath.push_back(vvgoal);
    }
    lppath.reverse();
    double pathlength = 0;
    Node prv = lppath.front();
    for (auto it = lppath.begin(); it != lppath.end(); it++) {
        if (it == lppath.begin()) continue;
        pathlength += dist(prv, *it, options);
        prv = *it;
    }
    sresult.pathlength = pathlength;
    hppath.push_back(lppath.front());
    if (lppath.size() > 1) {
        auto it = lppath.begin();
        it++;
        hppath.push_back(*it);
        std::pair<int, int> dd = {it->i - hppath.front().i, it->j - hppath.front().j};
        it++;
        for (; it != lppath.end(); it++) {
            std::pair<int, int> dn = {it->i - hppath.back().i, it->j - hppath.back().j};
            if (dn == dd) {
                hppath.pop_back();
            }
            dd = dn;
            hppath.push_back(*it);
        }
    }
//    std::cout << "#####\n";
    for (auto it = hppath.begin(); it != hppath.end(); it++) {
//        std::cout << it->i << " " << it->j << '\n';
    }
    sresult.hppath = &hppath; //Here is a constant pointer
    sresult.lppath = &lppath;
    return sresult;
}

/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/
