#include "Graph.h"
#include <iostream>
#include <set>
using namespace std;

int Graph::dijkstra(int src, int dest, vector<sf::VertexArray>& lines, map<pair<int, int>, int>& lineMapper) {
    if (adjList[src].empty() || adjList[dest].empty()) {
        cout << "No path found" << endl;
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
    if (dist[dest] == INT_MAX) {
        cout << "No path found" << endl;
        return -1;
    }
    //changes color of the path
    int vertex = dest;
    while (p[vertex] != -1) {
        lines[lineMapper[{p[vertex], vertex}]][0].color = sf::Color::Green;
        lines[lineMapper[{p[vertex], vertex}]][1].color = sf::Color::Green;
        lines[lineMapper[{p[vertex], vertex}]][0].color = sf::Color::Green;
        lines[lineMapper[{p[vertex], vertex}]][1].color = sf::Color::Green;
        cout << "Lines:" << vertex << " to " << p[vertex] << endl;
        vertex = p[vertex];
    }
    return dist[dest];
}

int Graph::two_way_dijkstra(int src, int dest, vector<sf::VertexArray> &lines, map<pair<int, int>, int> &lineMapper) {
    if (adjList[src].empty() || adjList[dest].empty()) {
        cout << "No path found" << endl;
        return -1;
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq_src; //pair - (dist, vertex)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq_dest;
    vector<int> dist_src(numVertices, INT_MAX);
    vector<int> dist_dest(numVertices, INT_MAX);
    vector<int> p_src(numVertices, -1);
    vector<int> p_dest(numVertices, -1);

    pq_src.push(make_pair(0, src));
    pq_dest.push(make_pair(0, dest));
    dist_src[src] = 0;
    dist_dest[dest] = 0;
    set<int> visited_forward;
    set<int> visited_backward;
    int mid_point;
    int min_dist = INT_MAX;

    while (!pq_src.empty() && !pq_dest.empty()) {
        pair<int, int> current_src = pq_src.top();
        pair<int, int> current_dest = pq_dest.top();
        pq_src.pop();
        pq_dest.pop();
        int u_src = current_src.second; //vertex
        int u_dest = current_dest.second; //vertex

        visited_forward.insert(u_src);
        visited_backward.insert(u_dest);

        for (auto it_src = adjList[u_src].begin(); it_src != adjList[u_src].end(); ++it_src) {

            int v_src = it_src->first; //neighbor
            int w_src = it_src->second; //weight
            if (dist_src[v_src] > dist_src[u_src] + w_src) {
                dist_src[v_src] = dist_src[u_src] + w_src;
                pq_src.push(make_pair(dist_src[v_src], v_src));
                p_src[v_src] = u_src;
                if (visited_backward.count(v_src) && dist_src[u_src] + w_src + dist_dest[v_src] < min_dist) {
                    mid_point = v_src;
                    min_dist = dist_src[u_src] + dist_dest[v_src] + w_src;
                }
            }
        }

        for (auto it_dest = adjList[u_dest].begin(); it_dest != adjList[u_dest].end(); ++it_dest) {
            int v_dest = it_dest->first; //neighbor
            int w_dest = it_dest->second; //weight
            if (dist_dest[v_dest] > dist_dest[u_dest] + w_dest) {
                dist_dest[v_dest] = dist_dest[u_dest] + w_dest;
                pq_dest.push(make_pair(dist_dest[v_dest], v_dest));
                p_dest[v_dest] = u_dest;
                if (visited_forward.count(v_dest) && dist_dest[u_dest] + w_dest + dist_src[v_dest] < min_dist) {
                    mid_point = v_dest;
                    min_dist = dist_dest[u_dest] + dist_src[v_dest] + w_dest;
                }
            }
        }
        if (dist_src[u_src] + dist_dest[u_dest] >= min_dist) {
            break;
        }
    }

    //changes color of the path
    int vertex_src = mid_point;
    int vertex_dest = mid_point;
    while (p_src[vertex_src] != -1) {
        lines[lineMapper[{p_src[vertex_src], vertex_src}]][0].color = sf::Color::Blue;
        lines[lineMapper[{p_src[vertex_src], vertex_src}]][1].color = sf::Color::Blue;
        lines[lineMapper[{p_src[vertex_src], vertex_src}]][0].color = sf::Color::Blue;
        lines[lineMapper[{p_src[vertex_src], vertex_src}]][1].color = sf::Color::Blue;
        cout << "Lines from source: " << vertex_src << " to " << p_src[vertex_src] << endl;
        vertex_src = p_src[vertex_src];
    }
    while (p_dest[vertex_dest] != -1) {
        lines[lineMapper[{p_dest[vertex_dest], vertex_dest}]][0].color = sf::Color::White;
        lines[lineMapper[{p_dest[vertex_dest], vertex_dest}]][1].color = sf::Color::White;
        lines[lineMapper[{p_dest[vertex_dest], vertex_dest}]][0].color = sf::Color::White;
        lines[lineMapper[{p_dest[vertex_dest], vertex_dest}]][1].color = sf::Color::White;
        cout << "Lines from destination:" << vertex_dest << " to " << p_dest[vertex_dest] << endl;
        vertex_dest = p_dest[vertex_dest];
    }
    cout << dist_src[mid_point] << " / " << dist_dest[mid_point] << endl;
    return min_dist;
}
