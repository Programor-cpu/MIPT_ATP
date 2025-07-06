#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

void DFS(int ver, const std::vector<std::vector<int>>& graf,
         std::vector<int>& used, std::vector<int>& components, int counter) {
  used[ver] = counter;
  components.push_back(ver);
  for (int i = 0; i != (int)graf[ver].size(); i++) {
    if (used[graf[ver][i]] == 0) {
      DFS(graf[ver][i], graf, used, components, counter);
    }
  }
}

void Components(const std::vector<std::vector<int>>& graf, int n) {
  std::vector<int> used(n, 0);
  std::vector<std::vector<int>> components;
  int counter = 0;
  for (int i = 0; i != n; ++i) {
    if (used[i] == 0) {
      ++counter;
      components.push_back(std::vector<int>());
      DFS(i, graf, used, components[counter - 1], counter);
    }
  }
  std::cout << components.size() << '\n';
  for (int i = 0; i != (int)components.size(); ++i) {
    std::cout << components[i].size() << '\n';
    for (int j = 0; j != (int)components[i].size(); ++j) {
      std::cout << components[i][j] + 1 << ' ';
    }
    std::cout << '\n';
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
    graf[w - 1].push_back(v - 1);
    --m;
  }
  Components(graf, n);
}