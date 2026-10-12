#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
using namespace std;

const int MAXN = 3005;

class MaxFlowSolver {
private:
    vector<int> adj[MAXN];
    vector<pair<int, int>> edges;
    int n;
    
    // BFS to find if there's a path from s to t
    bool bfs(int s, int t, vector<int>& parent) {
        vector<bool> visited(n + 1, false);
        queue<int> q;
        q.push(s);
        visited[s] = true;
        parent[s] = -1;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int v : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    parent[v] = u;
                    q.push(v);
                    if (v == t) return true;
                }
            }
        }
        return false;
    }
    
    // Find one augmenting path and remove its edges
    int find_and_remove_path(int s, int t) {
        vector<int> parent(n + 1);
        if (!bfs(s, t, parent)) return 0;
        
        // Reconstruct path and remove edges
        vector<int> path;
        int current = t;
        while (current != -1) {
            path.push_back(current);
            current = parent[current];
        }
        reverse(path.begin(), path.end());
        
        // Remove edges from the path
        for (int i = 0; i < path.size() - 1; i++) {
            int u = path[i], v = path[i + 1];
            // Remove edge u-v from adjacency lists
            auto it = find(adj[u].begin(), adj[u].end(), v);
            if (it != adj[u].end()) adj[u].erase(it);
            it = find(adj[v].begin(), adj[v].end(), u);
            if (it != adj[v].end()) adj[v].erase(it);
        }
        
        return 1;
    }
    
public:
    MaxFlowSolver(int nodes) : n(nodes) {}
    
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.push_back({u, v});
    }
    
    int max_flow(int s, int t) {
        // Restore original graph
        for (int i = 1; i <= n; i++) {
            adj[i].clear();
        }
        for (auto& edge : edges) {
            adj[edge.first].push_back(edge.second);
            adj[edge.second].push_back(edge.first);
        }
        
        int flow = 0;
        while (true) {
            int path_flow = find_and_remove_path(s, t);
            if (path_flow == 0) break;
            flow += path_flow;
        }
        return flow;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    MaxFlowSolver solver(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        solver.add_edge(a, b);
    }
    
    long long total = 0;
    
    // For each pair (a, b) where a < b, calculate max flow
    for (int a = 1; a <= n; a++) {
        for (int b = a + 1; b <= n; b++) {
            int flow = solver.max_flow(a, b);
            total += flow;
        }
    }
    
    cout << total << endl;
    return 0;
}