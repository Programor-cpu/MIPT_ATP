#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const int cNum = 100;
const int cBegin = 2;

void Dynamism(int n) {
  long long answer = 0;
  ++n;
  std::vector<std::vector<long long>> dp(n, std::vector<long long>(cNum, 0));
  std::vector<std::vector<long long>> pd = dp;
  ++dp[cBegin - 1][cBegin - 1];
  for (int i = cBegin; i != n; i++) {
    for (int j = cBegin - 1; j != n; j++) {
      for (int z = cBegin - 1; z != cNum; z++) {
        if (n - z > j) {
          int g = z + j;
          pd[g][z] = dp[j][z] + pd[g][z];
          if ((n - z + 1 > j + 2 * i) && (cNum > z + 1)) {
            pd[g + 2 * i - 1][z + 1] = dp[j][z] + pd[g + 2 * i - 1][z + 1];
          }
        }
        if (j - n >= -1) {
          answer = answer + dp[j][z];
        }
      }
    }
    dp = pd;
    for (int j = cBegin - 1; j != n; j++) {
      for (int z = cBegin - 1; z != cNum; z++) {
        pd[j][z] -= pd[j][z];
      }
    }
  }
  for (int i = cBegin - 1; i != cNum; i++) {
    answer = answer + dp[n - 1][i];
  }
  std::cout << answer << '\n';
}

int main() {
  int n;
  std::cin >> n;
  Dynamism(n);
}