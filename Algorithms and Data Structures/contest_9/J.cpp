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
    long long rev_index;
    long long id;
  };
  std::vector<std::vector<Edge>> graph_;
  std::vector<Edge*> original_edges_;

 public:
  Graph(long long n) : graph_(n) {}

  void AddEdge(long long u, long long v, long long c, long long i) {
    graph_[u].push_back({u, v, c, 0, (long long)graph_[v].size(), i});
    graph_[v].push_back({v, u, 0, 0, (long long)graph_[u].size() - 1, -1});
    original_edges_.push_back(&graph_[u].back());
  }
};

class Shell {
 private:
  Graph g_;
  long long n_;
  std::vector<long long> level_;
  std::vector<long long> ptr_;
  const long long cBig = LLONG_MAX;

 public:
  Shell(long long n) : g_(n), n_(n), level_(n, -1), ptr_(n) {}

  void AddEdge(long long u, long long v, long long c, long long idx) {
    g_.AddEdge(u, v, c, idx);
  }

  long long DFS(long long u, long long t, long long pushed, long long bound) {
    if (pushed == 0 || u == t) {
      return pushed;
    }
    long long tr = 0;
    for (long long& seed = ptr_[u];
         seed != static_cast<long long>(g_.graph_[u].size()); ++seed) {
      auto& edge = g_.graph_[u][seed];
      if (level_[edge.to] - 1 == level_[u] &&
          edge.capacity >= bound + edge.flow) {
        tr =
            DFS(edge.to, t, std::min(edge.capacity - edge.flow, pushed), bound);
        if (tr > 0) {
          edge.flow += tr;
          g_.graph_[edge.to][edge.rev_index].flow -= tr;
          return tr;
        }
      }
    }
    return 0;
  }

  bool BFS(long long start, long long t, long long bound) {
    level_.assign(n_, -1);
    std::queue<long long> q;
    q.push(start);
    level_[start] = 0;
    long long up = -1;
    while (!q.empty()) {
      up = q.front();
      q.pop();
      for (auto& edge : g_.graph_[up]) {
        if (level_[edge.to] == -1 && edge.capacity >= bound + edge.flow) {
          level_[edge.to] = level_[up] + 1;
          q.push(edge.to);
        }
      }
    }
    return (level_[t] != -1);
  }

  long long ScalingDinic(long long start, long long sink) {
    long long max_capacity = 0;
    long long flow = 0;
    for (auto& edges : g_.graph_) {
      for (auto& edge : edges) {
        max_capacity = std::max(edge.capacity, max_capacity);
      }
    }

    for (long long bound = cBig; bound > 0; bound /= 2) {
      while (BFS(start, sink, bound)) {
        ptr_.assign(n_, 0);
        while (long long pushed = DFS(start, sink, cBig, bound)) {
          flow += pushed;
        }
      }
    }
    return flow;
  }

  std::vector<long long> Flows(long long m) {
    std::vector<long long> flows(m, 0);
    for (long long u = 0; u != static_cast<long long>(g_.graph_.size()); ++u) {
      for (auto& edge : g_.graph_[u]) {
        if (m > edge.id && edge.id >= 0) {
          flows[edge.id] += edge.flow;
        }
      }
    }
    return flows;
  }
};

struct Match {
  long long one;
  long long two;
  long long how_much;
};

int main() {
  long long n;
  std::cin >> n;
  std::vector<long long> s(n);
  std::vector<long long> r(n);
  std::vector<std::vector<long long>> p(n, std::vector<long long>(n));
  for (long long i = 0; i != n; ++i) {
    std::cin >> s[i];
  }
  for (long long i = 0; i != n; ++i) {
    std::cin >> r[i];
  }
  long long max_score = s[0] + r[0];
  for (long long i = 0; i != n; ++i) {
    for (long long j = 0; j != n; ++j) {
      std::cin >> p[i][j];
    }
  }

  std::vector<Match> matches;
  for (long long i = 1; i != n; ++i) {
    for (long long j = 1 + i; j != n; ++j) {
      if (p[i][j] != 0) {
        matches.push_back({i, j, p[i][j]});
      }
    }
  }
  long long game_number = 0;
  long long size = (long long)matches.size() + n;
  Shell work(size + 1);
  long long games_amount = 0;
  for (long long i = 0; i != (long long)matches.size(); ++i) {
    work.AddEdge(size - 1, game_number, matches[i].how_much, i);
    work.AddEdge(game_number, ((long long)matches.size() + matches[i].one - 1),
                 matches[i].how_much, i);
    work.AddEdge(game_number, ((long long)matches.size() + matches[i].two - 1),
                 matches[i].how_much, i);
    games_amount += matches[i].how_much;
    ++game_number;
  }
  bool flag = true;
  for (long long i = 1; i != n; ++i) {
    if (s[i] > max_score) {
      std::cout << "NO" << '\n';
      flag = false;
    }
    work.AddEdge((long long)matches.size() + (i - 1), size, max_score - s[i],
                 i);
  }
  if (flag) {
    long long flow = work.ScalingDinic(size - 1, size);
    if (flow - games_amount != 0) {
      std::cout << "NO" << '\n';
    } else {
      std::cout << "YES" << '\n';
    }
  }
}