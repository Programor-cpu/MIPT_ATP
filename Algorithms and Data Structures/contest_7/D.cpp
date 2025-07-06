#include <algorithm>
#include <climits>
#include <iostream>
#include <set>
#include <vector>

class Graph {
 private:
  friend class Shell;
  std::vector<std::vector<int>> graph_;

 public:
  Graph(int n) : graph_(n) {}
  void AddEdge(int one, int two) { graph_[one].push_back(two); };
};

class Shell {
 private:
  Graph orig_;
  Graph inverted_;
  std::vector<int> distances_;
  int n_;
  void DFS(int ver, int component, std::vector<int>& components,
           std::vector<bool>& used) {
    used[ver] = true;
    components[ver] = component;
    for (int i = 0; i != (int)inverted_.graph_[ver].size(); ++i) {
      int to = inverted_.graph_[ver][i];
      if (!used[to]) {
        DFS(to, component, components, used);
      }
    }
  }
  void TopologicalSortMechanism(int ver, std::vector<int>& topologicly_sorted,
                                std::vector<bool>& used) {
    used[ver] = true;
    for (int i = 0; i != (int)orig_.graph_[ver].size(); ++i) {
      int to = orig_.graph_[ver][i];
      if (!used[to]) {
        TopologicalSortMechanism(to, topologicly_sorted, used);
      }
    }
    topologicly_sorted.push_back(ver);
  }

 public:
  const int cBig = INT_MAX;
  Shell(int n) : orig_(n), inverted_(n), distances_(n, cBig), n_(n) {}
  void AddEdge(int start, int finish) {
    orig_.AddEdge(start, finish);
    inverted_.AddEdge(finish, start);
  };
  void TopSort() {
    int components_amount = 0;
    std::vector<int> components(n_);
    std::vector<int> topologicly_sorted;
    std::vector<bool> used(n_, false);
    for (int i = 0; i != n_; ++i) {
      if (!used[i]) {
        TopologicalSortMechanism(i, topologicly_sorted, used);
      }
    }
    used.assign(n_, false);
    for (int i = (int)topologicly_sorted.size() - 1; i != -1; --i) {
      if (!used[topologicly_sorted[i]]) {
        ++components_amount;
        DFS(topologicly_sorted[i], components_amount, components, used);
      }
    }
    std::cout << components_amount << '\n';
    for (int i = 0; i != n_; ++i) {
      std::cout << components[i] << ' ';
    }
  }
};

int main() {
  int n;
  int m;
  int u;
  int v;
  std::cin >> n >> m;
  Shell work(n);
  std::set<std::pair<int, int>> check;
  while (m != 0) {
    --m;
    std::cin >> u >> v;
    if (u == v) {
      continue;
    }
    if (check.find({
            u - 1,
            v - 1,
        }) == check.end()) {
      work.AddEdge(u - 1, v - 1);
      check.insert({
          u - 1,
          v - 1,
      });
    }
  }
  work.TopSort();
}