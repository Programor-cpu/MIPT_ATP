#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cBig = 1e9 + 7;

void BinarySearchSpecified(const std::vector<std::vector<int>>& dp, int& left,
                           int& right, int i, int j) {
  int middle = 0;
  while (right > left + 1) {
    middle = left + (right - left) / 2;
    if (dp[j - middle][i] >= dp[middle][i - 1]) {
      left = middle;
    } else {
      right = middle;
    }
  }
}

int CountingThrows(int n, int k) {
  ++n;
  ++k;
  std::vector<std::vector<int>> dp(n, std::vector<int>(k, cBig));
  for (int i = 0; i != k; ++i) {
    dp[0][i] = 0;
  }
  for (int i = 1; i != k; ++i) {
    for (int j = 1; j != n; ++j) {
      int right_border = j;
      int left_border = 1;
      BinarySearchSpecified(dp, left_border, right_border, i, j);
      dp[j][i] =
          std::min(std::max(dp[left_border][i - 1], dp[right_border][i - 1]),
                   std::max(dp[j - left_border][i], dp[j - right_border][i]));
      ++dp[j][i];
    }
  }
  return dp[n - 1][k - 1] - 1;
}

int main() {
  int n;  // floor amount
  int k;  // balls amount (GIVE US YOUR BALLS)
  int answer;
  std::cin >> n >> k;
  int enough_balls = static_cast<int>(log2(n)) + 1;
  if (k > 0 && n > 1) {
    if (k > enough_balls) {
      k = enough_balls;
    }
    answer = CountingThrows(n, k);
    std::cout << answer << '\n';
  } else if (n == 1) {
    std::cout << "0" << '\n';
  } else {
    std::cout << "-1" << '\n';
  }
}
