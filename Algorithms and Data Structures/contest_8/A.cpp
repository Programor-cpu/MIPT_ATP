#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

class Graph {
 private:
  friend class Shell;
  struct Edge {
    long long from;
    long long to;
    long long weight;
  };
  std::vector<std::vector<Edge>> graph_;

 public:
  Graph(long long n) : graph_(n) {}
  void AddEdge(long long start, long long finish, long long weight) {
    graph_[start].push_back({start, finish, weight});
  };
};

class Shell {
 private:
  Graph g_;
  std::vector<long long> distances_;
  long long n_;

 public:
  const long long cBig = 2009000999;
  Shell(long long n) : g_(n), distances_(n, cBig), n_(n) {}
  void AddEdge(long long start, long long finish, long long weight) {
    g_.AddEdge(start, finish, weight);
  };
  std::vector<long long> FordBellman() {
    for (long long i = 0; i != n_; ++i) {
      distances_[i] = cBig;
    }
    distances_[0] = 0;
    for (long long w = 1; w != n_; ++w) {
      for (long long i = 0; i != n_; ++i) {
        for (long long j = 0; j != (long long)g_.graph_[i].size(); ++j) {
          if (distances_[i] != cBig &&
              distances_[g_.graph_[i][j].to] >
                  distances_[i] + g_.graph_[i][j].weight) {
            distances_[g_.graph_[i][j].to] =
                distances_[g_.graph_[i][j].from] + g_.graph_[i][j].weight;
          }
        }
      }
    }
    return distances_;
  }
  std::vector<long long> NegativeCycle() {
    for (long long i = 0; i != n_; ++i) {
      distances_[i] = 0;
    }
    long long flag = -1;
    std::vector<long long> parent(n_, -1);
    std::vector<long long> cycle;
    for (long long w = 0; w != n_; ++w) {
      flag = -1;
      for (long long i = 0; i != n_; ++i) {
        for (long long j = 0; j != (long long)g_.graph_[i].size(); ++j) {
          if (distances_[i] < cBig &&
              distances_[g_.graph_[i][j].to] >
                  distances_[i] + g_.graph_[i][j].weight) {
            distances_[g_.graph_[i][j].to] = std::max(
                distances_[g_.graph_[i][j].from] + g_.graph_[i][j].weight,
                -cBig);
            parent[g_.graph_[i][j].to] = g_.graph_[i][j].from;
            flag = g_.graph_[i][j].to;
          }
        }
      }
    }
    if (flag == -1) {
      return cycle;
    }

    for (long long i = 0; i != n_; ++i) {
      flag = parent[flag];
    }
    for (long long ver = flag;; ver = parent[ver]) {
      cycle.push_back(ver);
      if (ver == flag && 1 < (long long)cycle.size()) {
        break;
      }
    }
    std::reverse(cycle.begin(), cycle.end());
    return cycle;
  }
  std::vector<long long> Dijkstra(long long start) {
    distances_.assign(n_, cBig);
    distances_[start] = 0;
    std::set<std::pair<int, int>> ds;
    ds.insert(std::make_pair(0, start));
    while (!ds.empty()) {
      std::pair<int, int> min = *(ds.begin());
      ds.erase(ds.begin());

      int u = min.second;
      for (int i = 0; i != (int)g_.graph_[u].size(); ++i) {
        int v = g_.graph_[u][i].to;
        int weight = g_.graph_[u][i].weight;
        if (distances_[u] < distances_[v] - weight) {
          if (distances_[v] != cBig) {
            ds.erase(ds.find(std::make_pair(distances_[v], v)));
          }

          distances_[v] = weight + distances_[u];
          ds.insert(std::make_pair(distances_[v], v));
        }
      }
    }
    return distances_;
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  long long t;
  std::cin >> t;
  while (t != 0) {
    long long n;
    std::cin >> n;
    Shell work(n);
    long long m;
    long long u;
    long long v;
    long long c;
    std::cin >> m;
    while (m != 0) {
      --m;
      std::cin >> u >> v >> c;
      work.AddEdge(u, v, c);
      work.AddEdge(v, u, c);
    }
    long long s;
    std::cin >> s;
    std::vector<long long> distances = work.Dijkstra(s);
    for (long long i = 0; i != n; ++i) {
      std::cout << distances[i] << ' ';
    }
    std::cout << '\n';
    --t;
  }
}