#include <algorithm>
#include <iostream>
#include <vector>

const long long cAbort = 10000;

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
  const long long cBig = 9223372036854775807;
  Shell(long long n) : g_(n), distances_(n, cBig), n_(n) {
    for (long long i = 0; i != n; ++i) {
      distances_[i] = 0;
    }
  }
  void AddEdge(long long start, long long finish, long long weight) {
    g_.AddEdge(start, finish, weight);
  };
  std::vector<long long> FordBellman() {
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
    distances_[0] = 0;
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
};

int main() {
  long long n;
  std::cin >> n;
  Shell work(n);
  long long c;
  for (long long i = 0; i != n; ++i) {
    for (long long j = 0; j != n; ++j) {
      std::cin >> c;
      if (c > cAbort) {
        continue;
      }
      work.AddEdge(i, j, c);
    }
  }
  std::vector<long long> cycle = work.NegativeCycle();
  if (cycle.empty()) {
    std::cout << "NO" << '\n';
  } else {
    std::cout << "YES" << '\n';
    std::cout << cycle.size() << '\n';
    for (long long i = 0; i != (long long)cycle.size(); ++i) {
      std::cout << cycle[i] + 1 << ' ';
    }
  }
}