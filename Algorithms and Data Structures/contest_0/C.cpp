#include <cmath>
#include <iostream>
#include <vector>

const int cBig = 2147483647;

bool BinarySearch(int looking, std::vector<int>& arr, int n) {
  int f = 0;
  int l = n + 1;
  int m = 0;
  while (l > f + 1) {
    m = (l + f) / 2;
    if (arr[m] <= looking) {
      f = m;
    } else {
      l = m;
    }
  }
  return (arr[f] == looking);
}

int main() {
  int n;
  int k;
  std::cin >> n;
  std::cin >> k;
  std::vector<int> arr;
  int b;
  for (int j = 0; j < n; j++) {
    int q;
    std::cin >> q;
    arr.push_back(q);
  }
  arr.push_back(cBig);
  for (int j = 0; j < k; j++) {
    std::cin >> b;
    if (BinarySearch(b, arr, n)) {
      std::cout << "YES" << '\n';
    } else {
      std::cout << "NO" << '\n';
    }
  }
}