#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

const int cBig = INT_MAX;

class DSU {
 private:
  std::vector<int> ranks_;
  std::vector<int> parents_;

 public:
  DSU(int n) : ranks_(n, 0), parents_(n) {
    for (int i = 0; i != n; ++i) {
      parents_[i] = i;
    }
  }

  int Find(int ver) {
    while (ver != parents_[ver]) {
      ver = parents_[ver];
    }
    return ver;
  }
  void Unity(int ver_one, int ver_two) {
    ver_one = Find(ver_one);
    ver_two = Find(ver_two);
    if (ranks_[ver_one] < ranks_[ver_two]) {
      std::swap(ver_one, ver_two);
    }
    if (ver_two == ver_one) {
      return;
    }
    parents_[ver_two] = ver_one;
    if (ranks_[ver_one] == ranks_[ver_two]) {
      ranks_[ver_one] = ranks_[ver_one] + 1;
    }
  }
};

struct Edge {
  int from = 0;
  int to = 0;
  int weight = 0;
  bool operator>(const Edge& another) const {
    return (weight > another.weight);
  }
  bool operator<(const Edge& another) const {
    return (weight < another.weight);
  }
};

struct Request {
  int from;
  int to;
  int contain;
  int i;
  bool operator<(const Request& another) const {
    return (contain < another.contain);
  }
};

void Prim(const std::vector<bool>& stations,
          const std::vector<std::vector<std::pair<int, int>>>& graph, int start,
          std::vector<Edge>& answer, std::vector<int>& distance) {
  std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> pq;
  pq.push({start, start, 0});
  distance[start] = 0;
  while (!pq.empty()) {
    Edge min = pq.top();
    pq.pop();
    int ver = min.to;
    if (distance[ver] < min.weight) {
      continue;
    }
    int from = min.from;
    if (stations[ver] && ver != start) {
      answer.push_back({from, ver, min.weight});
      distance[ver] = 0;
      from = ver;
    }
    for (auto [to, weight] : graph[ver]) {
      if (distance[to] > distance[ver] + weight) {
        distance[to] = distance[ver] + weight;
        pq.push({from, to, distance[to]});
      }
    }
  }
}

int main() {
  int n;
  int k;
  int m;
  std::cin >> n >> k >> m;
  std::vector<bool> stations(n, false);
  for (int i = 0; i != k; ++i) {
    int in;
    std::cin >> in;
    stations[in - 1] = true;
  }
  std::vector<Edge> edges;
  std::vector<std::vector<std::pair<int, int>>> graph(n);
  for (int i = 0; i != m; ++i) {
    int u;
    int v;
    int w;
    std::cin >> u >> v >> w;
    edges.push_back({u - 1, v - 1, w});
    edges.push_back({v - 1, u - 1, w});
    graph[u - 1].push_back({v - 1, w});
    graph[v - 1].push_back({u - 1, w});
  }
  int q;
  std::cin >> q;
  std::vector<Request> requests;
  for (int i = 0; i != q; ++i) {
    int u;
    int v;
    int c;
    std::cin >> u >> v >> c;
    requests.push_back({u - 1, v - 1, c, i});
  }
  DSU vers(n);
  std::vector<bool> request_status(q, false);
  std::vector<Edge> result;
  std::vector<int> distance(n, cBig);
  for (int i = 0; i != n; ++i) {
    if (stations[i] && distance[i] == cBig) {
      Prim(stations, graph, i, result, distance);
    }
  }
  std::sort(result.begin(), result.end());
  std::sort(requests.begin(), requests.end());
  int index = 0;
  for (int i = 0; i != q; ++i) {
    int c = requests[i].contain;
    while (index < (int)result.size() && result[index].weight <= c) {
      vers.Unity(result[index].to, result[index].from);
      ++index;
    }
    if (vers.Find(requests[i].from) == vers.Find(requests[i].to)) {
      request_status[requests[i].i] = true;
    }
  }
  for (int i = 0; i != q; ++i) {
    if (request_status[i]) {
      std::cout << "YES" << '\n';
      continue;
    }
    std::cout << "NO" << '\n';
  }
}