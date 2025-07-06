#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

std::vector<std::vector<int>> Calculation(std::vector<std::vector<int>>& matrix,
                                          int n, int k) {
  std::vector<std::vector<int>> first(n, std::vector<int>(n - k + 1));
  std::vector<std::vector<int>> second(n - k + 1, std::vector<int>(n - k + 1));
  for (int i = 0; i != n; i++) {
    std::vector<int> b(n);
    std::vector<int> c(n);
    for (int j = 0; j != n; j++) {
      if (j % k != 0) {
        b[j] = std::min(b[j - 1], matrix[i][j]);
      } else {
        b[j] = matrix[i][j];
      }
    }
    c[n - 1] = matrix[i][n - 1];
    for (int j = n - 2; j != -1; j--) {
      if ((j + 1) % k != 0) {
        c[j] = std::min(c[j + 1], matrix[i][j]);
      } else {
        c[j] = matrix[i][j];
      }
    }
    for (long long j = 0; j + k - 1 != n; j++) {
      first[i][j] = std::min(c[j], b[j + k - 1]);
    }
  }
  for (int i = 0; i != n - k + 1; i++) {
    std::vector<int> b(n);
    std::vector<int> c(n);
    for (int j = 0; j != n; j++) {
      if (j % k != 0) {
        b[j] = std::min(b[j - 1], first[j][i]);
      } else {
        b[j] = first[j][i];
      }
    }
    c[n - 1] = first[n - 1][i];
    for (int j = n - 2; j != -1; j--) {
      if ((j + 1) % k != 0) {
        c[j] = std::min(c[j + 1], first[j][i]);
      } else {
        c[j] = first[j][i];
      }
    }
    for (long long j = 0; j + k - 1 != n; j++) {
      second[j][i] = std::min(c[j], b[j + k - 1]);
    }
  }
  return second;
}

int main() {
  int n;
  int k;
  std::cin >> n >> k;
  std::vector<std::vector<int>> matrix(n, std::vector<int>(n));
  for (int i = 0; i != n; i++) {
    for (int j = 0; j != n; j++) {
      int m;
      std::cin >> m;
      matrix[i][j] = m;
    }
  }

  std::vector<std::vector<int>> result = Calculation(matrix, n, k);
  for (int i = 0; i != n - k + 1; i++) {
    for (int j = 0; j != n - k + 1; j++) {
      std::cout << result[i][j] << ' ';
    }
    std::cout << '\n';
  }
}