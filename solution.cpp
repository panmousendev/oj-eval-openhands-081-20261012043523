#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
using namespace std;

const int MAXN = 3005;

class FastMaxFlow {
private:
    vector<int> adj[MAXN];
    vector<pair<int, int>> edges;
    int n;
    
    // Use Union-Find for connectivity checks
    class UnionFind {
    private:
        vector<int> parent, rank;
    public:
        UnionFind(int size) {
            parent.resize(size + 1);
            rank.resize(size + 1, 0);
            for (int i = 0; i <= size; i++) parent[i] = i;
        }
        
        int find(int x) {
            if (parent[x] != x) parent[x] = find(parent[x]);
            return parent[x];
        }
        
        void unite(int x, int y) {
            x = find(x);
            y = find(y);
            if (x == y) return;
            if (rank[x] < rank[y]) swap(x, y);
            parent[y] = x;
            if (rank[x] == rank[y]) rank[x]++;
        }
        
        bool connected(int x, int y) {
            return find(x) == find(y);
        }
    };
    
    // Find edge-disjoint paths using BFS with edge removal
    int find_disjoint_paths(int s, int t) {
        if (s == t) return 0;
        
        int paths = 0;
        vector<bool> edge_used(edges.size(), false);
        
        while (paths < 3) { // Max 3 paths due to degree constraint
            vector<int> parent(n + 1, -1);
            vector<int> edge_parent(n + 1, -1);
            queue<int> q;
            q.push(s);
            parent[s] = s;
            
            bool found = false;
            while (!q.empty() && !found) {
                int u = q.front();
                q.pop();
                
                for (int i = 0; i < (int)adj[u].size(); i++) {
                    int v = adj[u][i];
                    if (parent[v] == -1) {
                        // Find edge index
                        int edge_idx = -1;
                        for (int j = 0; j < (int)edges.size(); j++) {
                            if ((edges[j].first == u && edges[j].second == v) ||
                                (edges[j].first == v && edges[j].second == u)) {
                                edge_idx = j;
                                break;
                            }
                        }
                        
                        if (edge_idx != -1 && !edge_used[edge_idx]) {
                            parent[v] = u;
                            edge_parent[v] = edge_idx;
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
            
            // Mark edges in the path as used
            int current = t;
            while (current != s) {
                int edge_idx = edge_parent[current];
                edge_used[edge_idx] = true;
                current = parent[current];
            }
            
            paths++;
        }
        
        return paths;
    }
    
public:
    FastMaxFlow(int nodes) : n(nodes) {}
    
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.push_back({u, v});
    }
    
    int max_flow(int s, int t) {
        // Quick connectivity check
        UnionFind uf(n);
        for (auto& edge : edges) {
            uf.unite(edge.first, edge.second);
        }
        if (!uf.connected(s, t)) return 0;
        
        return find_disjoint_paths(s, t);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    FastMaxFlow solver(n);
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