#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
#include <map>
using namespace std;

const int MAXN = 3005;

class OptimizedMaxFlow {
private:
    vector<int> adj[MAXN];
    vector<pair<int, int>> edges;
    int n;
    map<pair<int, int>, int> edge_to_idx; // Map edge pairs to indices
    
    // Precompute all-pairs shortest paths for connectivity
    vector<vector<int>> precompute_distances() {
        vector<vector<int>> dist(n + 1, vector<int>(n + 1, -1));
        
        for (int i = 1; i <= n; i++) {
            queue<int> q;
            q.push(i);
            dist[i][i] = 0;
            
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                
                for (int v : adj[u]) {
                    if (dist[i][v] == -1) {
                        dist[i][v] = dist[i][u] + 1;
                        q.push(v);
                    }
                }
            }
        }
        return dist;
    }
    
    // Count edge-disjoint paths
    int count_disjoint_paths(int s, int t, const vector<vector<int>>& dist) {
        if (s == t || dist[s][t] == -1) return 0;
        
        // Max flow is bounded by min(degree[s], degree[t])
        int max_possible = min(adj[s].size(), adj[t].size());
        if (max_possible == 0) return 0;
        if (max_possible > 3) max_possible = 3; // Due to degree constraint
        
        vector<bool> edge_used(edges.size(), false);
        int paths = 0;
        
        for (int attempt = 0; attempt < max_possible; attempt++) {
            vector<int> parent(n + 1, -1);
            vector<int> edge_idx_used(n + 1, -1);
            queue<int> q;
            q.push(s);
            parent[s] = s;
            
            bool found = false;
            while (!q.empty() && !found) {
                int u = q.front();
                q.pop();
                
                for (int v : adj[u]) {
                    if (parent[v] == -1) {
                        // Find edge index using map
                        int edge_idx = -1;
                        auto it = edge_to_idx.find({min(u, v), max(u, v)});
                        if (it != edge_to_idx.end()) {
                            edge_idx = it->second;
                        }
                        
                        if (edge_idx != -1 && !edge_used[edge_idx]) {
                            parent[v] = u;
                            edge_idx_used[v] = edge_idx;
                            q.push(v);
                            if (v == t) {
                                found = true;
                                break;
                            }
                        }
                    }
                }
            }
            
            if (!found) break;
            
            // Mark edges in this path as used
            int current = t;
            while (current != s) {
                int edge_idx = edge_idx_used[current];
                edge_used[edge_idx] = true;
                current = parent[current];
            }
            
            paths++;
        }
        
        return paths;
    }
    
public:
    OptimizedMaxFlow(int nodes) : n(nodes) {}
    
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        int idx = edges.size();
        edges.push_back({u, v});
        edge_to_idx[{min(u, v), max(u, v)}] = idx;
    }
    
    long long solve() {
        // Precompute distances once
        auto dist = precompute_distances();
        
        long long total = 0;
        
        // For each pair (a, b) where a < b, calculate max flow
        for (int a = 1; a <= n; a++) {
            for (int b = a + 1; b <= n; b++) {
                int flow = count_disjoint_paths(a, b, dist);
                total += flow;
            }
        }
        
        return total;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    OptimizedMaxFlow solver(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        solver.add_edge(a, b);
    }
    
    cout << solver.solve() << endl;
    return 0;
}