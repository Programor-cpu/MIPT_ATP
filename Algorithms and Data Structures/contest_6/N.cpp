#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int LCS(const std::vector<int>& one, const std::vector<int>& two) {
  std::vector<int> dp(two.size(), 0);
  int n = (int)one.size();
  int m = (int)two.size();
  int len;
  for (int i = 0; i != n; ++i) {
    len = 0;
    for (int j = 0; j != m; ++j) {
      if (one[i] == two[j]) {
        dp[j] = std::max(dp[j], len + 1);
      } else if (two[j] < one[i]) {
        len = std::max(dp[j], len);
      }
    }
  }
  int maxl = 0;
  for (int i = 0; i != m; i++) {
    if (dp[i] > maxl) {
      maxl = dp[i];
    }
  }
  return maxl;
}

int main() {
  int n;
  int k;
  std::cin >> n >> k;
  std::vector<int> one(n);
  std::vector<int> two(k);
  for (int i = 0; i != n; i++) {
    std::cin >> one[i];
  }
  for (int i = 0; i != k; i++) {
    std::cin >> two[i];
  }
  if (one.size() > two.size()) {
    std::swap(one, two);
  }
  std::cout << LCS(one, two);
}