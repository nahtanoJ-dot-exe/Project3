//
// Created by Jonat on 11/25/2025.
//
#include <functional>
#include <queue>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>
using namespace std;

#ifndef PROJECT3_GRAPH_H
#define PROJECT3_GRAPH_H

struct Edge {
    int src, dest, weight;
    Edge(int _src, int _dest, float _weight) {src = _src; dest = _dest; weight = _weight;}
};

// Struct to hold coordinate data from .co file
struct NodeCoord {
    int id;
    double x, y;  // Normalized coordinates for display
    double rawX, rawY;  // Original coordinates from file
};

// DIMACS file loader results
struct DIMACSData {
    vector<NodeCoord> nodes;
    vector<Edge> edges;
    int numNodes;
    int numEdges;
    double minX, maxX, minY, maxY;  // Bounding box of original coordinates
};

class Graph {
public:
    int numVertices;
    vector<vector<pair<float, float>>> adjList;

    Graph(const vector<Edge>& edges, int vertices) {
        numVertices = vertices;

        adjList.resize(vertices);

        for (auto &edge : edges) {
            adjList[edge.src].push_back(make_pair(edge.dest, edge.weight));
        }
    }
    int dijkstra(int src, int dest, vector<sf::VertexArray>& lines, map<pair<int, int>, int>& lineMapper);
    int two_way_dijkstra(int src, int dest, vector<sf::VertexArray>& lines,map<pair<int, int>, int>& lineMapper);

    // versions that return the path
    vector<int> dijkstraPath(int src, int dest);
    vector<int> twoWayDijkstraPath(int src, int dest);

    // A* algorithm, needs coordinates for the heuristic
    vector<int> aStarPath(int src, int dest, const vector<NodeCoord>& coords);

    // Static methods to load DIMACS files
    static DIMACSData loadDIMACS(const string& coFile, const string& grFile);
    static vector<NodeCoord> loadCoordinates(const string& filename);
    static vector<Edge> loadEdges(const string& filename, int& numNodes, int& numEdges);
};


#endif //PROJECT3_GRAPH_H
