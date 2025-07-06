#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

const int cBig = INT_MAX;

int Dijkstra(int amount, int start, int finish,
             const std::vector<std::vector<std::pair<int, int>>>& graph) {
  std::vector<int> distance(amount, cBig);
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      pq;
  distance[start] = 0;
  pq.push({0, start});

  while (!pq.empty()) {
    auto [current_dist, u] = pq.top();
    pq.pop();

    if (distance[u] < current_dist) {
      continue;
    }

    for (auto [v, weight] : graph[u]) {
      if (distance[v] - weight > distance[u]) {
        distance[v] = distance[u] + weight;
        pq.push({distance[v], v});
      }
    }
  }
  return distance[finish];
}

int main() {
  std::unordered_map<int, int> floor_index;
  int t;
  int in;
  int n;
  std::cin >> n;
  int u;
  std::cin >> u;
  int d;
  std::cin >> d;
  int i;
  std::cin >> i;
  int o;
  std::cin >> o;
  int k;
  std::cin >> k;
  std::vector<std::vector<int>> teleportators(k, std::vector<int>());
  std::vector<int> floors = {1, n};
  for (int j = 0; j != k; ++j) {
    std::cin >> t;
    for (int w = 0; w < t; ++w) {
      std::cin >> in;
      teleportators[j].push_back(in);
      floors.push_back(in);
    }
  }
  std::sort(floors.begin(), floors.end());
  floors.erase(std::unique(floors.begin(), floors.end()), floors.end());

  int amount = static_cast<int>(floors.size());
  int total_amount = k + amount;
  std::vector<std::vector<std::pair<int, int>>> graph(total_amount);
  for (int j = 0; j != amount; ++j) {
    floor_index[floors[j]] = j;
  }
  for (int j = 0; j != amount - 1; ++j) {
    graph[j].push_back({j + 1, (floors[j + 1] - floors[j]) * u});
    graph[j + 1].push_back({j, (floors[j + 1] - floors[j]) * d});
  }

  for (int teleport = 0; teleport != k; ++teleport) {
    for (int j = 0; j != (int)teleportators[teleport].size(); ++j) {
      graph[floor_index[teleportators[teleport][j]]].push_back(
          {teleport + amount, i});
      graph[teleport + amount].push_back(
          {floor_index[teleportators[teleport][j]], o});
    }
  }
  std::cout << Dijkstra(total_amount, floor_index[1], floor_index[n], graph);
}