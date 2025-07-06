#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <vector>

class Graph {
 private:
  friend class Shell;
  std::vector<std::vector<int>> graph_;
  std::map<std::pair<int, int>, int> edge_control_;
  std::map<std::pair<int, int>, int> all_edges_;

 public:
  Graph(int n) : graph_(n) {}
  void AddEdge(int v, int w, int i) {
    if (v == w) {
      return;
    }
    graph_[v - 1].push_back(w - 1);
    graph_[w - 1].push_back(v - 1);
    all_edges_[{v - 1, w - 1}] = i;
    all_edges_[{w - 1, v - 1}] = i;
    if (edge_control_.find({v - 1, w - 1}) == edge_control_.end()) {
      edge_control_[{v - 1, w - 1}] = 1;
      edge_control_[{w - 1, v - 1}] = 1;
    } else {
      ++edge_control_[{v - 1, w - 1}];
      ++edge_control_[{w - 1, v - 1}];
    }
  };
};

class Shell {
 private:
  Graph orig_;
  int n_;
  struct Info {
    int current_ver;
    int parent;
    int& timer;
  };

  void DFSForBridges(
      std::pair<int&, std::vector<int>&> res, Info information,
      std::vector<bool>& used,
      std::pair<std::vector<int>&, std::vector<int>&> help_vectors) {
    int current_ver = information.current_ver;
    int parent = information.parent;
    int& timer = information.timer;
    std::vector<int>& tin = help_vectors.first;
    tin[current_ver] = timer;
    std::vector<int>& fup = help_vectors.second;
    fup[current_ver] = timer;
    used[current_ver] = true;
    ++timer;
    for (int i = 0; i != static_cast<int>(orig_.graph_[current_ver].size());
         ++i) {
      if (orig_.graph_[current_ver][i] == parent) {
        continue;
      }
      if (!used[orig_.graph_[current_ver][i]]) {
        DFSForBridges(res, {orig_.graph_[current_ver][i], current_ver, timer},
                      used, {tin, fup});
        fup[current_ver] =
            std::min(fup[orig_.graph_[current_ver][i]], fup[current_ver]);
        if (tin[current_ver] < fup[orig_.graph_[current_ver][i]] &&
            orig_.edge_control_[{current_ver, orig_.graph_[current_ver][i]}] ==
                1) {
          ++res.first;
          res.second.push_back(
              orig_.all_edges_[{current_ver, orig_.graph_[current_ver][i]}] +
              1);
        }
      } else {
        fup[current_ver] =
            std::min(tin[orig_.graph_[current_ver][i]], fup[current_ver]);
      }
    }
  }

 public:
  const int cBig = INT_MAX;
  Shell(int n) : orig_(n), n_(n) {}
  void AddEdge(int start, int finish, int i) {
    orig_.AddEdge(start, finish, i);
  };
  void FindBridges() {
    int timer = 0;
    int amount = 0;
    std::vector<int> bridges;
    std::vector<int> tin(n_, 0);
    std::vector<int> fup = tin;
    std::vector<bool> used(n_, false);
    for (int i = 0; i != n_; ++i) {
      if (!used[i]) {
        DFSForBridges({amount, bridges}, {i, -1, timer}, used, {tin, fup});
      }
    }
    std::sort(bridges.begin(), bridges.end());
    std::cout << amount << '\n';
    for (int i = 0; i != amount; ++i) {
      std::cout << bridges[i] << '\n';
    }
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  int n;
  int m;
  int v;
  int w;
  std::cin >> n >> m;
  Shell work(n);
  for (int i = 0; i != m; ++i) {
    std::cin >> v >> w;
    work.AddEdge(v, w, i);
  }
  work.FindBridges();
}