#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

const int cBig = INT_MAX;

bool BFS(const std::vector<std::vector<int>>& graph, int n,
         std::vector<int>& pair_u, std::vector<int>& pair_v,
         std::vector<int>& distance) {
  std::queue<int> query;
  bool find = false;
  for (int i = 0; i != n; ++i) {
    if (pair_u[i] + 1 != 0) {
      distance[i] = cBig;
    } else {
      distance[i] = 0;
      query.push(i);
    }
  }

  while (!query.empty()) {
    int u = query.front();
    query.pop();
    for (int ver = 0; ver != (int)graph[u].size(); ++ver) {
      if (pair_v[graph[u][ver]] + 1 == 0) {
        find = true;
      } else if (distance[pair_v[graph[u][ver]]] == cBig) {
        distance[pair_v[graph[u][ver]]] = distance[u];
        ++distance[pair_v[graph[u][ver]]];
        query.push(pair_v[graph[u][ver]]);
      }
    }
  }

  return find;
}

bool DFS(int u, const std::vector<std::vector<int>>& graph, int n,
         std::vector<int>& pair_u, std::vector<int>& pair_v,
         std::vector<int>& distance) {
  for (int ver = 0; ver != (int)graph[u].size(); ++ver) {
    if (pair_v[graph[u][ver]] + 1 == 0 ||
        (distance[pair_v[graph[u][ver]]] - 1 == distance[u] &&
         DFS(pair_v[graph[u][ver]], graph, n, pair_u, pair_v, distance))) {
      pair_u[u] = graph[u][ver];
      pair_v[graph[u][ver]] = u;
      return true;
    }
  }
  distance[u] = cBig;
  return false;
}

int HopcroftKarp(const std::vector<std::vector<int>>& graph, int n) {
  std::vector<int> pair_u(n, -1);
  std::vector<int> pair_v(n, -1);
  std::vector<int> distance(n, 0);
  int matched = 0;
  while (BFS(graph, n, pair_u, pair_v, distance)) {
    for (int i = 0; i != n; ++i) {
      if (pair_u[i] + 1 == 0 && DFS(i, graph, n, pair_u, pair_v, distance)) {
        ++matched;
      }
    }
  }
  return n - matched;
}
int main() {
  int n;
  int m;
  std::cin >> n >> m;
  std::vector<std::vector<int>> graph(n);

  for (int i = 0; i < m; ++i) {
    int u;
    int v;
    std::cin >> u >> v;
    graph[u - 1].push_back(v - 1);
  }

  std::cout << HopcroftKarp(graph, n);
}
