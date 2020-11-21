#include "search.h"

Search::Search()
{
//set defaults here
}

Search::~Search() {}

std::list<Node> Search::returnSuccessors(const Node &v, const Map &Map, const EnvironmentOptions &options)
{
    std::list<Node> succ;
    std::vector<int> vx = {1, -1, 0, 0};
    std::vector<int> vy = {0, 0, 1, -1};
    for (int i = 0; i < 4; i++) {
        int ni, nj;
        ni = v.i + vx[i];
        nj = v.j + vy[i];
        Node adjv(ni, nj);
        succ.push_back(adjv);
    }
    return succ;
}

void Search::countHeuristicFunc(Node &v, const Map &map, const EnvironmentOptions &options)
{
    std::pair<int, int> goal = map.getGoal();
    int di = goal.first - v.i;
    int dj = goal.second - v.j;
    v.H = di + dj;
    v.F = v.g + v.H;
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    //need to implement
    auto starttime = std::chrono::steady_clock::now();
    sresult.pathfound = false;
    
    std::pair<int, int> nstart = map.getStart();
    std::pair<int, int> ngoal = map.getGoal();
    
    Node start(nstart.first, nstart.second);
    countHeuristicFunc(start, map, options);
    OPEN.clear();
    OPEN.push_back(start);
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
        Node s = OPEN.front();
        std::list<Node>::iterator sit = OPEN.begin();
        for (auto it = OPEN.begin(); it != OPEN.end(); it++) {
            if (it->F < s.F) {
                s = *it;
                sit = it;
            }
        }
        //std::cout << "@\n";
        OPEN.erase(sit);
        CLOSED.push_back(s);
//        std::cout << s.i << " " << s.j << '\n';
        if (s.i == ngoal.first && s.j == ngoal.second) {
            sresult.pathfound = true;
            vvgoal = s;
//            std::cout << "YEESSSSS  " << vvgoal.parent->i << " " << vvgoal.parent->j << " " << (vvgoal.parent == nullptr) << '\n' ;
            break;
        }
        std::list<Node> succ = returnSuccessors(s, map, options);
        for (auto it = succ.begin(); it != succ.end(); it++) {
            std::pair<int, int> cur = {it->i, it->j};
            if (map.getValue(cur.first, cur.second) != 0)
                continue;
            if (gen.find(cur) == gen.end()) {
                Node ns(cur);
                ns.g = s.g + 1;
                ns.parent = &CLOSED.back();
                countHeuristicFunc(ns, map, options);
                OPEN.push_back(ns);
                gen.insert(cur);
            } else {
                std::list<Node>::iterator nsit = CLOSED.end();
                for (auto it = CLOSED.begin(); it != CLOSED.end(); it++) {
                    if (it->i == cur.first && it->j == cur.second) {
                        nsit = it;
                    }
                }
                if (nsit != CLOSED.end()) continue;
                for (auto it = OPEN.begin(); it != OPEN.end(); it++) {
                    if (it->i == cur.first && it->j == cur.second) {
                        nsit = it;
                    }
                }
                if (nsit->g > s.g + 1) {
                    nsit->g = s.g+1;
                    nsit->parent = &CLOSED.back();
                    countHeuristicFunc(*nsit, map, options);
                }
            }
        }
    }
    auto endtime = std::chrono::steady_clock::now();
    
    sresult.nodescreated =  OPEN.size() + CLOSED.size();
    sresult.numberofsteps = nsteps;
    sresult.time = std::chrono::duration<double>(duration_cast<std::chrono::milliseconds>(endtime - starttime)).count();
    if (!sresult.pathfound) {
        return sresult;
    }
    
    hppath.push_back(vvgoal);
    int cnt = 0;
//    std::cout << "YEESSSSS  " << (*(vvgoal.parent)).i << " " << vvgoal.parent->j << " " << '\n' ;
//    std::cout << "YEESSSSS  " << (*(vvgoal.parent)).i << " " << vvgoal.parent->j << " " << '\n' ;
    while (vvgoal.parent != nullptr && cnt++ < 1000) {
        vvgoal = *(vvgoal.parent);
//        std::cout << "     " << vvgoal.i << " " << vvgoal.j << " " << vvgoal.pari << " " << vvgoal.parj << '\n';
        hppath.push_back(vvgoal);
    }
    hppath.reverse();
    sresult.pathlength = hppath.size();
    lppath.push_back(hppath.front());
    if (hppath.size() > 1) {
        auto it = hppath.begin();
        it++;
        lppath.push_back(*it);
        std::pair<int, int> dd = {it->i - lppath.front().i, it->j - lppath.front().j};
        it++;
        for (; it != hppath.end(); it++) {
            std::pair<int, int> dn = {it->i - lppath.back().i, it->j - lppath.back().j};
            if (dn == dd) {
                lppath.pop_back();
            }
            dd = dn;
            lppath.push_back(*it);
        }
    }
//    std::cout << "#####\n";
    for (auto it = lppath.begin(); it != lppath.end(); it++) {
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
