#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

void Dynamism(long long n) {
  long long answer = 0;
  std::vector<std::vector<std::pair<long long, long long>>> dp(
      n + 1, std::vector<std::pair<long long, long long>>(n + 1, {0, 0}));
  dp[1][1].first++;
  dp[1][1].second++;
  for (long long i = 2; i != n + 1; i++) {
    for (long long j = 1; j != n + 1; j++) {
      if (j > i) {
        dp[i][j].first += dp[i / 2][j - i].second;
        dp[i][j].second += (dp[i - 1][j].second + dp[i][j].first);
      } else if (i == j) {
        dp[i][j].second = dp[i - 1][j].second;
        dp[i][j].second++;
        dp[i][j].first += 1;
      } else {
        dp[i][j].second = dp[i - 1][j].second;
      }
    }
    answer += dp[i][n].first;
  }
  std::cout << answer + dp[1][n].first;
}

int main() {
  long long n;
  std::cin >> n;
  Dynamism(n);
}