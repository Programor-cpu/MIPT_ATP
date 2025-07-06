#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

class Graph {
 private:
  friend class Shell;
  struct Edge {
    long long from;
    long long to;
    long long capacity;
    long long flow;
    int rev_index;
    int id;
  };
  std::vector<std::vector<Edge>> graph_;
  std::vector<Edge*> original_edges_;

 public:
  Graph(long long n) : graph_(n) {}

  void AddEdge(long long u, long long v, long long c, int i) {
    graph_[u].push_back({u, v, c, 0, (int)graph_[v].size(), i});
    graph_[v].push_back({v, u, 0, 0, (int)graph_[u].size() - 1, -1});
    original_edges_.push_back(&graph_[u].back());
  }
};

class Shell {
 private:
  Graph g_;
  long long n_;
  std::vector<long long> level_;
  std::vector<long long> ptr_;
  const long long cBig = LLONG_MAX;

 public:
  Shell(long long n) : g_(n), n_(n), level_(n, -1), ptr_(n) {}

  void AddEdge(long long u, long long v, long long c, int idx) {
    g_.AddEdge(u - 1, v - 1, c, idx);
  }

  long long DFS(long long u, long long t, long long pushed, long long bound) {
    if (pushed == 0 || u == t) {
      return pushed;
    }
    long long tr = 0;
    for (long long& seed = ptr_[u];
         seed != static_cast<long long>(g_.graph_[u].size()); ++seed) {
      auto& edge = g_.graph_[u][seed];
      if (level_[edge.to] - 1 == level_[u] &&
          edge.capacity >= bound + edge.flow) {
        tr =
            DFS(edge.to, t, std::min(edge.capacity - edge.flow, pushed), bound);
        if (tr > 0) {
          edge.flow += tr;
          g_.graph_[edge.to][edge.rev_index].flow -= tr;
          return tr;
        }
      }
    }
    return 0;
  }

  bool BFS(long long start, long long t, long long bound) {
    level_.assign(n_, -1);
    std::queue<long long> q;
    q.push(start);
    level_[start] = 0;
    long long up = -1;
    while (!q.empty()) {
      up = q.front();
      q.pop();
      for (auto& edge : g_.graph_[up]) {
        if (level_[edge.to] == -1 && edge.capacity >= bound + edge.flow) {
          level_[edge.to] = level_[up] + 1;
          q.push(edge.to);
        }
      }
    }
    return (level_[t] != -1);
  }

  long long ScalingDinic(long long start, long long sink) {
    long long max_capacity = 0;
    long long flow = 0;
    for (auto& edges : g_.graph_) {
      for (auto& edge : edges) {
        max_capacity = std::max(edge.capacity, max_capacity);
      }
    }

    for (long long bound = cBig; bound > 0; bound /= 2) {
      while (BFS(start, sink, bound)) {
        ptr_.assign(n_, 0);
        while (long long pushed = DFS(start, sink, cBig, bound)) {
          flow += pushed;
        }
      }
    }
    return flow;
  }

  std::vector<long long> Flows(long long m) {
    std::vector<long long> flows(m, 0);
    for (long long u = 0; u != static_cast<long long>(g_.graph_.size()); ++u) {
      for (auto& edge : g_.graph_[u]) {
        if (m > edge.id && edge.id >= 0) {
          flows[edge.id] += edge.flow;
        }
      }
    }
    return flows;
  }
};

int main() {
  long long n;
  long long m;
  std::cin >> n >> m;
  long long u;
  long long v;
  long long c;
  Shell work(n);
  for (long long i = 0; i != m; ++i) {
    std::cin >> u >> v >> c;
    work.AddEdge(u, v, c, i);
  }
  long long max_flow = work.ScalingDinic(0, n - 1);
  std::cout << max_flow << '\n';
  std::vector<long long> flows = work.Flows(m);
  for (int i = 0; i != static_cast<int>(flows.size()); ++i) {
    std::cout << flows[i] << '\n';
  }
}
