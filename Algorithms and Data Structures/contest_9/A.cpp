#include <algorithm>
#include <iostream>
#include <vector>

class Graph {
 private:
  friend class Shell;

  long long n_left_;

  std::vector<long long> mr_;
  std::vector<std::vector<long long>> graph_;
  std::vector<bool> used_;

 public:
  Graph(long long nl, long long nr)
      : n_left_(nl), mr_(nr + 1, -1), graph_(nl + 1), used_(nl + 1, false) {}

  void AddEdge(long long u, long long v) { graph_[u].push_back(v); }
};

class Shell {
 private:
  Graph g_;

 public:
  Shell(long long n_left, long long n_right) : g_(n_left, n_right) {}

  void AddEdge(long long u, long long v) { g_.AddEdge(u, v); }

  bool DFS(long long u) {
    if (g_.used_[u]) {
      return false;
    }
    g_.used_[u] = true;
    for (long long ver : g_.graph_[u]) {
      if (g_.mr_[ver] == -1 || DFS(g_.mr_[ver])) {
        g_.mr_[ver] = u;
        return true;
      }
    }
    return false;
  }

  void Kuhn() {
    long long match = 0;
    for (long long u = 0; u != g_.n_left_; ++u) {
      g_.used_.assign(g_.n_left_ + 1, false);
      if (DFS(u + 1)) {
        ++match;
      }
    }

    std::cout << match << "\n";
    for (long long v = 0; v != (int)g_.mr_.size() - 1; ++v) {
      if (g_.mr_[v + 1] != -1) {
        std::cout << g_.mr_[v + 1] << " " << v + 1 << "\n";
      }
    }
  }
};

int main() {
  long long n;
  long long k;
  std::cin >> n >> k;
  Shell work(n, k);
  long long v;

  for (long long i = 0; i != n; ++i) {
    v = -1;
    while (v != 0) {
      std::cin >> v;
      if (v == 0) {
        break;
      }
      work.AddEdge(i + 1, v);
    }
  }

  work.Kuhn();
}
