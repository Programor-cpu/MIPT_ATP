#include <iostream>
#include <vector>

const int cDx[] = {-1, 1, 0, 0};
const int cDy[] = {0, 0, -1, 1};

bool DFS(int ver, const std::vector<std::vector<int>>& graph,
         std::vector<bool>& visited, std::vector<int>& matched) {
  if (visited[ver]) {
    return false;
  }
  visited[ver] = !visited[ver];
  for (int u = 0; u != (int)graph[ver].size(); ++u) {
    if (matched[graph[ver][u]] == -1 ||
        DFS(matched[graph[ver][u]], graph, visited, matched)) {
      matched[graph[ver][u]] = ver;
      return true;
    }
  }
  return false;
}

void Count(const std::vector<std::vector<char>>& grid, std::pair<int, int> help,
           std::vector<std::vector<int>>& id, int counter, int a, int b) {
  int n = help.first;
  int m = help.second;
  std::vector<std::vector<int>> graph(counter);
  std::vector<int> matched(counter, -1);
  std::vector<bool> visited(counter, false);
  std::vector<std::pair<int, int>> position;
  int pr = 0;
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != m; ++j) {
      if (grid[i][j] == '*') {
        position.push_back({i, j});
        if ((i + j) % 2 == 0) {
          for (int k = 0; k != 4; ++k) {
            int nig = i + cDx[k];
            int njg = j + cDy[k];
            if (0 < nig + 1 && 0 < n - nig && njg + 1 > 0 && 0 < m - njg &&
                grid[nig][njg] == '*') {
              graph[id[i][j]].push_back(id[nig][njg]);
            }
          }
        }
      }
    }
  }

  for (int ver = 0; ver != counter; ++ver) {
    int i = position[ver].first;
    int j = position[ver].second;
    if ((i + j) % 2 == 0) {
      visited.assign(counter, false);
      if (DFS(ver, graph, visited, matched)) {
        ++pr;
      }
    }
  }
  std::cout << pr * a + (counter - 2 * pr) * b;
  ;
}

int main() {
  int n;
  int m;
  int a;
  int b;
  int counter = 0;
  std::cin >> n >> m >> a >> b;
  std::vector<std::vector<char>> grid(n, std::vector<char>(m, 'a'));
  std::vector<std::vector<int>> id(n, std::vector<int>(m, -1));
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != m; ++j) {
      std::cin >> grid[i][j];
      if (grid[i][j] == '*') {
        id[i][j] = counter;
        ++counter;
      }
    }
  }
  if (a >= 2 * b) {
    std::cout << counter * b;
  } else {
    Count(grid, {n, m}, id, counter, a, b);
  }
}
