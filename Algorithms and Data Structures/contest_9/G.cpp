#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

const int cBig = 2501;

const int cDx[] = {1, -1, 0, 0};
const int cDy[] = {0, 0, 1, -1};

struct Edge {
  int to;
  int flow;
  int rev;
};

bool DFS(std::vector<std::vector<Edge>>& graph,
         std::vector<std::pair<int, Edge*>>& parent, int n) {
  std::queue<int> query;
  query.push(0);
  std::vector<bool> visited(n, false);
  visited[0] = true;
  --parent[0].first;

  while (!query.empty()) {
    int u = query.front();
    query.pop();

    for (Edge& e : graph[u]) {
      int ver = e.to;
      if (!visited[ver] && e.flow > 0) {
        query.push(ver);
        visited[ver] = !visited[ver];
        parent[ver].first = u;
        parent[ver].second = &e;
        if (ver - n + 1 == 0) {
          return true;
        }
      }
    }
  }
  return false;
}

std::vector<bool> Backtrack(std::vector<std::vector<Edge>>& graph, int n) {
  std::queue<int> query;
  query.push(0);
  std::vector<bool> visited(n, false);
  visited[0] = true;

  while (!query.empty()) {
    int u = query.front();
    query.pop();

    for (Edge& e : graph[u]) {
      int ver = e.to;
      if (!visited[ver] && e.flow > 0) {
        query.push(ver);
        visited[ver] = !visited[ver];
      }
    }
  }
  return visited;
}

int FordFulkerson(std::vector<std::vector<Edge>>& graph, int n) {
  std::vector<std::pair<int, Edge*>> parent(n, {0, nullptr});
  int max_flow = 0;
  while (DFS(graph, parent, n)) {
    int path = cBig;

    for (int ver = n - 1; ver != 0; ver = parent[ver].first) {
      path = std::min(path, parent[ver].second->flow);
    }

    for (int ver = n - 1; ver != 0; ver = parent[ver].first) {
      int rev = parent[ver].second->rev;
      parent[ver].second->flow -= path;
      graph[ver][rev].flow += path;
    }

    max_flow += path;
  }

  return max_flow;
}

std::pair<int, int> SplitCoord(int x, int y, int w) {
  int n = 2 * (x * w + y) + 1;
  return {n, n + 1};
}

void AddEdge(std::vector<std::vector<Edge>>& graph, int x, int y, int cap,
             int cap_two = 0) {
  graph[x].push_back({y, cap, (int)graph[y].size()});
  graph[y].push_back({x, cap_two, (int)graph[x].size() - 1});
}

int main() {
  int h;
  int w;
  std::cin >> h >> w;
  int b;
  int p;
  int x;
  int y;
  std::vector<std::vector<int>> matrix(h, std::vector<int>(w, 2));
  std::cin >> b >> p;
  for (int i = 0; i != b; ++i) {
    std::cin >> x >> y;
    matrix[--x][--y] = 0;
  }
  for (int i = 0; i != p; ++i) {
    std::cin >> x >> y;
    matrix[--x][--y] = 1;
  }
  int xs;
  int ys;
  int xf;
  int yf;
  std::cin >> xs >> ys >> xf >> yf;
  --xs;
  --ys;
  --xf;
  --yf;
  int n = 2 * h * w + 2;
  std::vector<std::vector<Edge>> graph(n);

  AddEdge(graph, 0, SplitCoord(xs, ys, w).first, cBig);
  AddEdge(graph, SplitCoord(xf, yf, w).second, n - 1, cBig);
  for (int i = 0; i != h; ++i) {
    for (int j = 0; j != w; ++j) {
      int x = SplitCoord(i, j, w).first;
      int y = SplitCoord(i, j, w).second;
      if (matrix[i][j] == 2) {
        AddEdge(graph, x, y, cBig);
      } else if (matrix[i][j] == 1) {
        AddEdge(graph, x, y, 1);
      }
      for (int k = 0; k != 4; ++k) {
        int i_d = i + cDx[k];
        int j_d = j + cDy[k];
        if (i_d < 0 || j_d < 0 || i_d >= h || j_d >= w) {
          continue;
        }
        int x_d = SplitCoord(i_d, j_d, w).first;
        AddEdge(graph, y, x_d, cBig);
      }
    }
  }
  int flow = FordFulkerson(graph, n);
  if (flow == cBig) {
    std::cout << "-1" << '\n';
    return 0;
  }
  std::cout << flow << '\n';
  std::vector<bool> visited = Backtrack(graph, n);
  for (int i = 0; i != h; ++i) {
    for (int j = 0; j != w; ++j) {
      if (matrix[i][j] == 1) {
        int x = SplitCoord(i, j, w).first;
        int y = SplitCoord(i, j, w).second;
        for (Edge& e : graph[x]) {
          if (e.to == y && visited[x] && !visited[y] && e.flow == 0) {
            std::cout << i + 1 << ' ' << j + 1 << '\n';
          }
        }
      }
    }
  }
}