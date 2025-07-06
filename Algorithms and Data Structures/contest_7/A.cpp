#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

const int cWhite = -1;
const int cGrey = 0;
const int cBlack = 1;

void DFS(const std::vector<std::vector<int>>& graf, std::vector<int>& used_ones,
         std::vector<int>& colors, std::pair<int, int> vers, bool& is_cycle,
         int& cycle) {
  colors[vers.first] = cGrey;
  used_ones[vers.first] = vers.second;
  for (int i = 0; i != (int)graf[vers.first].size(); i++) {
    if (used_ones[graf[vers.first][i]] == cWhite ||
        colors[graf[vers.first][i]] != cGrey) {
      if (colors[graf[vers.first][i]] != cBlack) {
        DFS(graf, used_ones, colors, {graf[vers.first][i], vers.first},
            is_cycle, cycle);
      }
    } else {
      cycle = vers.first;
      used_ones[graf[vers.first][i]] = cycle;
      is_cycle = true;
      break;
    }
  }
  colors[vers.first] = cBlack;
}

void Solve(const std::vector<std::vector<int>>& graf, int n) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  std::vector<int> colors(n, cWhite);
  std::vector<int> used_ones(n, cWhite);
  bool is_cycle = false;
  int start = cGrey;
  for (int i = 0; i != n; ++i) {
    if (used_ones[i] == cWhite) {
      DFS(graf, used_ones, colors, {i, cWhite}, is_cycle, start);
    }
    if (is_cycle) {
      break;
    }
  }
  if (is_cycle) {
    std::cout << "YES" << '\n';
    colors.clear();
    int rem = start;
    while (used_ones[start] != rem) {
      colors.push_back(start + cBlack);
      start = used_ones[start];
    }
    colors.push_back(start + cBlack);
    for (int i = (int)colors.size() - 1; i != -1; i--) {
      std::cout << colors[i] << ' ';
    }
  } else {
    std::cout << "NO" << '\n';
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
  Solve(graf, n);
}