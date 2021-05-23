
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <SFML/Graphics.hpp>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"

void drawRectangle(sf::RenderWindow &win, float height, float width, sf::Color color, float shH = 0, float shW = 0, float angle = 0) {
    sf::RectangleShape rectangle(sf::Vector2f(height, width));
    rectangle.move(shW, shH);
    rectangle.setFillColor(color);
    rectangle.rotate(angle);
    win.draw(rectangle);
}

void drawCircle(sf::RenderWindow &win, float radius, sf::Color color, float shH = 0, float shW = 0) {
    sf::CircleShape circle(radius);
    circle.move(shW, shH);
    circle.setFillColor(color);
    win.draw(circle);
}

void drawGrid(sf::RenderWindow& win, int rows, int cols){
    // initialize values
    int numLines = rows+cols-2;
    sf::VertexArray grid(sf::Lines, 2*(numLines));
    win.setView(win.getDefaultView());
    auto size = win.getView().getSize();
    float rowH = size.y/rows;
    float colW = size.x/cols;
    // row separators
    for(int i=0; i < rows-1; i++){
        int r = i+1;
        float rowY = rowH*r;
        grid[i*2].position = {0, rowY};
        grid[i*2].color = sf::Color::Black;
        grid[i*2+1].position = {size.x, rowY};
        grid[i*2+1].color = sf::Color::Black;
    }
    // column separators
    for(int i=rows-1; i < numLines; i++){
        int c = i-rows+2;
        float colX = colW*c;
        grid[i*2].position = {colX, 0};
        grid[i*2].color = sf::Color::Black;
        grid[i*2+1].position = {colX, size.y};
        grid[i*2+1].color = sf::Color::Black;
    }
    // draw it
    win.draw(grid);
}

int getVal(std::vector<std::vector<int>> &map, int i, int j) {
    int n = map.size(), m = map[0].size();
    if (i < 0 || i >= n || j < 0 || j >= m) return -3;
    return map[i][j];
}

void drawLine(sf::RenderWindow &window, float h1, float w1, float h2, float w2) {
    sf::Vertex line_without_thickness[] =
            {
                // Координата первой вершины
                sf::Vertex(sf::Vector2f(w1, h1)),
                // Координата второй вершины
                sf::Vertex(sf::Vector2f(w2, h2))
            };
    line_without_thickness->color = sf::Color::Black;
    window.draw(line_without_thickness, 2, sf::Lines);
}

void drawText(sf::RenderWindow &window, double x, float size, float shH, float shW, sf::Font font, sf::Color color = sf::Color::Black) {
    char buf[64];
    snprintf(buf, 64, "%.2lf", x);
    sf::Text text(buf, font, 50);
    text.setFillColor(color);
    text.setCharacterSize(size);
    text.move(shW, shH);
    window.draw(text);
}

std::map<std::pair<int, int>, float> angle = { {{1, 0}, 90}, {{0, 1}, 0}, {{-1, 0}, 270}, {{0, -1}, 180}, {{1, 1}, 45}, {{-1, 1}, 315}, {{-1, -1}, 225}, {{1, -1}, 135} };

struct hasher {
    size_t operator () (const std::pair<int, int>& p) const {
        return 1ll * INT32_MAX * p.first + p.second;
    }
};
std::unordered_map<std::pair<int, int>, double, hasher> nodes_heur;
sf::Font font;

