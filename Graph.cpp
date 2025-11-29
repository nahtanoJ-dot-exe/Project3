#include "Graph.h"
#include <iostream>
#include <set>
#include <limits>
#include <algorithm>
using namespace std;

// Load coordinates from .co file
vector<NodeCoord> Graph::loadCoordinates(const string& filename) {
    vector<NodeCoord> nodes;
    ifstream file(filename);

    if (!file.is_open()) {
        // error handling
        cerr << "Error: Could not open coordinate file: " << filename << endl;
        return nodes;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == 'c' || line[0] == 'p') {
            continue;
        }

        if (line[0] == 'v') {
            NodeCoord node;
            char type;
            istringstream iss(line);
            iss >> type >> node.id >> node.rawX >> node.rawY;
            node.id--;  // Convert to 0-indexed
            nodes.push_back(node);
        }
    }

    file.close();
    cout << "Loaded " << nodes.size() << " coordinates from " << filename << endl;
    return nodes;
}

// Load edges from .gr file
vector<Edge> Graph::loadEdges(const string& filename, int& numNodes, int& numEdges) {
    vector<Edge> edges;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open graph file: " << filename << endl;
        return edges;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == 'c') {
            continue;  // Skip comments
        }

        if (line[0] == 'p') {
            // Problem line: p sp <nodes> <edges>
            char type;
            string sp;
            istringstream iss(line);
            iss >> type >> sp >> numNodes >> numEdges;
            continue;
        }

        if (line[0] == 'a') {
            // Arc line: a <src> <dest> <weight>
            char type;
            int src, dest, weight;
            istringstream iss(line);
            iss >> type >> src >> dest >> weight;
            // Convert to 0-indexed
            edges.emplace_back(src - 1, dest - 1, weight);
        }
    }

    file.close();
    cout << "Loaded " << edges.size() << " edges from " << filename << endl;
    return edges;
}

// Load both files and combine into DIMACSData
DIMACSData Graph::loadDIMACS(const string& coFile, const string& grFile) {
    DIMACSData data;

    // Load coordinates
    data.nodes = loadCoordinates(coFile);

    // Load edges
    data.edges = loadEdges(grFile, data.numNodes, data.numEdges);

    // Calculate bounding box
    if (!data.nodes.empty()) {
        data.minX = data.maxX = data.nodes[0].rawX;
        data.minY = data.maxY = data.nodes[0].rawY;

        for (const auto& node : data.nodes) {
            data.minX = min(data.minX, node.rawX);
            data.maxX = max(data.maxX, node.rawX);
            data.minY = min(data.minY, node.rawY);
            data.maxY = max(data.maxY, node.rawY);
        }

        cout << "Bounding box: X[" << data.minX << ", " << data.maxX << "] "
             << "Y[" << data.minY << ", " << data.maxY << "]" << endl;
    }

    return data;
}

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
    // color the path bright green
    int vertex = dest;
    while (p[vertex] != -1) {
        auto it = lineMapper.find({p[vertex], vertex});
        if (it != lineMapper.end() && it->second < (int)lines.size()) {
            lines[it->second][0].color = sf::Color(0, 255, 0);  // bright green
            lines[it->second][1].color = sf::Color(0, 255, 0);
        }
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

    // color the path
    int vertex_src = mid_point;
    int vertex_dest = mid_point;
    while (p_src[vertex_src] != -1) {
        auto it = lineMapper.find({p_src[vertex_src], vertex_src});
        if (it != lineMapper.end() && it->second < (int)lines.size()) {
            lines[it->second][0].color = sf::Color(0, 255, 255);  // cyan
            lines[it->second][1].color = sf::Color(0, 255, 255);
        }
        vertex_src = p_src[vertex_src];
    }
    while (p_dest[vertex_dest] != -1) {
        auto it = lineMapper.find({p_dest[vertex_dest], vertex_dest});
        if (it != lineMapper.end() && it->second < (int)lines.size()) {
            lines[it->second][0].color = sf::Color(255, 255, 0);  // yellow
            lines[it->second][1].color = sf::Color(255, 255, 0);
        }
        vertex_dest = p_dest[vertex_dest];
    }
    return min_dist;
}

// returns the path as a vector of node ids
vector<int> Graph::dijkstraPath(int src, int dest) {
    vector<int> path;
    if (adjList[src].empty() || adjList[dest].empty()) {
        return path;
    }

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(numVertices, INT_MAX);
    vector<int> parent(numVertices, -1);

    pq.push({0, src});
    dist[src] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        for (auto& neighbor : adjList[u]) {
            int v = neighbor.first;
            int w = neighbor.second;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
                parent[v] = u;
            }
        }
    }

    if (dist[dest] == INT_MAX) {
        return path;
    }

    // build path from dest to src
    int node = dest;
    while (node != -1) {
        path.push_back(node);
        node = parent[node];
    }

    // reverse to get src to dest
    reverse(path.begin(), path.end());
    return path;
}

vector<int> Graph::twoWayDijkstraPath(int src, int dest) {
    vector<int> path;
    if (adjList[src].empty() || adjList[dest].empty()) {
        return path;
    }

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq_src;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq_dest;
    vector<int> dist_src(numVertices, INT_MAX);
    vector<int> dist_dest(numVertices, INT_MAX);
    vector<int> p_src(numVertices, -1);
    vector<int> p_dest(numVertices, -1);

    pq_src.push({0, src});
    pq_dest.push({0, dest});
    dist_src[src] = 0;
    dist_dest[dest] = 0;

    set<int> visited_fwd;
    set<int> visited_bwd;
    int mid = -1;
    int minDist = INT_MAX;

    while (!pq_src.empty() && !pq_dest.empty()) {
        int u_src = pq_src.top().second;
        int u_dest = pq_dest.top().second;
        pq_src.pop();
        pq_dest.pop();

        visited_fwd.insert(u_src);
        visited_bwd.insert(u_dest);

        for (auto& nb : adjList[u_src]) {
            int v = nb.first;
            int w = nb.second;
            if (dist_src[v] > dist_src[u_src] + w) {
                dist_src[v] = dist_src[u_src] + w;
                pq_src.push({dist_src[v], v});
                p_src[v] = u_src;
                if (visited_bwd.count(v) && dist_src[u_src] + w + dist_dest[v] < minDist) {
                    mid = v;
                    minDist = dist_src[u_src] + w + dist_dest[v];
                }
            }
        }

        for (auto& nb : adjList[u_dest]) {
            int v = nb.first;
            int w = nb.second;
            if (dist_dest[v] > dist_dest[u_dest] + w) {
                dist_dest[v] = dist_dest[u_dest] + w;
                pq_dest.push({dist_dest[v], v});
                p_dest[v] = u_dest;
                if (visited_fwd.count(v) && dist_dest[u_dest] + w + dist_src[v] < minDist) {
                    mid = v;
                    minDist = dist_dest[u_dest] + w + dist_src[v];
                }
            }
        }

        if (dist_src[u_src] + dist_dest[u_dest] >= minDist) {
            break;
        }
    }

    if (mid == -1) return path;

    // build path from src to mid
    vector<int> pathToMid;
    int node = mid;
    while (node != -1) {
        pathToMid.push_back(node);
        node = p_src[node];
    }
    reverse(pathToMid.begin(), pathToMid.end());

    // build path from mid to dest
    node = p_dest[mid];
    while (node != -1) {
        pathToMid.push_back(node);
        node = p_dest[node];
    }

    return pathToMid;
}
