#include <iostream>
#include <vector>
int main() {
  int n;
  std::cin >> n;
  std::vector<int> arr;
  arr.push_back(0);
  for (int i = 0; i < n; i++) {
    int t;
    std::cin >> t;
    arr.push_back(t);
  }
  int q;
  std::vector<int> pr(n + 1);
  std::vector<int> su(n + 1);
  for (int j = 1; j < n + 1; j++) {
    if (j == 1) {
      pr[1] = arr[1];
    } else {
      if (arr[j] < pr[j - 1]) {
        pr[j] = arr[j];
      } else {
        pr[j] = pr[j - 1];
      }
    }
  }
  for (int j = n; j > 0; j--) {
    if (j == n) {
      su[n] = arr[n];
    } else {
      if (arr[j] < su[j + 1]) {
        su[j] = arr[j];
      } else {
        su[j] = su[j + 1];
      }
    }
  }
  std::cin >> q;
  for (int i = 0; i < q; i++) {
    int m;
    int l;
    int r;
    std::cin >> l;
    std::cin >> r;
    if (pr[l] < su[r]) {
      m = pr[l];
    } else {
      m = su[r];
    }
    std::cout << m << "\n";
  }
}