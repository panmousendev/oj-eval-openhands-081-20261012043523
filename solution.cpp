#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
using namespace std;

const int MAXN = 3005;

class ExactMaxFlowSum {
private:
    vector<int> adj[MAXN];
    vector<pair<int, int>> edges;
    int n;
    vector<bool> visited;
    
    // Find connected components
    void find_component(int u, vector<int>& component) {
        visited[u] = true;
        component.push_back(u);
        
        for (int v : adj[u]) {
            if (!visited[v]) {
                find_component(v, component);
            }
        }
    }
    
    // Optimized max flow computation using early termination
    int compute_max_flow_optimized(int s, int t) {
        if (s == t) return 0;
        
        // Quick degree check
        int deg_s = adj[s].size();
        int deg_t = adj[t].size();
        int max_possible = min(deg_s, deg_t);
        if (max_possible == 0) return 0;
        if (max_possible > 3) max_possible = 3;
        
        // Special cases for optimization
        if (max_possible == 1) {
            // Just need to check if there's a path
            return has_path(s, t) ? 1 : 0;
        }
        
        // General case: find edge-disjoint paths
        vector<bool> edge_used(edges.size(), false);
        int paths = 0;
        
        for (int attempt = 0; attempt < max_possible; attempt++) {
            if (!find_path_with_edge_constraints(s, t, edge_used)) break;
            paths++;
        }
        
        return paths;
    }
    
    // Check if there's a path from s to t
    bool has_path(int s, int t) {
        vector<bool> visited_local(n + 1, false);
        queue<int> q;
        q.push(s);
        visited_local[s] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int v : adj[u]) {
                if (!visited_local[v]) {
                    if (v == t) return true;
                    visited_local[v] = true;
                    q.push(v);
                }
            }
        }
        
        return false;
    }
    
    // Find a path avoiding used edges
    bool find_path_with_edge_constraints(int s, int t, vector<bool>& edge_used) {
        vector<int> parent(n + 1, -1);
        vector<int> edge_idx_used(n + 1, -1);
        queue<int> q;
        q.push(s);
        parent[s] = s;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int v : adj[u]) {
                if (parent[v] == -1) {
                    // Find edge index
                    int edge_idx = get_edge_index(u, v);
                    
                    if (edge_idx != -1 && !edge_used[edge_idx]) {
                        parent[v] = u;
                        edge_idx_used[v] = edge_idx;
                        q.push(v);
                        if (v == t) {
                            // Mark edges in this path as used
                            int current = t;
                            while (current != s) {
                                int idx = edge_idx_used[current];
                                edge_used[idx] = true;
                                current = parent[current];
                            }
                            return true;
                        }
                    }
                }
            }
        }
        
        return false;
    }
    
    // Get edge index efficiently
    int get_edge_index(int u, int v) {
        for (size_t i = 0; i < edges.size(); i++) {
            if ((edges[i].first == u && edges[i].second == v) ||
                (edges[i].first == v && edges[i].second == u)) {
                return i;
            }
        }
        return -1;
    }
    
    // Compute exact sum for a component
    long long compute_component_sum_exact(const vector<int>& component) {
        long long total = 0;
        int comp_size = component.size();
        
        // Process all pairs exactly
        for (int i = 0; i < comp_size; i++) {
            for (int j = i + 1; j < comp_size; j++) {
                int u = component[i], v = component[j];
                total += compute_max_flow_optimized(u, v);
            }
        }
        
        return total;
    }
    
public:
    ExactMaxFlowSum(int nodes) : n(nodes) {
        visited.resize(n + 1, false);
    }
    
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.push_back({u, v});
    }
    
    long long solve() {
        long long total = 0;
        fill(visited.begin(), visited.end(), false);
        
        for (int i = 1; i <= n; i++) {
            if (!visited[i]) {
                vector<int> component;
                find_component(i, component);
                total += compute_component_sum_exact(component);
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
    
    ExactMaxFlowSum solver(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        solver.add_edge(a, b);
    }
    
    cout << solver.solve() << endl;
    return 0;
}