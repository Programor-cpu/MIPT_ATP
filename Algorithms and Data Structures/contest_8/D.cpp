#include <algorithm>
#include <iostream>
#include <vector>

class Graph {
 private:
  friend class Shell;
  struct Edge {
    int from;
    int to;
    int weight;
  };
  std::vector<std::vector<Edge>> graph_;

 public:
  Graph(int n) : graph_(n) {}
  void AddEdge(int start, int finish, int weight) {
    graph_[start].push_back({start, finish, weight});
  };
};

class Shell {
 private:
  Graph g_;
  std::vector<int> distances_;
  int n_;

 public:
  const int cBig = 30000;
  Shell(int n) : g_(n), distances_(n, cBig), n_(n) {
    for (int i = 0; i != n; ++i) {
      distances_[i] = cBig;
    }
  }
  void AddEdge(int start, int finish, int weight) {
    g_.AddEdge(start, finish, weight);
  };
  std::vector<int> FordBellman() {
    distances_[0] = 0;
    for (int w = 1; w != n_; ++w) {
      for (int i = 0; i != n_; ++i) {
        for (int j = 0; j != (int)g_.graph_[i].size(); ++j) {
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
};

int main() {
  int n;
  std::cin >> n;
  Shell work(n);
  int m;
  int u;
  int v;
  int c;
  std::cin >> m;
  while (m != 0) {
    --m;
    std::cin >> u >> v >> c;
    work.AddEdge(u - 1, v - 1, c);
  }
  std::vector<int> distances = work.FordBellman();
  for (int i = 0; i != n; ++i) {
    std::cout << distances[i] << ' ';
  }
}