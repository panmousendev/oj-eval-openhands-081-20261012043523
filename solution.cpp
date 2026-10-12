#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
using namespace std;

const int MAXN = 3005;

class UltraFastMaxFlow {
private:
    vector<int> adj[MAXN];
    vector<pair<int, int>> edges;
    int n;
    vector<bool> visited;
    vector<vector<int>> edge_map; // Fast edge lookup
    
    // Precompute edge mapping for O(1) edge lookup
    void build_edge_map() {
        edge_map.assign(n + 1, vector<int>(n + 1, -1));
        for (size_t i = 0; i < edges.size(); i++) {
            int u = edges[i].first, v = edges[i].second;
            edge_map[u][v] = i;
            edge_map[v][u] = i;
        }
    }
    
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
    
    // Ultra-fast max flow with aggressive optimizations
    int compute_max_flow_ultra_fast(int s, int t) {
        if (s == t) return 0;
        
        int deg_s = adj[s].size();
        int deg_t = adj[t].size();
        int max_possible = min(deg_s, deg_t);
        if (max_possible == 0) return 0;
        if (max_possible > 3) max_possible = 3;
        
        // Special case: degree 1
        if (max_possible == 1) {
            return has_path_fast(s, t) ? 1 : 0;
        }
        
        // Use bitset for edge tracking (faster than vector<bool>)
        vector<unsigned int> edge_used((edges.size() + 31) / 32, 0);
        int paths = 0;
        
        for (int attempt = 0; attempt < max_possible; attempt++) {
            if (!find_path_bitset(s, t, edge_used)) break;
            paths++;
        }
        
        return paths;
    }
    
    // Fast path check
    bool has_path_fast(int s, int t) {
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
    
    // Path finding with bitset optimization
    bool find_path_bitset(int s, int t, vector<unsigned int>& edge_used) {
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
                    int edge_idx = edge_map[u][v];
                    
                    if (edge_idx != -1 && !(edge_used[edge_idx / 32] & (1u << (edge_idx % 32)))) {
                        parent[v] = u;
                        edge_idx_used[v] = edge_idx;
                        q.push(v);
                        if (v == t) {
                            // Mark edges in this path as used
                            int current = t;
                            while (current != s) {
                                int idx = edge_idx_used[current];
                                edge_used[idx / 32] |= (1u << (idx % 32));
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
    
    // Optimized component computation with early termination
    long long compute_component_optimized(const vector<int>& component) {
        long long total = 0;
        int comp_size = component.size();
        
        // Sort by degree for better cache performance
        vector<pair<int, int>> nodes_with_degree;
        for (int node : component) {
            nodes_with_degree.push_back({adj[node].size(), node});
        }
        sort(nodes_with_degree.begin(), nodes_with_degree.end());
        
        // Process pairs with degree-based ordering
        for (int i = 0; i < comp_size; i++) {
            int u = nodes_with_degree[i].second;
            for (int j = i + 1; j < comp_size; j++) {
                int v = nodes_with_degree[j].second;
                
                // Skip if both have degree 0 (isolated nodes)
                if (adj[u].empty() || adj[v].empty()) continue;
                
                total += compute_max_flow_ultra_fast(u, v);
            }
        }
        
        return total;
    }
    
public:
    UltraFastMaxFlow(int nodes) : n(nodes) {
        visited.resize(n + 1, false);
    }
    
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.push_back({u, v});
    }
    
    long long solve() {
        build_edge_map();
        
        long long total = 0;
        fill(visited.begin(), visited.end(), false);
        
        for (int i = 1; i <= n; i++) {
            if (!visited[i]) {
                vector<int> component;
                find_component(i, component);
                total += compute_component_optimized(component);
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
    
    UltraFastMaxFlow solver(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        solver.add_edge(a, b);
    }
    
    cout << solver.solve() << endl;
    return 0;
}