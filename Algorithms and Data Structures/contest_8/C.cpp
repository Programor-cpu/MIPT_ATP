#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

const int cInf = INT_MAX;
const int cAbort = -1;

class Graph {
 private:
  struct Edge {
    int to;
    int weight;
    int time;
  };
  std::vector<std::vector<Edge>> graph_;

 public:
  Graph(int n) : graph_(n) {}
  void AddEdge(int from, int to, int weight, int time) {
    graph_[from].push_back({to, weight, time});
  }
  const std::vector<Edge>& GetEdges(int from) const { return graph_[from]; }
  int Size() const { return graph_.size(); }
};

class Shell {
 private:
  Graph g_;
  std::vector<int> distances_;

 public:
  Shell(int n) : g_(n), distances_(n, cInf) {}
  void AddEdge(int from, int to, int weight, int time) {
    g_.AddEdge(from, to, weight, time);
  }
  std::vector<int> FordBellman() {
    std::fill(distances_.begin(), distances_.end(), cInf);
    distances_[0] = 0;
    for (int i = 0; i < g_.Size() - 1; ++i) {
      for (int u = 0; u < g_.Size(); ++u) {
        for (const auto& edge : g_.GetEdges(u)) {
          if (distances_[u] != cInf &&
              distances_[edge.to] > distances_[u] + edge.weight) {
            distances_[edge.to] = distances_[u] + edge.weight;
          }
        }
      }
    }
    return distances_;
  }
  std::vector<int> NegativeCycle() {
    std::fill(distances_.begin(), distances_.end(), 0);
    std::vector<int> parent(g_.Size(), -1);
    int flag = -1;
    for (int i = 0; i < g_.Size(); ++i) {
      flag = -1;
      for (int u = 0; u < g_.Size(); ++u) {
        for (const auto& edge : g_.GetEdges(u)) {
          if (distances_[u] != cInf &&
              distances_[edge.to] > distances_[u] + edge.weight) {
            distances_[edge.to] = std::max(distances_[u] + edge.weight, -cInf);
            parent[edge.to] = u;
            flag = edge.to;
          }
        }
      }
    }
    if (flag == -1) {
      return {};
    }
    for (int i = 0; i < g_.Size(); ++i) {
      flag = parent[flag];
    }
    std::vector<int> cycle;
    for (int v = flag;; v = parent[v]) {
      cycle.push_back(v);
      if (v == flag && cycle.size() > 1) {
        break;
      }
    }
    std::reverse(cycle.begin(), cycle.end());
    return cycle;
  }
  void Dijkstra(int s, std::vector<std::vector<int>>& dp,
                std::vector<std::vector<std::pair<int, int>>>& dp_save) {
    std::priority_queue<std::pair<int, std::pair<int, int>>,
                        std::vector<std::pair<int, std::pair<int, int>>>,
                        std::greater<>>
        pq;
    dp[0][0] = 0;
    pq.push({0, {0, 0}});
    while (!pq.empty()) {
      auto [cost, p] = pq.top();
      int u = p.second;
      int current_time = p.first;
      pq.pop();
      if (cost != dp[u][current_time]) {
        continue;
      }
      for (const auto& edge : g_.GetEdges(u)) {
        int time = current_time + edge.time;
        if (s >= time &&
            dp[edge.to][time] - edge.weight > dp[u][current_time]) {
          dp[edge.to][time] = dp[u][current_time] + edge.weight;
          dp_save[edge.to][time] = {u, current_time};
          pq.push({dp[edge.to][time], {time, edge.to}});
        }
      }
    }
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  int n;
  int m;
  int s;
  std::cin >> n >> m >> s;
  Shell work(n);
  while (m != 0) {
    int v;
    int u;
    int w;
    int t;
    std::cin >> v >> u >> w >> t;
    if (v == u || t > s) {
      continue;
    }
    work.AddEdge(v - 1, u - 1, w, t);
    if (v - 1 != 0 && u != n) {
      work.AddEdge(u - 1, v - 1, w, t);
    }
    --m;
  }
  std::vector<std::vector<int>> dp(n, std::vector<int>(s + 1, cInf));
  std::vector<std::vector<std::pair<int, int>>> dp_save(
      n, std::vector<std::pair<int, int>>(s + 1, {cAbort, cAbort}));
  work.Dijkstra(s, dp, dp_save);

  int min_cost = cInf;
  int best_time = cAbort;
  for (int i = 0; i <= s; ++i) {
    if (dp[n - 1][i] < min_cost) {
      min_cost = dp[n - 1][i];
      best_time = i;
    }
  }

  if (best_time != cAbort) {
    std::cout << min_cost << '\n';
    std::vector<int> sequence;
    int current_node = n - 1;
    int current_time = best_time;
    while (current_time >= 0) {
      sequence.push_back(current_node + 1);
      int prev_node = dp_save[current_node][current_time].first;
      int prev_time = dp_save[current_node][current_time].second;
      current_node = prev_node;
      current_time = prev_time;
    }
    std::cout << sequence.size() << '\n';
    for (auto it = sequence.rbegin(); it != sequence.rend(); ++it) {
      std::cout << *it << ' ';
    }
  } else {
    std::cout << "-1";
  }
}