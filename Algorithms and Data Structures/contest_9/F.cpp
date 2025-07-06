#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

struct Edge {
  int to;
  int rev;
  int capacity;
  int flow;
};

class Graph {
 private:
  friend class Shell;

  std::vector<std::vector<Edge>> graph_;

 public:
  Graph(int n) : graph_(n, std::vector<Edge>(0)) {}

  void AddEdge(int u, int v, int c) {
    graph_[u].push_back({v, (int)graph_[v].size(), c, 0});
    graph_[v].push_back({u, (int)graph_[u].size() - 1, c, 0});
  }
};

class Shell {
 private:
  int n_;
  Graph g_;
  std::vector<int> level_;
  std::vector<int> ptr_;

  int DFS(int v, int t, int flow) {
    if (flow == 0 || v == t) {
      return flow;
    }
    while (ptr_[v] != static_cast<int>(g_.graph_[v].size())) {
      Edge& edge = g_.graph_[v][ptr_[v]];
      if (level_[edge.to] - level_[v] == 1) {
        int pushed = DFS(edge.to, t, std::min(flow, edge.capacity - edge.flow));
        if (pushed > 0) {
          g_.graph_[edge.to][edge.rev].flow -= pushed;
          edge.flow += pushed;
          return pushed;
        }
      }
      ++ptr_[v];
    }
    return 0;
  }
  bool BFS(int s, int t) {
    level_.assign(n_, -1);
    std::queue<int> bfs_queue;
    level_[s] = 0;
    bfs_queue.push(s);
    while (!bfs_queue.empty()) {
      int ver = bfs_queue.front();
      bfs_queue.pop();
      for (Edge& e : g_.graph_[ver]) {
        if (level_[e.to] == -1 && e.flow < e.capacity) {
          level_[e.to] = level_[ver] + 1;
          bfs_queue.push(e.to);
        }
      }
    }
    return level_[t] != -1;
  }

  const int cBig = 2147483647;

 public:
  Shell(int n) : n_(n), g_(n), level_(n_, -1) {}

  void AddEdge(int u, int v, int c) { g_.AddEdge(u, v, c); }

  int MaxFlow(int s, int t) {
    int total = 0;
    while (BFS(s, t)) {
      ptr_.assign(n_, 0);
      while (int pushed = DFS(s, t, cBig)) {
        total += pushed;
      }
    }
    return total;
  }

  std::vector<std::pair<int, int>> FindCut(int s) {
    std::vector<bool> visited(n_, false);
    std::queue<int> q;
    q.push(s);
    visited[s] = true;
    while (!q.empty()) {
      int v = q.front();
      q.pop();
      for (Edge& edge : g_.graph_[v]) {
        if (!visited[edge.to] && edge.capacity > edge.flow) {
          visited[edge.to] = true;
          q.push(edge.to);
        }
      }
    }

    std::vector<std::pair<int, int>> cuts;
    for (int v = 0; v != n_; ++v) {
      if (visited[v]) {
        for (Edge& e : g_.graph_[v]) {
          if (!visited[e.to] && e.capacity > 0) {
            cuts.push_back({v, e.to});
          }
        }
      }
    }
    return cuts;
  }
};

struct Info {
  int from;
  int to;
  int cap;
  int index;
};
int main() {
  int n;
  int m;
  int u;
  int v;
  int c;
  std::vector<int> indexes;
  int sum_cap = 0;
  std::cin >> n >> m;

  Shell work(n);
  std::vector<Info> edges;

  for (int i = 0; i != m; ++i) {
    std::cin >> u >> v >> c;
    work.AddEdge(u - 1, v - 1, c);
    edges.push_back({u - 1, v - 1, c, i + 1});
  }

  work.MaxFlow(0, n - 1);
  std::vector<std::pair<int, int>> cuts = work.FindCut(0);

  for (auto [u, v] : cuts) {
    for (auto [ver_u, ver_v, cap, index] : edges) {
      if ((u == ver_u && v == ver_v) || (v == ver_u && u == ver_v)) {
        indexes.push_back(index);
        sum_cap += cap;
        break;
      }
    }
  }

  std::sort(indexes.begin(), indexes.end());
  indexes.erase(std::unique(indexes.begin(), indexes.end()), indexes.end());

  std::cout << indexes.size() << " " << sum_cap << "\n";
  for (int i = 0; i != static_cast<int>(indexes.size()); ++i) {
    std::cout << indexes[i] << " ";
  }
}