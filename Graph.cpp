#include "Graph.h"

#include <iostream>
#include <set>
using namespace std;

int Graph::dijkstra(int src, int dest, vector<sf::VertexArray>& lines) {
    if (adjList[src].empty() || adjList[dest].empty()) {
        return -1;
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; //pair - (dist, vertex)
    vector<int> dist(numVertices, INT_MAX);
    vector<int> p(numVertices, -1);

    pq.push(make_pair(0, src));
    dist[src] = 0;


    while (!pq.empty()) {
        pair<int, int> current = pq.top();
        pq.pop();
        int u = current.second; //vertex

        for (auto it = adjList[u].begin(); it != adjList[u].end(); it++) {
            int v = it->first; //neighbor
            int w = it->second; //weight
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push(make_pair(dist[v], v));
                p[v] = u;
            }
        }
    }
    int vertex = p[dest];
    while (vertex != -1) {
        lines[vertex][0].color = sf::Color::Blue;
        lines[vertex][1].color = sf::Color::Blue;
        lines[vertex][0].color = sf::Color::Blue;
        lines[vertex][1].color = sf::Color::Blue;
        cout << vertex << " ";
        vertex = p[vertex];
    }
    return dist[dest];
}