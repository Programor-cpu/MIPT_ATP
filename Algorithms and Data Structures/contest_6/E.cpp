#include <algorithm>
#include <iostream>
#include <vector>

void Backtrack(const std::vector<int>& weights, const std::vector<int>& costs,
               int weight_limit, int n) {
  std::vector<std::vector<int>> dp(n + 1,
                                   std::vector<int>(weight_limit + 1, 0));
  for (int i = 1; i != n + 1; ++i) {
    for (int j = 1; j != weight_limit + 1; ++j) {
      if (weights[i - 1] > j) {
        dp[i][j] = dp[i - 1][j];
      } else {
        dp[i][j] = std::max(dp[i - 1][j - weights[i - 1]] + costs[i - 1],
                            dp[i - 1][j]);
      }
    }
  }
  std::vector<int> packed_items;
  int x = weight_limit;
  int y = n;
  while (dp[y][x] != 0) {
    if (dp[y][x] == dp[y - 1][x]) {
      --y;
      continue;
    }
    packed_items.push_back(y);
    x -= weights[y - 1];
    --y;
  }
  for (int i = static_cast<int>(packed_items.size()) - 1; i != -1; --i) {
    std::cout << packed_items[i] << '\n';
  }
}

int main() {
  int n;
  int weight_limit;
  std::cin >> n >> weight_limit;
  std::vector<int> weights(n);
  std::vector<int> costs(n);
  for (int i = 0; i != n; ++i) {
    std::cin >> weights[i];
  }
  for (int i = 0; i != n; ++i) {
    std::cin >> costs[i];
  }
  Backtrack(weights, costs, weight_limit, n);
}