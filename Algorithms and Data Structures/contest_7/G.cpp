#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

struct Info {
  int current_ver;
  int parent;
  int& timer;
};

void DFSForBridges(std::pair<int&, std::set<std::pair<int, int>>&> res,
                   Info information, const std::vector<std::vector<int>>& graph,
                   std::vector<bool>& used,
                   std::pair<std::vector<int>&, std::vector<int>&> help_vectors,
                   std::pair<std::map<std::pair<int, int>, int>&,
                             std::map<std::pair<int, int>, int>&>
                       help_maps) {
  int current_ver = information.current_ver;
  int parent = information.parent;
  int& timer = information.timer;
  std::vector<int>& tin = help_vectors.first;
  tin[current_ver] = timer;
  std::vector<int>& fup = help_vectors.second;
  fup[current_ver] = timer;
  used[current_ver] = true;
  ++timer;
  for (int i = 0; i != static_cast<int>(graph[current_ver].size()); ++i) {
    if (graph[current_ver][i] == parent) {
      continue;
    }
    if (!used[graph[current_ver][i]]) {
      DFSForBridges(res, {graph[current_ver][i], current_ver, timer}, graph,
                    used, {tin, fup}, help_maps);
      fup[current_ver] = std::min(fup[graph[current_ver][i]], fup[current_ver]);
      if (tin[current_ver] < fup[graph[current_ver][i]] &&
          help_maps.first[{current_ver, graph[current_ver][i]}] == 1) {
        ++res.first;
        res.second.insert({current_ver, graph[current_ver][i]});
        res.second.insert({graph[current_ver][i], current_ver});
      }
    } else {
      fup[current_ver] = std::min(tin[graph[current_ver][i]], fup[current_ver]);
    }
  }
}

bool DFS(int ver, const std::vector<std::vector<int>>& graph,
         const std::set<std::pair<int, int>>& bridges, std::vector<bool>& used,
         std::set<std::pair<int, int>>& leaves) {
  used[ver] = true;
  bool statistic = false;
  bool save;
  for (int i = 0; i != (int)graph[ver].size(); i++) {
    if (!used[graph[ver][i]]) {
      save = DFS(graph[ver][i], graph, bridges, used, leaves);
      statistic |= save;
      if (!save && bridges.find({ver, graph[ver][i]}) != bridges.end()) {
        leaves.insert({ver, graph[ver][i]});
        leaves.insert({graph[ver][i], ver});
        statistic = true;
      }
    }
  }
  return statistic;
}

void FindBridges(const std::vector<std::vector<int>>& graph, int n,
                 std::map<std::pair<int, int>, int>& edge_control,
                 std::map<std::pair<int, int>, int>& all_edges) {
  int timer = 0;
  int amount = 0;
  std::set<std::pair<int, int>> bridges;
  std::set<std::pair<int, int>> leaves;
  std::vector<int> tin(n, 0);
  std::vector<int> fup = tin;
  std::vector<bool> used(n, false);
  DFSForBridges({amount, bridges}, {0, -1, timer}, graph, used, {tin, fup},
                {edge_control, all_edges});
  used.assign(n, false);
  if (bridges.empty()) {
    std::cout << 0 << '\n';
    return;
  }
  DFS(0, graph, bridges, used, leaves);
  used.assign(n, false);
  DFS(leaves.begin()->first, graph, bridges, used, leaves);
  int amount_l = (int)leaves.size();
  amount_l /= 2;
  std::cout << (amount_l / 2 + amount_l % 2) << '\n';
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  int n;
  int m;
  int v;
  int w;
  std::cin >> n >> m;
  std::map<std::pair<int, int>, int> edge_control;
  std::map<std::pair<int, int>, int> all_edges;
  std::vector<std::vector<int>> graph(n, std::vector<int>());
  for (int i = 0; i != m; ++i) {
    std::cin >> v >> w;
    if (v == w) {
      continue;
    }
    graph[v - 1].push_back(w - 1);
    graph[w - 1].push_back(v - 1);
    all_edges[{v - 1, w - 1}] = i;
    all_edges[{w - 1, v - 1}] = i;
    if (edge_control.find({v - 1, w - 1}) == edge_control.end()) {
      edge_control[{v - 1, w - 1}] = 1;
      edge_control[{w - 1, v - 1}] = 1;
    } else {
      ++edge_control[{v - 1, w - 1}];
      ++edge_control[{w - 1, v - 1}];
    }
  }
  FindBridges(graph, n, edge_control, all_edges);
}