void drawMap(sf::RenderWindow &window, std::vector<std::vector<int>> &map, int cur_H, int cur_W, int area, int visibility, float cell_H, float cell_W, std::vector<std::vector<std::pair<int, int>>> &path_line, std::set<std::pair<int, int>> &borders) {
    int lh = cur_H - area/2, rh = cur_H + area/2;
    int lw = cur_W - area/2, rw = cur_W + area/2;
    int lvh = cur_H - visibility/2, rvh = cur_H + visibility/2;
    int lvw = cur_W - visibility/2, rvw = cur_W + visibility/2;
    for (int i = lh; i <= rh; i++) {
        for (int j = lw; j <= rw; j++) {
            int x = getVal(map, i, j);
            float cwh = cell_H * (i-lh), cww = cell_W * (j - lw);
            if (x == -1 && i >= lvh && i <= rvh && j >= lvw && j <= rvw) {
                map[i][j] = -2;
                x = -2;
            }
            
            if (x != -3) {
                if (borders.find({i, j}) != borders.end()) {
                    drawRectangle(window, cell_H, cell_W, sf::Color(255,0,0,150), cwh, cww);
                }
                if (path_line[i][j] != std::make_pair(0, 0)) {
//                    std::cout << "#";
                    int dh = path_line[i][j].first, dw = path_line[i][j].second;
//                    float cwh2 = cell_H * (i + dh -lh), cww2 = cell_W * (j + dw - lw);
//                    drawLine(window, cwh + cell_H / 2, cww + cell_W / 2, cwh2 + cell_H / 2, cww2 + cell_W / 2);
                    drawRectangle(window, sqrt(dh*dh+dw*dw) * cell_H + 2, 4, sf::Color::Black, cwh + cell_H/2, cww + cell_W/2, angle[{dh, dw}]);
                }
                if (nodes_heur.find({i, j}) != nodes_heur.end()) {
                    double x = nodes_heur[{i, j}];
                    drawText(window, x, cell_H/5, cwh, cww + cell_W/10, font);
                }
            }
            if (x == -1) {
                drawRectangle(window, cell_H, cell_W, sf::Color(0, 0, 0, 50), cwh, cww);
            }
            if (x == -2 || x == -3) {
                drawRectangle(window, cell_H, cell_W, sf::Color::Black, cwh, cww);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    
    std::ifstream file;
    file.open(argv[1]);
    int map_H, map_W, visibility, start_H, start_W, goal_H, goal_W;
    file >> map_H >> map_W >> visibility >> start_H >> start_W >> goal_H >> goal_W;
    std::vector<std::vector<int>> map(map_H, std::vector<int>(map_W));
    std::vector<std::vector<std::pair<int, int>>> path_line(map_H, std::vector<std::pair<int, int>>(map_W));
    std::set<std::pair<int, int> > borders;
    for (int i = 0; i < map_H; i++) {
        for (int j = 0; j < map_W; j++) {
            file >> map[i][j];
            if (map[i][j] == 1) map[i][j] = -1;
        }
    }
    // Create the main window
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    int window_H = 1800, window_W = 1800;
    visibility = visibility * 2 + 1;
    int area = visibility + 4;
    float v_rate = 1.f * visibility / area;
    float cellH = 1.f * window_H / area, cellW = 1.f * window_W / area;
    sf::RenderWindow window(sf::VideoMode(window_H, window_W), "LSS_LRTA*");

    // Set the Icon
    /*sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());*/

    // Load a sprite to display
    /*sf::Texture texture;
    if (!texture.loadFromFile(resourcePath() + "cute_image.jpg")) {
        return EXIT_FAILURE;
    }
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    
    sf::Text text("LSS-LRTA*", font, 50);
    text.setFillColor(sf::Color::Black);*/

    // Start the game loop
    
    int path_len = 0, cur_pos = 0, pathfound_len = 0;
    bool eof = 0;
    int cur_H = start_H, cur_W = start_W;
    std::vector<std::pair<int, int>> path;
    
    sf::RectangleShape test(sf::Vector2f(100, 5));
    test.setFillColor(sf::Color::Black);
    test.move(400, 400);
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            if (!eof && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
//                test.rotate(45.f);
                if (path_len == cur_pos) {
                    for (int i = cur_pos; i < pathfound_len; i++) {
                        path_line[path[i].first][path[i].second] = {0, 0};
                    }
                    int num_borders;
                    if (!(file >> num_borders)) {
                        eof = 1;
                    } else {
                        borders.clear();
                        for (int i = 0; i < num_borders; i++) {
                            int b_i, b_j;
                            file >> b_i >> b_j;
                            borders.insert({b_i, b_j});
                        }
                    }
                    int heur_len;
                    if (!(file >> heur_len)) {
                        eof = 1;
                    } else {
                        for (int i = 0; i < heur_len; i++) {
                            int n_i, n_j;
                            double n_H;
                            file >> n_i >> n_j >> n_H;
                            nodes_heur[{n_i, n_j}] = n_H;
                        }
                    }
                    if (!(file >> pathfound_len)) {
                        eof = 1;
                    } else {
                        path.clear();
                        for (int i = 0; i < pathfound_len; i++) {
                            int x, y;
                            file >> x >> y;
                            path.push_back({x, y});
                        }
                        file >> path_len;
                        cur_pos = 0;
                        int d_W, d_H, c_H = cur_H, c_W = cur_W;
                        for (int i = 0; i < pathfound_len; i++) {
                            d_H = path[i].first - c_H;
                            d_W = path[i].second - c_W;
//                            std::cout << path[i].first << " " << path[i].second << " " << c_H << " " << c_W << " " << -d_H << " " << -d_W << std::endl;
                            path_line[path[i].first][path[i].second] = {-d_H, -d_W};
                            c_H = path[i].first;
                            c_W = path[i].second;
                        }
                    }
                } else {
                    cur_H = path[cur_pos].first;
                    cur_W = path[cur_pos].second;
                    path_line[cur_H][cur_W] = {0, 0};
                    cur_pos++;
                }
            }
        }

        // Clear screen
        window.clear(sf::Color::White);

        // Draw the sprite
//        window.draw(sprite);

        // Draw the string
//        window.draw(text);
        
        double v_H = window_H * v_rate, v_W = window_W * v_rate;
        
        drawRectangle(window, v_H, v_W, sf::Color::Green, (window_H - v_H)/2, (window_W - v_W)/2);
        
        drawMap(window, map, cur_H, cur_W, area, visibility, cellH, cellW, path_line, borders);
        
        drawGrid(window, area, area);
        
        int lh = cur_H - area/2, rh = cur_H + area/2;
        int lw = cur_W - area/2, rw = cur_W + area/2;
        if (goal_H >= lh && goal_H <= rh && goal_W >= lw && goal_W <= rw) {
            float cwh = cellH * (goal_H-lh), cww = cellW * (goal_W - lw);
            drawCircle(window, std::min(cellH, cellW) / 4, sf::Color::Blue, cwh + 0.25 * cellH, cww + 0.25 * cellW);
        }
        
        drawCircle(window, std::min(cellH, cellW) / 4, sf::Color::Black, (window_H - 0.5 * cellH) / 2, (window_W - 0.5 * cellW) / 2);
        
        //drawText(window, 10.25, 30, 30, 20, font);
        
//        drawRectangle(window, 100, 4, sf::Color::Black, 400, 400, angle[{1, -1}]);
        
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
