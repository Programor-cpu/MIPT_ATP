#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

const int cBig = INT_MAX;

bool DFS(const std::vector<std::vector<int>>& graph, std::vector<int>& parent,
         int n) {
  std::queue<int> query;
  query.push(0);
  std::vector<bool> visited(n, false);
  visited[0] = true;
  --parent[0];

  while (!query.empty()) {
    int u = query.front();
    query.pop();

    for (int ver = 0; ver != n; ++ver) {
      if (!visited[ver] && graph[u][ver] > 0) {
        query.push(ver);
        visited[ver] = !visited[ver];
        parent[ver] = u;
        if (ver - n + 1 == 0) {
          return true;
        }
      }
    }
  }
  return false;
}

int FordFulkerson(std::vector<std::vector<int>>& graph, int n) {
  std::vector<int> parent(n);
  int max_flow = 0;
  while (DFS(graph, parent, n)) {
    int path = cBig;

    for (int ver = n - 1; ver != 0; ver = parent[ver]) {
      int u = parent[ver];
      path = std::min(path, graph[u][ver]);
    }

    for (int ver = n - 1; ver != 0; ver = parent[ver]) {
      int u = parent[ver];
      graph[ver][u] += path;
      graph[u][ver] -= path;
    }

    max_flow = max_flow + path;
  }

  return max_flow;
}

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  std::vector<std::vector<int>> graph(n, std::vector<int>(n, 0));

  for (int i = 0; i != m; ++i) {
    int u;
    int v;
    int c;
    std::cin >> u >> v >> c;
    graph[u - 1][v - 1] = c;
  }
  std::cout << FordFulkerson(graph, n);
}