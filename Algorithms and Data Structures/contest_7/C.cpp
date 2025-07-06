#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

void DFS(bool& pos, int ver, const std::vector<std::vector<int>>& graf,
         std::vector<int>& used, std::vector<int>& colors,
         std::vector<int>& sorted) {
  used[ver]++;
  colors[ver]++;
  for (int i = 0; i != (int)graf[ver].size(); i++) {
    if (colors[graf[ver][i]] - 1 != 0) {
      if (used[graf[ver][i]] == 0) {
        DFS(pos, graf[ver][i], graf, used, colors, sorted);
      }
    } else {
      pos = false;
      break;
    }
  }
  sorted.push_back(ver);
  colors[ver]++;
}

void Topsort(const std::vector<std::vector<int>>& graf, int n) {
  bool possibility = true;
  std::vector<int> used(n, 0);
  std::vector<int> colors(n, 0);
  std::vector<int> sorted;
  for (int i = 0; i != n; ++i) {
    if (used[i] != 1) {
      DFS(possibility, i, graf, used, colors, sorted);
    }
  }
  if (possibility) {
    for (int i = (int)sorted.size() - 1; i != -1; i--) {
      std::cout << sorted[i] + 1 << ' ';
    }
  } else {
    std::cout << "-1";
  }
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
  std::vector<std::vector<int>> graf(n, std::vector<int>());
  while (m != 0) {
    std::cin >> v >> w;
    graf[v - 1].push_back(w - 1);
    --m;
  }
  Topsort(graf, n);
}