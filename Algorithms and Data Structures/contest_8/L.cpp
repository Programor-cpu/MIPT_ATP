#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

class DSU {
 private:
  std::vector<int> ranks_;
  std::vector<int> parents_;
  std::stack<std::pair<int, int>> story_;

 public:
  DSU(int n) : ranks_(n, 0), parents_(n) {
    for (int i = 1; i != n; ++i) {
      parents_[i] = i;
    }
  }

  int Find(int ver) {
    while (ver != parents_[ver]) {
      ver = parents_[ver];
    }
    return ver;
  }
  void GetBack(int s) {
    while (s != 0) {
      auto [ver, previous] = story_.top();
      story_.pop();
      if (0 <= ver) {
        parents_[ver] = previous;

      } else {
        --ranks_[-ver];
      }
      --s;
    }
  }
  void Unity(int ver_one, int ver_two) {
    ver_one = Find(ver_one);
    ver_two = Find(ver_two);
    if (ranks_[ver_one] - ranks_[ver_two] < 0) {
      std::swap(ver_one, ver_two);
    }
    if (ver_two - ver_one == 0) {
      return;
    }

    story_.push({ver_two, parents_[ver_two]});
    parents_[ver_two] = ver_one;
    if (ranks_[ver_one] - ranks_[ver_two] == 0) {
      story_.push({-ver_one, -1});
      ranks_[ver_one] = ranks_[ver_one] + 1;
    }
  }
};

struct Request {
  bool type = false;
  int u = 0;
  int v = 0;
};

int main() {
  std::map<std::pair<int, int>, int> edges;
  int n;
  int m;
  std::cin >> n >> m;
  DSU work(n + 1);
  int q;
  std::cin >> q;
  std::vector<Request> requests(q);
  std::vector<std::pair<int, int>> graph(m);
  for (int i = 0; i != m; ++i) {
    int u;
    int v;
    std::cin >> u >> v;
    if (u - v > 0) {
      int reserve = u;
      u = v;
      v = reserve;
    }
    ++edges[{u, v}];
    graph[i] = {u, v};
  }
  for (int i = 0; i != q; ++i) {
    std::string in;
    std::cin >> in;
    std::cin >> requests[i].u >> requests[i].v;
    if (requests[i].u - requests[i].v > 0) {
      int reserve = requests[i].u;
      requests[i].u = requests[i].v;
      requests[i].v = reserve;
    }
    if (in == "ask") {
      requests[i].type = true;
    } else {
      --edges[{requests[i].u, requests[i].v}];
      requests[i].type = false;
    }
  }
  for (auto [edge, c] : edges) {
    if (0 < c) {
      work.Unity(edge.first, edge.second);
    }
  }
  std::vector<bool> answers;
  for (int i = q - 1; i != -1; --i) {
    if (requests[i].type) {
      if (work.Find(requests[i].v) == work.Find(requests[i].u)) {
        answers.push_back(true);
      } else {
        answers.push_back(false);
      }
    } else {
      work.Unity(requests[i].u, requests[i].v);
    }
  }
  for (int i = (int)answers.size() - 1; i != -1; --i) {
    if (answers[i]) {
      std::cout << "YES" << '\n';
    } else {
      std::cout << "NO" << '\n';
    }
  }
}