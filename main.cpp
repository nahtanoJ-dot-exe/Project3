#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <map>
#include <chrono>
#include "Graph.h"

using namespace std;

const string CO_FILE = "../USA-road-d.NY.co";
const string GR_FILE = "../USA-road-d.NY.gr";

const int WIDTH = 1400;
const int HEIGHT = 1400;
const int PAD = 50;

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "NY Roads - SPACE to find path");

    // load map
    cout << "Loading map..." << endl;
    DIMACSData data = Graph::loadDIMACS(CO_FILE, GR_FILE);

    if (data.nodes.empty()) {
        cout << "ERROR!" << endl;
        return 1;
    }

    sort(data.nodes.begin(), data.nodes.end(), [](const NodeCoord& a, const NodeCoord& b) {
        return a.id < b.id;
    });

    // focus on a smaller region // testing different area sizes
    double fullRangeX = data.maxX - data.minX;
    double fullRangeY = data.maxY - data.minY;

    // pick center region
    double regionMinX = data.minX + fullRangeX * 0.3;
    double regionMaxX = data.minX + fullRangeX * 0.7;
    double regionMinY = data.minY + fullRangeY * 0.3;
    double regionMaxY = data.minY + fullRangeY * 0.7;

    cout << "Filtering to center region..." << endl;

    // find nodes in this region
    vector<int> nodesInRegion;
    vector<sf::Vector2f> nodePos(data.nodes.size());

    for (auto& n : data.nodes) {
        if (n.rawX >= regionMinX && n.rawX <= regionMaxX &&
            n.rawY >= regionMinY && n.rawY <= regionMaxY) {
            nodesInRegion.push_back(n.id);
        }
        // calculate screen position for all nodes (needed for edges)
        float x = PAD + (float)((n.rawX - regionMinX) / (regionMaxX - regionMinX)) * (WIDTH - 2 * PAD);
        float y = HEIGHT - PAD - (float)((n.rawY - regionMinY) / (regionMaxY - regionMinY)) * (HEIGHT - 2 * PAD);
        nodePos[n.id] = sf::Vector2f(x, y);
    }

    cout << "Nodes in region: " << nodesInRegion.size() << endl;

    // pick src and dest from nodes in region
    int src_index = 0;
    int dest_index = nodesInRegion.size() - 1;
    int src = nodesInRegion[src_index];
    int dest = nodesInRegion[dest_index];

    // create edges only for nodes in view
    vector<sf::VertexArray> lines;
    map<pair<int, int>, int> edgeToLine;

    cout << "Creating visible edges..." << endl;
    for (auto& e : data.edges) {
        sf::Vector2f p1 = nodePos[e.src];
        sf::Vector2f p2 = nodePos[e.dest];

        // only add if both endpoints are on screen
        if (p1.x >= 0 && p1.x <= WIDTH && p1.y >= 0 && p1.y <= HEIGHT &&
            p2.x >= 0 && p2.x <= WIDTH && p2.y >= 0 && p2.y <= HEIGHT) {

            sf::VertexArray line(sf::PrimitiveType::LineStrip, 2);
            line[0].color = sf::Color(80, 80, 80);
            line[1].color = sf::Color(80, 80, 80);
            line[0].position = p1;
            line[1].position = p2;
            lines.push_back(line);
            edgeToLine[{e.src, e.dest}] = lines.size() - 1;
        }
    }
    cout << "Visible edges: " << lines.size() << endl;

    // build full graph (need all edges for pathfinding)
    Graph graph(data.edges, data.numNodes);

    // path variables
    vector<int> path;
    vector<sf::RectangleShape> pathLines;
    int pathIndex = 0;
    bool pathFound = false;
    bool animating = false;

    // algorithm selection: 1 = Dijkstra, 2 = A*
    int selectedAlgo = 1;

    cout << "\n===== CONTROLS =====" << endl;
    cout << "SPACE - Run pathfinding algorithm" << endl;
    cout << "1 - Select Dijkstra's Algorithm" << endl;
    cout << "2 - Select A* Algorithm" << endl;
    cout << "Arrow Keys - Move source node" << endl;
    cout << "A/D - Move destination node" << endl;
    cout << "R - Reset map" << endl;
    cout << "====================" << endl;
    cout << "\nCurrent Algorithm: Dijkstra" << endl;
    cout << "Source: " << src << " | Dest: " << dest << endl;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (event->is<sf::Event::KeyPressed>()) {
                auto key = event->getIf<sf::Event::KeyPressed>();
                // algorithm selection keys
                if (key && key->code == sf::Keyboard::Key::Num1) {
                    selectedAlgo = 1;
                    cout << "\nSelected: Dijkstra's Algorithm" << endl;
                }
                else if (key && key->code == sf::Keyboard::Key::Num2) {
                    selectedAlgo = 2;
                    cout << "\nSelected: A* Algorithm" << endl;
                }

                if (!pathFound) {
                    if (key && key->code == sf::Keyboard::Key::Space) {

                        // run the selected algorithm
                        if (selectedAlgo == 1) {
                            // run dijkstra
                            cout << "\n===== DIJKSTRA'S ALGORITHM =====" << endl;
                            auto start = chrono::high_resolution_clock::now();
                            path = graph.dijkstraPath(src, dest);
                            auto end = chrono::high_resolution_clock::now();
                            auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

                            cout << "Time: " << time.count() << " ms" << endl;
                            cout << "Path length: " << path.size() << " nodes" << endl;
                            cout << "=================================" << endl;
                        }
                        else if (selectedAlgo == 2) {
                            // run A*
                            cout << "\n===== A* ALGORITHM =====" << endl;
                            auto start = chrono::high_resolution_clock::now();
                            path = graph.aStarPath(src, dest, data.nodes);
                            auto end = chrono::high_resolution_clock::now();
                            auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

                            cout << "Time: " << time.count() << " ms" << endl;
                            cout << "Path length: " << path.size() << " nodes" << endl;
                            cout << "=========================" << endl;
                        }

                        if (!path.empty()) {
                            pathFound = true;
                            animating = true;
                            pathIndex = 0;
                        } else {
                            cout << "No path found!" << endl;
                        }
                    }
                    //moving src back and forth
                    else if (key && key->code == sf::Keyboard::Key::Left) {
                        if (src_index > 0) {
                            src_index--;
                            src = nodesInRegion[src_index];
                        }
                    }
                    else if (key && key->code == sf::Keyboard::Key::Right) {
                        if (src_index < nodesInRegion.size() - 1) {
                            src_index++;
                            src = nodesInRegion[src_index];
                        }
                    }
                    //moving dest back and forth
                    else if (key && key->code == sf::Keyboard::Key::A) {
                        if (dest_index > 0) {
                            dest_index--;
                            dest = nodesInRegion[dest_index];
                        }
                    }
                    else if (key && key->code == sf::Keyboard::Key::D) {
                        if (dest_index < nodesInRegion.size() - 1) {
                            dest_index++;
                            dest = nodesInRegion[dest_index];
                        }
                    }
                }
                //reset map
                if (key && key->code == sf::Keyboard::Key::R) {
                    pathFound = false;
                    src_index = 0;
                    dest_index = nodesInRegion.size() - 1;
                    src = nodesInRegion[src_index];
                    dest = nodesInRegion[dest_index];
                    pathLines.clear();
                }
            }
        }

        // animate to traverse path // slower: FIX
        if (animating && pathIndex < (int)path.size() - 1) {
            for (int i = 0; i < 30 && pathIndex < (int)path.size() - 1; i++) {
                int from = path[pathIndex];
                int to = path[pathIndex + 1];

                sf::Vector2f p1 = nodePos[from];
                sf::Vector2f p2 = nodePos[to];
                float dx = p2.x - p1.x;
                float dy = p2.y - p1.y;
                float length = sqrt(dx*dx + dy*dy);
                float angle = atan2(dy, dx) * 180.f / 3.14159f;

                sf::RectangleShape rect(sf::Vector2f(length, 4.f));
                rect.setPosition(p1);
                rect.setRotation(sf::degrees(angle));
                rect.setFillColor(sf::Color(0, 255, 0));
                pathLines.push_back(rect);
                pathIndex++;
            }
        }

        window.clear(sf::Color::Black);

        // draw edges
        for (auto& line : lines) {
            window.draw(line);
        }

        // draw path
        for (auto& rect : pathLines) {
            window.draw(rect);
        }

        // draw src = green and dest = red
        sf::CircleShape srcCircle(10.f);
        srcCircle.setFillColor(sf::Color::Green);
        srcCircle.setPosition(nodePos[src] - sf::Vector2f(10.f, 10.f));
        window.draw(srcCircle);

        sf::CircleShape destCircle(10.f);
        destCircle.setFillColor(sf::Color::Red);
        destCircle.setPosition(nodePos[dest] - sf::Vector2f(10.f, 10.f));
        window.draw(destCircle);

        window.display();
    }

    return 0;
}
