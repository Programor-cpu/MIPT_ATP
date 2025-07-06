#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

int main() {
  int n;
  std::cin >> n;
  std::vector<std::vector<int>> matrix(n, std::vector<int>(n));
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != n; ++j) {
      std::cin >> matrix[i][j];
    }
  }
  for (int k = 0; k != n; ++k) {
    for (int i = 0; i != n; ++i) {
      for (int j = 0; j != n; ++j) {
        if (matrix[i][j] == 1 || (matrix[i][k] == 1 && matrix[k][j] == 1)) {
          matrix[i][j] = 1;
        } else {
          matrix[i][j] = 0;
        }
      }
    }
  }
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != n; ++j) {
      std::cout << matrix[i][j] << ' ';
    }
    std::cout << "\n";
  }
}