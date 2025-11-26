//
// Created by Jonat on 11/25/2025.
//
#include <functional>
#include <queue>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;

#ifndef PROJECT3_GRAPH_H
#define PROJECT3_GRAPH_H

struct Edge {
    int src, dest, weight;
    Edge(int _src, int _dest, float _weight) {src = _src; dest = _dest; weight = _weight;}
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
    int dijkstra(int src, int dest, vector<sf::VertexArray>&);
};


#endif //PROJECT3_GRAPH_H