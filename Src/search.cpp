#include "search.h"

Search::Search()
{
//set defaults here
}

Search::~Search() {}

std::list<std::pair<int, int>> Search::returnSuccessors(const Node *v, const Map &Map, const EnvironmentOptions &options)
{
    std::list<std::pair<int,int>> succ;
    std::vector<int> vx = {1, 0, -1, 0};
    std::vector<int> vy = {0, 1, 0, -1};
    for (int i = 0; i < vx.size(); i++) {
        int ni, nj;
        ni = v->i + vx[i];
        nj = v->j + vy[i];
        if (Map.CellOnGrid(ni, nj) && Map.CellIsPresumablyTraversable(ni, nj)) {
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
        int ni = v->i + vdx[i], nj = v->j + vdy[i];
        if (!Map.CellOnGrid(ni, nj) || Map.CellIsObservedObstacle(ni, nj)) continue;
        int ai1 = v->i + vx[i], aj1 = v->j + vy[i];
        int ai2 = v->i + vx[i+1], aj2 = v->j + vy[i+1];
        bool f1 = Map.CellOnGrid(ai1, aj1) && Map.CellIsObservedObstacle(ai1, aj1);
        bool f2 = Map.CellOnGrid(ai2, aj2) && Map.CellIsObservedObstacle(ai2, aj2);
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


void Search::countHeuristicFunc(Node *v, const Map &map, const EnvironmentOptions &options)
{
    v->F = v->g + v->H;
    if (v->H != 0) {
        return;
    }
    std::pair<int, int> goal(map.getGoal());
    double di = abs(goal.first - v->i);
    double dj = abs(goal.second - v->j);
    switch(options.metrictype) {
        case CN_SP_MT_EUCL:
            v->H = sqrt(di*di + dj*dj);
            break;
        case CN_SP_MT_MANH:
            v->H = di + dj;
            break;
        case CN_SP_MT_DIAG:
            v->H = abs(di-dj) + CN_SQRT_TWO * std::min(di, dj);
            break;
        case CN_SP_MT_CHEB:
            v->H = std::max(di, dj);
            break;
        default:
            v->H = 0;
            break;
    }
    v->F = v->g + v->H;
    if (di == 0 && dj == 0) {
//        std::cout << "###### " << v->F << " " << v->H << " " << v->g << std::endl;
    }
}

double dist (const Node* v, const Node* u, const EnvironmentOptions &options) {
    int di = abs(v->i-u->i), dj = abs(v->j-u->j);
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

void Search::OPEN_insert(const Node* v, bool is_new)
{
    OPEN.insert({{v->F, v->H}, {v->i, v->j}});
    OPEN_H.insert({v->H, {v->i, v->j}});
    if (is_new)
        OPEN_nodes[{v->i, v->j}] = &gen[{v->i, v->j}];
}

void Search::OPEN_erase(const Node* v, bool perm)
{
    OPEN.erase({{v->F, v->H}, {v->i, v->j}});
    OPEN_H.erase({v->H, {v->i, v->j}});
    if (perm)
        OPEN_nodes.erase({v->i, v->j});
}

void Search::OPEN_clear() {
    OPEN.clear();
    OPEN_nodes.clear();
    OPEN_H.clear();
}

bool isReachable(int i, int j, int i0, int j0, const Map &map, const EnvironmentOptions &options) {
    if (!map.CellOnGrid(i, j) || map.CellIsObstacle(i, j)) return 0;
    int di = i - i0, dj = j - j0;
    if (di == 0 || dj == 0) return 1;
    if (!options.allowdiagonal) return 0;
    bool f1 = map.CellOnGrid(i, j0) & map.CellIsObstacle(i, j0);
    bool f2 = map.CellOnGrid(i0, j) & map.CellIsObstacle(i0, j);
    if (!f1 && !f2) return 1;
    if (f1 && f2) {
        return options.cutcorners & options.allowsqueeze;
    }
    return options.cutcorners;
}

void observe(Node *v, const Map &map, const EnvironmentOptions &options) {
    int area = options.visibility;
    int x = v->i, y = v->j;
    std::vector<int> vx, vy;
    for (int i = -area; i <= area; i++) {
        for (int j = -area; j <= area; j++) {
            vx.push_back(i);
            vy.push_back(j);
        }
    }
    for (int i = 0; i < vx.size(); i++) {
        int nx = x + vx[i], ny = y + vy[i];
        if (map.CellOnGrid(nx, ny) && map.CellIsObstacle(nx, ny)) map.setValue(nx, ny, 2);
    }
}

void Search::Astar(const Map &map, const EnvironmentOptions &options, Node* st)
{
    int lookahead = options.lookahead;
    sresult.pathfound = false;
    OPEN_clear();
    CLOSED.clear();
    std::unordered_set<Node *> used;
    st->g = 0;
    used.insert(st);
    countHeuristicFunc(st, map, options);
    h_v.push_back({{st->i, st->j}, st->H});
    OPEN_insert(st, 1);
    int expansions = 0;
    while (!OPEN.empty() && expansions++ < lookahead) {
        sresult.numberofsteps++;
        std::pair<int, int> sp = OPEN.begin()->second;
        Node *s = OPEN_nodes[sp];
//        std::cout << "i " << s->i << " j " << s->j << " F " << s->F << " g " << s->g << " H " << s->H << std::endl;
        if (s->i == goal->i && s->j == goal->j) {
            sresult.pathfound = true;
            break;
        }
        OPEN_erase(s, 1);
        CLOSED[sp] = s;
        std::list<std::pair<int, int>> succ = returnSuccessors(s, map, options);
        for (auto it = succ.begin(); it != succ.end(); it++) {
            std::pair<int, int> cur = *it;
            if (cur.first == 0 && cur.second == 4) {
//                std::cout << "@@@@ " << map.getValue(0, 4) << " " << map.CellIsPresumablyTraversable(0, 4) << '\n';
            }
            if (gen.find(cur) == gen.end()) {
                gen[cur] = Node(cur);
                Node *ns = &gen[cur];
                ns->g = s->g + dist(ns, s, options);
                ns->parent = s;
                countHeuristicFunc(ns, map, options);
                h_v.push_back({{ns->i, ns->j}, ns->H});
                OPEN_insert(ns, 1);
            } else {
                if (CLOSED.find(cur) != CLOSED.end()) continue;
                if (OPEN_nodes.find(cur) != OPEN_nodes.end()) {
                    Node *ns = OPEN_nodes[cur];
                    double Dist = dist(s, ns, options);
                    if (ns->g > s->g + Dist || used.find(ns) == used.end()) {
                        OPEN_erase(ns, 0);
                        ns->g = s->g+Dist;
                        ns->parent = s;
                        used.insert(ns);
                        countHeuristicFunc(ns, map, options);
                        OPEN_insert(ns, 0);
                    }
                } else {
                    Node *ns = &gen[cur];
                    ns->g = s->g + dist(ns, s, options);
                    ns->parent = s;
                    countHeuristicFunc(ns, map, options);
                    OPEN_insert(ns, 1);
                }
            }
        }
    }
//    std::cout << "------------\n";
}

void Search::updateHeuristic(const Map &map, const EnvironmentOptions &options) {
    std::vector<std::pair<int, int> > CLOSED_p;
    for (auto &p : CLOSED) {
        p.second->H = CN_INF;
        CLOSED_p.push_back({p.second->i, p.second->j});
    }
    while (!CLOSED.empty()) {
        Node* s = OPEN_nodes[OPEN_H.begin()->second];
        OPEN_erase(s, 1);
        std::pair<int, int> ps = {s->i, s->j};
        if (CLOSED.find(ps) != CLOSED.end()) {
            CLOSED.erase(ps);
        }
        std::list<std::pair<int, int>> succ = returnSuccessors(s, map, options);
        for (std::pair<int, int> cur : succ) {
            if (CLOSED.find(cur) != CLOSED.end()) {
                Node* ns = CLOSED[cur];
                double Dist = dist(s, ns, options);
                if (ns->H > s->H + Dist) {
                    if (OPEN_nodes.find({ns->i, ns->j}) == OPEN_nodes.end()) {
                        ns->H = s->H + Dist;
                        countHeuristicFunc(ns, map, options);
                        OPEN_insert(ns, 1);
                    } else {
                        OPEN_erase(ns, 0);
                        ns->H = s->H + Dist;
                        countHeuristicFunc(ns, map, options);
                        OPEN_insert(ns, 0);
                    }
                }
            }
        }
    }
    for (auto &p : CLOSED_p) {
        h_v.push_back({{p.first, p.second}, gen[p].H});
    }
}

std::string to_logv_file (const char* fname) {
    std::string str;
    str.append(fname);
    size_t found = str.find_last_of(".");
    if (found != std::string::npos) {
        str.erase(found);
        str.insert(found, "_vlog.txt");
    }
    else
        str.append("_vlog.txt");
    return str;
}

void vlog_map(const Map &map, const EnvironmentOptions& options, std::ofstream &vfile) {
    int n = map.getMapHeight(), m = map.getMapWidth();
    int vis = options.visibility;
    auto start = map.getStart();
    auto goal = map.getGoal();
    vfile << n << " " << m << std::endl << vis << std::endl << start.first << " " << start.second << std::endl << goal.first << " " << goal.second << std::endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int v = map.getValue(i, j);
            vfile << v << " ";
        }
        vfile << std::endl;
    }
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options, const char* fileName)
{
    std::string vfileName = to_logv_file(fileName);
    std::ofstream vfile;
    vfile.open(vfileName);
    vlog_map(map, options, vfile);
    //std::cout << "Visibility: " << options.visibility << "\nLookahead: " << options.lookahead << '\n';
    auto starttime = std::chrono::high_resolution_clock::now();
    sresult.pathfound = false;
    
    std::pair<int, int> nstart = map.getStart();
    std::pair<int, int> ngoal = map.getGoal();
    
    gen[nstart] = Node(nstart);
    gen[ngoal] = Node(ngoal);
    start = &gen[nstart];
    goal = &gen[ngoal];
    
    observe(start, map, options);
    lppath.push_back(*start);
    
    h_v.clear();
    h_v.push_back({{goal->i, goal->j}, goal->H});
    sresult.numberofsteps = 0;
    start->parent = nullptr;
    Node* st = start;
    
    while (st != goal) {
//        std::cout << "start " << st->i << " " << st->j << " " << map.getValue(0, 4) << '\n';
        sresult.pathfound = false;
        Astar(map, options, st);
        if (OPEN.empty()) break;
        
        vfile << OPEN_nodes.size() << std::endl;
        for (auto &p : OPEN_nodes) {
            vfile << p.first.first << " " << p.first.second << std::endl;
        }
        
        Node *tgoal;
        if (sresult.pathfound) tgoal = goal;
        else
            tgoal = OPEN_nodes[OPEN.begin()->second];
//        std::cout << "GOAL i " << tgoal->i << " j " << tgoal->j << " F " << tgoal->F << " g " << tgoal->g << " H " << tgoal->H << std::endl;
        if (tgoal == goal) sresult.pathfound = true;
        updateHeuristic(map, options);
        vfile << h_v.size() << std::endl;
        for (auto &p : h_v) {
            vfile << p.first.first << " " << p.first.second << " " << p.second << std::endl;
        }
        h_v.clear();
        std::vector<Node*> tpath;
        while (tgoal != st) {
            tpath.push_back(tgoal);
            tgoal = tgoal->parent;
        }
        std::reverse(tpath.begin(), tpath.end());
        vfile << tpath.size() << std::endl;
        countHeuristicFunc(st, map, options);
        for (Node *vcur : tpath) {
            vfile << vcur->i << " " << vcur->j << std::endl;
            countHeuristicFunc(vcur, map, options);
//            std::cout << vcur->i << " " << vcur->j << " " << vcur->F <<  '\n';
        }
        int len = 0;
        for (Node *vcur : tpath) {
//            std::cout << vcur->i << " " << vcur->j << "     " << vcur->F << " " << st->F << " " << '\n';
            if (!isReachable(vcur->i, vcur->j, st->i, st->j, map, options) || vcur->F > st->F) break;
            len++;
            st = vcur;
            lppath.push_back(*st);
            observe(st, map, options);
        }
        vfile << len << std::endl;
    }
    
//    std::cout << "#### " << (st == goal) << '\n';
    
    
    sresult.nodescreated =  gen.size();
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - starttime;
    sresult.time = duration.count();
    
    if (!sresult.pathfound) {
        return sresult;
    }
    
    double pathlength = 0;
    Node prv = lppath.front();
    for (auto it = lppath.begin(); it != lppath.end(); it++) {
        if (it == lppath.begin()) continue;
        pathlength += dist(&prv, &*it, options);
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
    sresult.hppath = &hppath;
    sresult.lppath = &lppath;
    return sresult;
}
