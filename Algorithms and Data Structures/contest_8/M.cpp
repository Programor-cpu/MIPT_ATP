#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

class Graph {
 private:
  friend class Shell;
  struct Edge {
    int from;
    int to;
    int weight;
  };
  std::vector<std::vector<Edge>> graph_;
  std::map<std::pair<int, int>, int> edges_;

 public:
  Graph(int n) : graph_(n) {}
  void AddEdge(int start, int finish, int weight) {
    graph_[start].push_back({start, finish, weight});
    if (edges_.contains({start, finish})) {
      if (weight < edges_[{start, finish}]) {
        edges_[{start, finish}] = weight;
      }
    } else {
      edges_[{start, finish}] = weight;
    }
  };
};

class Shell {
 private:
  Graph g_;
  std::vector<int> distances_;
  int n_;

 public:
  const int cBig = INT_MAX;
  Shell(int n) : g_(n), distances_(n, cBig), n_(n) {}
  void AddEdge(int start, int finish, int weight) {
    g_.AddEdge(start, finish, weight);
  };
  std::vector<int> FordBellman() {
    for (int i = 0; i != n_; ++i) {
      distances_[i] = cBig;
    }
    distances_[0] = 0;
    for (int w = 1; w != n_; ++w) {
      for (int i = 0; i != n_; ++i) {
        for (int j = 0; j != (int)g_.graph_[i].size(); ++j) {
          if (distances_[i] != cBig &&
              distances_[g_.graph_[i][j].to] >
                  distances_[i] + g_.graph_[i][j].weight) {
            distances_[g_.graph_[i][j].to] =
                distances_[g_.graph_[i][j].from] + g_.graph_[i][j].weight;
          }
        }
      }
    }
    return distances_;
  }
  std::vector<int> NegativeCycle() {
    for (int i = 0; i != n_; ++i) {
      distances_[i] = 0;
    }
    int flag = -1;
    std::vector<int> parent(n_, -1);
    std::vector<int> cycle;
    for (int w = 0; w != n_; ++w) {
      flag = -1;
      for (int i = 0; i != n_; ++i) {
        for (int j = 0; j != (int)g_.graph_[i].size(); ++j) {
          if (distances_[i] < cBig &&
              distances_[g_.graph_[i][j].to] >
                  distances_[i] + g_.graph_[i][j].weight) {
            distances_[g_.graph_[i][j].to] = std::max(
                distances_[g_.graph_[i][j].from] + g_.graph_[i][j].weight,
                -cBig);
            parent[g_.graph_[i][j].to] = g_.graph_[i][j].from;
            flag = g_.graph_[i][j].to;
          }
        }
      }
    }
    if (flag == -1) {
      return cycle;
    }

    for (int i = 0; i != n_; ++i) {
      flag = parent[flag];
    }
    for (int ver = flag;; ver = parent[ver]) {
      cycle.push_back(ver);
      if (ver == flag && 1 < (int)cycle.size()) {
        break;
      }
    }
    std::reverse(cycle.begin(), cycle.end());
    return cycle;
  }
  std::vector<int> Dijkstra(int start) {
    distances_.assign(n_, cBig);
    distances_[start] = 0;
    std::set<std::pair<int, int>> ds;
    ds.insert(std::make_pair(0, start));
    while (!ds.empty()) {
      std::pair<int, int> min = *(ds.begin());
      ds.erase(ds.begin());

      int u = min.second;
      for (int i = 0; i != (int)g_.graph_[u].size(); ++i) {
        int v = g_.graph_[u][i].to;
        int weight = g_.graph_[u][i].weight;
        if (distances_[u] < distances_[v] - weight) {
          if (distances_[v] != cBig) {
            ds.erase(ds.find(std::make_pair(distances_[v], v)));
          }

          distances_[v] = weight + distances_[u];
          ds.insert(std::make_pair(distances_[v], v));
        }
      }
    }
    return distances_;
  }
  int MstWeight() {
    std::vector<std::vector<std::pair<int, int>>> adj(n_);
    for (const auto& [uv, w] : g_.edges_) {
      int u = uv.first;
      int v = uv.second;
      adj[u].push_back({v, w});
      adj[v].push_back({u, w});
    }
    std::vector<int> key(n_, cBig);
    std::vector<bool> in_mst(n_, false);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        std::greater<std::pair<int, int>>>
        pq;
    pq.push({0, 0});
    key[0] = 0;

    int weight = 0;

    while (!pq.empty()) {
      int u = pq.top().second;
      pq.pop();
      if (in_mst[u]) {
        continue;
      }
      in_mst[u] = true;
      weight += key[u];
      for (const auto& [ver, weight] : adj[u]) {
        if (!in_mst[ver] && key[ver] > weight) {
          key[ver] = weight;
          pq.push({key[ver], ver});
        }
      }
    }

    return weight;
  }
};

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  Shell work(n);
  while (m != 0) {
    --m;
    int u;
    int v;
    int c;
    std::cin >> u >> v >> c;
    if (u == v) {
      continue;
    }

    work.AddEdge(u - 1, v - 1, c);
    work.AddEdge(v - 1, u - 1, c);
  }
  std::cout << work.MstWeight();
}