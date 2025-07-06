#include <algorithm>
#include <iostream>
#include <vector>

int Amount(std::vector<int>& arr, int l, int n, int k) {
  int current = 1;
  int now = arr[0];
  for (int i = 0; i < n; i++) {
    if ((arr[i] - now) > l) {
      current += 1;
      now = arr[i];
      if (current > k) {
        return 0;
      }
    }
  }
  return 1;
}

int BinarySearch(std::vector<int>& arr, int n, int k) {
  int f = -1;
  int l = arr[n - 1] - arr[0] + 1;
  int m = 0;
  int t = k;
  while (l > f + 1) {
    m = f + (l - f) / 2;
    if (Amount(arr, m, n, t) == 1) {
      l = m;
    } else {
      f = m;
    }
  }
  return l;
}

int main() {
  int n;
  int k;
  std::cin >> n;
  std::cin >> k;
  std::vector<int> arr;
  for (int i = 0; i < n; i++) {
    int q;
    std::cin >> q;
    arr.push_back(q);
  }
  std::sort(arr.begin(), arr.end());
  std::cout << BinarySearch(arr, n, k);
}