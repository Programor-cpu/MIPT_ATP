#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const int cDva = 2;

int DFS(int ver, const std::string& in,
        const std::vector<std::vector<std::pair<int, int>>>& graph,
        std::vector<bool>& used, std::vector<int>& colored) {
  int s = 0;
  int get = 0;
  used[ver] = true;
  for (int i = 0; i != (int)graph[ver].size(); ++i) {
    int to = graph[ver][i].first;
    if (!used[to]) {
      get = DFS(to, in, graph, used, colored);
      colored[graph[ver][i].second] = get;
      s = s + get;
    }
  }
  return ((cDva * cDva - s + (in[ver] - '0')) % cDva + cDva * cDva) % cDva;
}

void Solve(const std::vector<std::vector<std::pair<int, int>>>& graph,
           const std::string& in, int n, int m) {
  std::vector<int> colored(m);
  std::vector<bool> used(n);
  bool is_solutable = true;
  for (int i = 0; i != n; ++i) {
    if (!used[i] && is_solutable) {
      is_solutable = (DFS(i, in, graph, used, colored) == 0);
    }
  }
  if (is_solutable) {
    for (int i = 0; i != m; ++i) {
      std::cout << colored[i];
    }
  } else {
    std::cout << "-1";
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  std::string in;
  int t;
  int n;
  int m;
  int v;
  int w;
  std::cin >> t;
  int begin;
  while (t != 0) {
    --t;
    begin = 0;
    std::cin >> n >> m;
    std::vector<std::vector<std::pair<int, int>>> graph(n);
    while (m != 0) {
      std::cin >> v >> w;
      graph[v - 1].push_back({w - 1, begin});
      graph[w - 1].push_back({v - 1, begin});
      --m;
      ++begin;
    }
    std::cin >> in;
    Solve(graph, in, n, begin);
    std::cout << '\n';
  }
}