#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

const int cBig = INT_MAX;

void SolveAssignmentProblem(const std::vector<std::vector<int>>& cost_matrix,
                            int n) {
  std::vector<int> worker_potential(n + 1, 0);
  std::vector<int> task_potential(n + 1, 0);
  std::vector<int> matching(n + 1, 0);
  std::vector<int> path(n + 1, 0);

  for (int worker = 1; worker != n + 1; ++worker) {
    matching[0] = worker;
    std::vector<int> min_slack(n + 1, cBig);
    std::vector<bool> visited(n + 1, false);
    int current_task = 0;

    do {
      int next_task = -1;

      int assigned_worker = matching[current_task];
      int delta = cBig;
      visited[current_task] = true;

      for (int task = 1; task != n + 1; ++task) {
        if (!visited[task]) {
          int reduced_cost = cost_matrix[assigned_worker - 1][task - 1] -
                             task_potential[task] -
                             worker_potential[assigned_worker];

          if (reduced_cost - min_slack[task] < 0) {
            min_slack[task] = reduced_cost;
            path[task] = current_task;
          }
          if (min_slack[task] - delta < 0) {
            delta = min_slack[task];
            next_task = task;
          }
        }
      }

      for (int t = 0; t != n + 1; ++t) {
        if (!visited[t]) {
          min_slack[t] = min_slack[t] - delta;

        } else {
          task_potential[t] = task_potential[t] - delta;
          worker_potential[matching[t]] += delta;
        }
      }

      current_task = next_task;

    } while (matching[current_task] != 0);

    do {
      int previous = path[current_task];
      matching[current_task] = matching[previous];
      current_task = previous;
    } while (current_task != 0);
  }

  std::vector<int> assigned_task(n);
  for (int task = 1; task != n + 1; ++task) {
    assigned_task[matching[task] - 1] = task - 1;
  }
  std::cout << task_potential[0] * (-1) << '\n';
  for (int i = 0; i != n; ++i) {
    std::cout << i + 1 << " " << ++assigned_task[i] << '\n';
  }
}

int main() {
  int n;
  std::cin >> n;

  std::vector<std::vector<int>> cost_matrix(n, std::vector<int>(n));
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != n; ++j) {
      std::cin >> cost_matrix[i][j];
    }
  }

  SolveAssignmentProblem(cost_matrix, n);
}
