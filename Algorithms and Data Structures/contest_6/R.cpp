#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <vector>
const int cBig = 1e9;

int Binpow(int n) { return (1 << n); }
int Updatemask(int mask, int n) {
  int g = (Binpow(n) | mask);
  return g;
}
int Byte(int a, int b) {
  int g = 1 & (a >> b);
  return g;
}

void Ans(const std::vector<std::vector<int>>& matrix,
         std::vector<std::vector<int>>& dp, int n) {
  for (int i = 1; i != Binpow(n); i++) {
    for (int j = 0; j != n; j++) {
      for (int z = 0; z != n; z++) {
        if (Byte(i, z) == 0) {
          dp[z][Updatemask(i, z)] =
              std::min(dp[j][i] + matrix[j][z], dp[z][Updatemask(i, z)]);
        }
      }
    }
  }
}
void Req(const std::vector<std::vector<int>>& matrix,
         std::vector<std::vector<int>>& dp, int next, std::vector<int>& way,
         int mask) {
  way.push_back(next + 1);
  if (way.size() == matrix.size()) {
    return;
  }
  for (int i = 0; i != (int)matrix.size(); i++) {
    if ((Byte(mask, i) == 1) && i != next) {
      if (dp[next][mask] == (dp[i][mask & ~(Binpow(next))] + matrix[i][next])) {
        Req(matrix, dp, i, way, mask & ~(Binpow(next)));
        return;
      }
    }
  }
}

void TSP(const std::vector<std::vector<int>>& matrix, int n) {
  std::vector<std::vector<int>> dp(n, std::vector<int>(n * Binpow(n), 0));
  for (int i = 0; i != n; i++) {
    for (int j = 0; j != Binpow(n); j++) {
      if (j - Binpow(i) != 0) {
        dp[i][j] += cBig;
      }
    }
  }
  std::vector<int> way;
  int mask = Binpow(n) - 1;
  int answer = cBig;
  int next = cBig;
  Ans(matrix, dp, n);
  for (int i = 0; i != n; ++i) {
    if (answer - dp[i][mask] > 0) {
      next = i;
      answer = dp[i][mask];
    }
  }
  std::cout << answer << '\n';
  Req(matrix, dp, next, way, Binpow(n) - 1);
  for (int i = 0; i != n; ++i) {
    std::cout << way[i] << ' ';
  }
}

int main() {
  int n;
  std::cin >> n;
  std::vector<std::vector<int>> arr(n, std::vector<int>(n));
  for (int i = 0; i != n; ++i) {
    for (int j = 0; j != n; ++j) {
      std::cin >> arr[i][j];
    }
  }
  TSP(arr, n);
}