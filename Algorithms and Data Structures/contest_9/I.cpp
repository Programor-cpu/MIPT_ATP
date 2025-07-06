#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

const int cNeed = 2;
const float cBig = 1e9;
const float cSmall = 1e-9;
const float cHalf = 0.5;

struct Edge {
  int to;
  int rev;
  float flow;
  float capacity;
};

struct Shell {
  int n;
  std::vector<int> ptr;
  std::vector<std::vector<Edge>> graph;
  std::vector<int> level;

  Shell(int given) : n(given + cNeed), graph(n), level(n, -1) {}

  void AddEdge(int u, int v, float capacity) {
    graph[u].push_back({v, static_cast<int>(graph[v].size()), 0, capacity});
    graph[v].push_back({u, static_cast<int>(graph[u].size()) - 1, 0, 0});
  }

  std::pair<float, bool> DFS(int u, int t, float flow) {
    if (u - t == 0) {
      return {flow, true};
    }

    for (int& i = ptr[u]; i < static_cast<int>(graph[u].size()); ++i) {
      Edge& edge = graph[u][i];
      if (level[edge.to] - level[u] == 1 &&
          edge.capacity - edge.flow > cSmall) {
        auto [pushed, ok] =
            DFS(edge.to, t, std::min(flow, edge.capacity - edge.flow));
        if (ok && pushed > cSmall) {
          edge.flow += pushed;
          graph[edge.to][edge.rev].flow -= pushed;
          return {pushed, true};
        }
      }
    }
    return {0, false};
  }

  bool BFS(int s, int t) {
    level.assign(n, -1);
    std::queue<int> bfs_queue;
    level[s] = 0;
    bfs_queue.push(s);

    while (!bfs_queue.empty()) {
      int u = bfs_queue.front();
      bfs_queue.pop();

      for (const Edge& e : graph[u]) {
        if (level[e.to] == -1 && e.capacity - e.flow > cSmall) {
          level[e.to] = level[u] + 1;
          bfs_queue.push(e.to);
        }
      }
    }
    return level[t] + 1 != 0;
  }

  std::vector<bool> Reachable(int s) {
    std::vector<bool> visited(n, false);
    std::queue<int> ququ;
    ququ.push(s);
    visited[s] = true;

    while (!ququ.empty()) {
      int upper = ququ.front();
      ququ.pop();

      for (Edge& edge : graph[upper]) {
        if (!visited[edge.to] && edge.capacity - edge.flow > cSmall) {
          visited[edge.to] = true;
          ququ.push(edge.to);
        }
      }
    }
    return visited;
  }

  float MF(int s, int t) {
    float total = 0;
    while (BFS(s, t)) {
      ptr.assign(n, 0);
      while (true) {
        auto [pushed, ok] = DFS(s, t, cBig);
        if (!ok) {
          break;
        }
        total += pushed;
      }
    }
    return total;
  }
};

struct Sundowner {
  int n;
  int m;
  std::vector<int> u;
  std::vector<int> v;
  std::vector<int> vodka;

 public:
  Sundowner(int num, int mum)
      : n(num), m(mum), u(m + 1), v(m + 1), vodka(n + 1) {}

  std::vector<int> Invincible() {
    float lower = 0;
    float upper = m;
    float precision = 1.0 / (n * (n - 1));
    std::vector<int> result;

    while (upper - lower >= precision) {
      float mid = (lower + upper) * cHalf;
      std::vector<int> current;
      if (RedSun(mid, current)) {
        lower = mid;
        result = std::move(current);
      } else {
        upper = mid;
      }
    }

    std::sort(result.begin(), result.end());
    return result;
  }

  bool RedSun(float g, std::vector<int>& result) {
    int sink = n + 2;
    Shell solver(sink);

    for (int i = 1; i != m + 1; ++i) {
      solver.AddEdge(u[i], v[i], 1);
      solver.AddEdge(v[i], u[i], 1);
    }

    for (int i = 1; i != n + 1; ++i) {
      solver.AddEdge(n + 1, i, m);
      solver.AddEdge(i, sink, m + 2 * g - vodka[i]);
    }

    solver.MF(n + 1, sink);
    std::vector<bool> reachable = solver.Reachable(n + 1);

    result.clear();
    for (int i = 1; i != n + 1; ++i) {
      if (reachable[i]) {
        result.push_back(i);
      }
    }

    return !result.empty();
  }
};

int main() {
  int n = 0;
  int m = 0;
  std::cin >> n >> m;

  if (m == 0) {
    std::cout << "1\n1\n";
    return 0;
  }

  Sundowner finder(n, m);
  for (int i = 1; i != m + 1; ++i) {
    std::cin >> finder.u[i] >> finder.v[i];
    ++finder.vodka[finder.u[i]];
    ++finder.vodka[finder.v[i]];
  }

  std::vector<int> vers = finder.Invincible();

  std::cout << vers.size() << '\n';
  for (int i = 0; i != static_cast<int>(vers.size()); ++i) {
    std::cout << vers[i] << '\n';
  }
}