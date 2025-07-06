#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

void LCS(const std::string& one, const std::string& two) {
  size_t l_o = one.size();
  size_t l_t = two.size();
  std::vector<std::vector<int>> dp(l_o + 1, std::vector<int>(l_t + 1, 0));
  for (size_t i = 1; i != l_o + 1; i++) {
    for (size_t j = 1; j != l_t + 1; j++) {
      if (one[i - 1] != two[j - 1]) {
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
      } else {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      }
    }
  }
  std::cout << dp[one.size()][two.size()] << '\n';
  std::vector<std::pair<int, int>> backtrack;
  size_t n = l_o;
  size_t m = l_t;
  while (n != 0 && m != 0) {
    if (dp[n][m - 1] == dp[n][m]) {
      --m;
      continue;
    }
    if (dp[n - 1][m] == dp[n][m]) {
      --n;
      continue;
    }
    if (dp[n - 1][m - 1] + 1 == dp[n][m]) {
      backtrack.push_back({n, m});
      --n;
      --m;
      continue;
    }
  }
  for (int i = (int)backtrack.size() - 1; i >= 0; i--) {
    std::cout << backtrack[i].first << " ";
  }
  std::cout << '\n';
  for (int i = (int)backtrack.size() - 1; i >= 0; i--) {
    std::cout << backtrack[i].second << " ";
  }
  std::cout << '\n';
}

int main() {
  std::string one;
  std::string two;
  std::cin >> one >> two;
  LCS(one, two);
}