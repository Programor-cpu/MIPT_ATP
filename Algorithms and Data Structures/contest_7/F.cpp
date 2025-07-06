#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

struct Info {
  const std::vector<std::vector<int>>& graf;
  std::vector<int>& tin;
  std::vector<int>& fup;
  std::vector<bool>& used;
};

void DFS(int ver, int p, int& timer, Info& info, std::set<int>& points) {
  int c = 0;
  info.used[ver] = true;
  info.tin[ver] = timer;
  info.fup[ver] = timer;
  ++timer;
  for (int i = 0; i != (int)info.graf[ver].size(); i++) {
    if (info.graf[ver][i] - p == 0) {
      continue;
    }
    if (!info.used[info.graf[ver][i]]) {
      DFS(info.graf[ver][i], ver, timer, info, points);
      info.fup[ver] = std::min(info.fup[ver], info.fup[info.graf[ver][i]]);
      if (p + 1 != 0 && 0 <= info.fup[info.graf[ver][i]] - info.tin[ver]) {
        points.insert(ver + 1);
      }
      c = c + 1;
    } else {
      info.fup[ver] = std::min(info.fup[ver], info.tin[info.graf[ver][i]]);
    }
  }
  if (0 < c - 1 && p + 1 == 0) {
    points.insert(ver + 1);
  }
}

void Connectivity(const std::vector<std::vector<int>>& graf, int n) {
  std::vector<bool> used(n, false);
  std::vector<int> tin(n, 0);
  std::vector<int> fup = tin;
  std::set<int> points;
  int timer = 0;

  Info inf = {graf, tin, fup, used};
  for (int i = 0; i != n; ++i) {
    if (!used[i]) {
      DFS(i, -1, timer, inf, points);
    }
  }
  std::cout << points.size() << '\n';
  for (auto it : points) {
    std::cout << it << ' ';
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  std::map<int, std::set<int>> edges;
  int n;
  int m;
  int v;
  int w;

  std::cin >> n >> m;
  std::vector<std::vector<int>> graf(n, std::vector<int>());
  while (m != 0) {
    std::cin >> v >> w;
    --m;
    if (v == w || edges[v - 1].find(w - 1) != edges[v - 1].end()) {
      continue;
    }
    edges[v - 1].insert(w - 1);
    edges[w - 1].insert(v - 1);
    graf[v - 1].push_back(w - 1);
    graf[w - 1].push_back(v - 1);
  }
  Connectivity(graf, n);
}