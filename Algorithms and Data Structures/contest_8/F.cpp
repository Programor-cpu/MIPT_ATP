#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

const int cInf = INT_MAX;

std::vector<std::vector<int>> FloydWarshall(
    const std::vector<std::vector<int>>& graph,
    std::vector<std::vector<int>>& next, int n) {
  std::vector<std::vector<int>> out = graph;
  for (int k = 0; k != n; ++k) {
    for (int i = 0; i != n; ++i) {
      for (int j = 0; j != n; ++j) {
        if ((out[i][j] == cInf || out[i][j] > (out[i][k] + out[k][j])) &&
            (out[i][k] != cInf && out[k][j] != cInf)) {
          out[i][j] = out[i][k] + out[k][j];
          next[i][j] = next[i][k];
        }
      }
    }
  }
  return out;
}
std::vector<std::vector<int>> FloydWarshallNo(
    const std::vector<std::vector<int>>& graph, int n) {
  std::vector<std::vector<int>> out = graph;
  for (int k = 0; k != n; ++k) {
    for (int i = 0; i != n; ++i) {
      for (int j = 0; j != n; ++j) {
        if ((out[i][j] == cInf || out[i][j] > (out[i][k] + out[k][j])) &&
            (out[i][k] != cInf && out[k][j] != cInf)) {
          out[i][j] = out[i][k] + out[k][j];
        }
      }
    }
  }
  return out;
}

int main() {
  int n;
  int m;
  int k;
  std::cin >> n >> m >> k;
  std::vector<std::vector<int>> graph(n, std::vector<int>(n, cInf));
  std::vector<std::vector<int>> numbers(n, std::vector<int>(n, cInf));
  std::vector<std::vector<int>> next(n, std::vector<int>(n, -1));
  for (int i = 0; i != m; ++i) {
    int u;
    int v;
    int c;
    std::cin >> u >> v >> c;
    graph[u - 1][v - 1] = -c;
    numbers[u - 1][v - 1] = i;
  }
  for (int i = 0; i != n; ++i) {
    graph[i][i] = 0;
  }
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != n; ++j) {
      next[i][j] = j;
    }
  }
  std::vector<int> turne(k);
  for (int i = 0; i != k; ++i) {
    std::cin >> turne[i];
    --turne[i];
  }
  std::vector<std::vector<int>> updated = FloydWarshall(graph, next, n);
  bool flag = true;
  std::vector<std::vector<int>> reserve = FloydWarshallNo(updated, n);
  for (int i = 0; i != k - 1; ++i) {
    if (updated[turne[i]][turne[i + 1]] > reserve[turne[i]][turne[i + 1]]) {
      std::cout << "infinitely kind";
      flag = false;
      break;
    }
  }
  if (flag) {
    std::vector<int> way;
    int index = 0;
    while (index != k - 1) {
      int grad = turne[index];
      while (grad != turne[index + 1]) {
        way.push_back(numbers[grad][next[grad][turne[index + 1]]] + 1);
        grad = next[grad][turne[index + 1]];
      }
      ++index;
    }

    std::cout << way.size() << '\n';
    for (int i = 0; i != (int)way.size(); ++i) {
      std::cout << way[i] << ' ';
    }
  }
}