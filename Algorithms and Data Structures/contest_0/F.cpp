#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cNum = 10;

int BinarySearchForK(const std::vector<std::vector<int>>& arr1,
                     const std::vector<std::vector<int>>& arr2, int i, int j,
                     int length) {
  int minmax = pow(cNum, 5) - 1;
  int first = 0;
  int last = length;
  int m = first + (last - first) / 2;
  int k = first + (last - first) / 2;
  while (last > first + 1) {
    if (arr1[i][m] - arr2[j][m] < 0) {
      if (std::max(arr1[i][m], arr2[j][m]) - minmax < 0) {
        k = m;
        minmax = std::max(arr1[i][k], arr2[j][k]);
      }
      first = m;
    } else if (arr1[i][m] - arr2[j][m] > 0) {
      if (std::max(arr1[i][m], arr2[j][m]) - minmax < 0) {
        k = m;
        minmax = std::max(arr1[i][k], arr2[j][k]);
      }
      last = m;
    } else {
      break;
    }
    m = first + (last - first) / 2;
  }
  if (minmax - std::max(arr1[i][m], arr2[j][m]) >= 0) {
    return m + 1;
  }
  return k + 1;
}

int main() {
  int n;
  int m;
  int l;
  std::cin >> n;
  std::cin >> m;
  std::cin >> l;
  std::vector<std::vector<int>> matrixa(n, std::vector<int>(l));
  std::vector<std::vector<int>> matrixb(m, std::vector<int>(l));
  for (int i = 0; i != n; i++) {
    for (int j = 0; j != l; j++) {
      int ingoing;
      std::cin >> ingoing;
      matrixa[i][j] = ingoing;
    }
  }
  for (int i = 0; i != m; i++) {
    for (int j = 0; j != l; j++) {
      int ingoing;
      std::cin >> ingoing;
      matrixb[i][j] = ingoing;
    }
  }
  int q;
  int i;
  int j;
  std::cin >> q;
  for (int w = 0; w != q; w++) {
    std::cin >> i;
    std::cin >> j;
    std::cout << BinarySearchForK(matrixa, matrixb, i - 1, j - 1, l) << '\n';
  }
}