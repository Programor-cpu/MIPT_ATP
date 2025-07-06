#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

class Graph {
 private:
  friend class Shell;
  std::vector<std::vector<std::pair<int, int>>> graph_;

 public:
  Graph(int n) : graph_(n) {}
  void AddEdge(int one, int two, int value) {
    graph_[one].push_back({two, value});
    graph_[two].push_back({one, value});
  };
};

class Shell {
 private:
  Graph orig_;
  std::vector<int> distances_;
  int n_;
  struct Vertex {
   public:
    int minus_x = 0;
    int plus_x = 0;
    int natural_minus = 0;
    int natural_plus = 0;
  };
  bool static BruteForce(int to_be_added, std::vector<Vertex>& answer_vertexes,
                         int n) {
    std::vector<int> trial(n);
    for (int i = 0; i != n; ++i) {
      if (answer_vertexes[i].plus_x >= 1) {
        trial[i] = to_be_added + answer_vertexes[i].natural_plus;
      } else {
        trial[i] = answer_vertexes[i].natural_minus - to_be_added;
      }
    }
    std::sort(trial.begin(), trial.end());
    for (int i = 0; i != n; ++i) {
      if (i + 1 != trial[i]) {
        return false;
      }
    }
    return true;
  }

  void DFS(int ver, std::vector<bool>& used,
           std::vector<Vertex>& answer_vertexes) {
    used[ver] = true;
    for (int i = 0; i != (int)orig_.graph_[ver].size(); ++i) {
      int to = orig_.graph_[ver][i].first;
      if (!used[to]) {
        if (answer_vertexes[ver].plus_x >= 1) {
          ++answer_vertexes[to].minus_x;
          answer_vertexes[to].natural_minus =
              orig_.graph_[ver][i].second - answer_vertexes[ver].natural_plus;
          DFS(to, used, answer_vertexes);
          continue;
        }
        ++answer_vertexes[to].plus_x;
        answer_vertexes[to].natural_plus =
            orig_.graph_[ver][i].second - answer_vertexes[ver].natural_minus;
        DFS(to, used, answer_vertexes);
      }
    }
  }

 public:
  const int cBig = INT_MAX;
  Shell(int n) : orig_(n), distances_(n, cBig), n_(n) {}
  void AddEdge(int start, int finish, int value) {
    orig_.AddEdge(start, finish, value);
  };
  void SolutionCalc() {
    int to_be_added = 0;
    bool uncount_cycle = false;
    int min_value = 0;
    int max_value = n_;
    std::vector<bool> used(n_, false);
    std::vector<Vertex> answer_vertexes(n_);
    std::vector<int> signum(n_, 0);
    ++answer_vertexes[0].plus_x;
    DFS(0, used, answer_vertexes);
    for (int i = 0; i != n_; ++i) {
      if (answer_vertexes[i].plus_x + answer_vertexes[i].minus_x >= 2) {
        to_be_added = (answer_vertexes[i].natural_minus -
                       answer_vertexes[i].natural_plus) /
                      2;
        uncount_cycle = true;
        break;
      }
      if (answer_vertexes[i].plus_x >= 1) {
        max_value = std::min(n_ - answer_vertexes[i].natural_plus, max_value);
        min_value = std::max(1 - answer_vertexes[i].natural_plus, min_value);
      } else {
        max_value = std::min(answer_vertexes[i].natural_minus - 1, max_value);
        min_value = std::max(answer_vertexes[i].natural_minus - n_, min_value);
      }
    }
    if (!uncount_cycle) {
      if (BruteForce(max_value, answer_vertexes, n_)) {
        to_be_added = max_value;
      } else {
        to_be_added = min_value;
      }
    }
    for (int i = 0; i != n_; ++i) {
      if (answer_vertexes[i].plus_x >= 1) {
        std::cout << to_be_added + answer_vertexes[i].natural_plus << ' ';
      } else {
        std::cout << answer_vertexes[i].natural_minus - to_be_added << ' ';
      }
    }
  }
};

int main() {
  int n;
  int m;
  int u;
  int v;
  int s;
  std::cin >> n >> m;
  Shell work(n);
  while (m != 0) {
    --m;
    std::cin >> u >> v >> s;
    work.AddEdge(u - 1, v - 1, s);
  }
  work.SolutionCalc();
}