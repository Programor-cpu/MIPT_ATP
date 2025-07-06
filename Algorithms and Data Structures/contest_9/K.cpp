#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>
class Graph {
 private:
  friend class Shell;
  struct Edge {
    long long from;
    long long to;
    long long capacity;
    long long flow;
    long long cost;
    long long rev_index;
  };
  std::vector<std::vector<Edge>> graph_;

 public:
  Graph(long long n) : graph_(n) {}
  void AddEdge(long long u, long long v, long long cap, long long cost) {
    graph_[u].push_back({u, v, cap, 0, cost, (long long)graph_[v].size()});
    graph_[v].push_back({v, u, 0, 0, -cost, (long long)graph_[u].size() - 1});
  }
};
class Shell {
 private:
  Graph g_;
  long long n_;
  std::vector<long long> distance_;
  std::vector<long long> pi_;
  std::vector<long long> prev_ver_;
  std::vector<long long> prev_edge_;
  const long long cBig = static_cast<long long>(1e15);

 public:
  Shell(long long n)
      : g_(n),
        n_(n),
        distance_(n, cBig),
        pi_(n, 0),
        prev_ver_(n),
        prev_edge_(n) {}

  void AddEdge(long long u, long long v, long long cap, long long cost) {
    g_.AddEdge(u - 1, v - 1, cap, cost);
  }

  long long MinCostMaxFlow(long long source, long long sink) {
    long long cost = 0;
    while (true) {
      distance_.assign(n_, cBig);
      distance_[source] = 0;
      std::priority_queue<std::pair<long long, long long>,
                          std::vector<std::pair<long long, long long>>,
                          std::greater<>>
          dijkstra_queue;
      dijkstra_queue.emplace(0, source);

      while (!dijkstra_queue.empty()) {
        long long dist = dijkstra_queue.top().first;
        long long u = dijkstra_queue.top().second;
        dijkstra_queue.pop();
        if (dist == distance_[u]) {
          for (long long i = 0; i != (long long)g_.graph_[u].size(); ++i) {
            if (g_.graph_[u][i].capacity > g_.graph_[u][i].flow) {
              long long cost_through = distance_[u] + g_.graph_[u][i].cost +
                                       (pi_[u] - pi_[g_.graph_[u][i].to]);
              if (cost_through < distance_[g_.graph_[u][i].to]) {
                prev_ver_[g_.graph_[u][i].to] = u;
                prev_edge_[g_.graph_[u][i].to] = i;
                distance_[g_.graph_[u][i].to] = cost_through;
                dijkstra_queue.emplace(distance_[g_.graph_[u][i].to],
                                       g_.graph_[u][i].to);
              }
            }
          }
        }
      }

      if (distance_[sink] == cBig) {
        break;
      }
      long long pushed = cBig;
      for (long long ver = 0; ver != n_; ++ver) {
        if (cBig > distance_[ver]) {
          pi_[ver] += distance_[ver];
        }
      }

      for (long long ver = sink; ver != source; ver = prev_ver_[ver]) {
        pushed = std::min(g_.graph_[prev_ver_[ver]][prev_edge_[ver]].capacity -
                              g_.graph_[prev_ver_[ver]][prev_edge_[ver]].flow,
                          pushed);
      }

      for (long long ver = sink; ver != source; ver = prev_ver_[ver]) {
        cost += g_.graph_[prev_ver_[ver]][prev_edge_[ver]].cost * pushed;
        g_.graph_[prev_ver_[ver]][prev_edge_[ver]].flow += pushed;
        g_.graph_[ver][g_.graph_[prev_ver_[ver]][prev_edge_[ver]].rev_index]
            .flow -= pushed;
      }
    }

    return cost;
  }
};

int main() {
  long long n;
  long long m;
  std::cin >> n >> m;
  Shell work(n);
  long long u;
  long long v;
  long long c;
  long long w;
  for (long long i = 0; i != m; ++i) {
    std::cin >> u >> v >> c >> w;
    work.AddEdge(u, v, c, w);
  }
  std::cout << work.MinCostMaxFlow(0, n - 1) << '\n';
}