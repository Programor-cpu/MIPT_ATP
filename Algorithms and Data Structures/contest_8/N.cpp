#include <algorithm>
#include <iostream>
#include <vector>

const int cBig = 2147483647;

int main() {
  int n;
  std::cin >> n;

  std::vector<std::vector<int>> matrix(n, std::vector<int>(n));
  std::vector<int> s(n);
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != n; ++j) {
      std::cin >> matrix[i][j];
    }
  }
  for (int i = 0; i != n; ++i) {
    std::cin >> s[i];
  }
  int cost = 0;

  std::vector<int> edges(n, cBig);

  edges[min_element(s.begin(), s.end()) - s.begin()] =
      s[min_element(s.begin(), s.end()) - s.begin()];
  std::vector<bool> used(n, false);

  for (int i = 0; i != n; ++i) {
    int ver = -cBig;
    for (int j = 0; j != n; ++j) {
      if (!used[j] && (ver == -cBig || edges[j] < edges[ver])) {
        ver = j;
      }
    }
    cost += edges[ver];
    used[ver] = true;

    for (int j = 0; j != n; ++j) {
      if (!used[j]) {
        if (edges[j] > matrix[ver][j]) {
          edges[j] = matrix[ver][j];
        }
        if (edges[j] > s[j]) {
          edges[j] = s[j];
        }
      }
    }
  }
  std::cout << cost;
}