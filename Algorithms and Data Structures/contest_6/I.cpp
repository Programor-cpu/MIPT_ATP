#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
const int cBig = 1e9 + 7;

#define vvv std::vector<std::vector<std::vector<int>>>

int Counting(const std::string& one, const std::string& two, int k) {
  vvv dp(one.size() + 1, std::vector<std::vector<int>>(
                             two.size() + 1, std::vector<int>(k + 1, cBig)));
  dp[0][0][0] = 0;
  for (int i = 0; i != (int)one.size() + 1; ++i) {
    for (int j = 0; j != (int)two.size() + 1; ++j) {
      for (int z = 0; z != k + 1; ++z) {
        if (i == 0 && j == 0 && z == 0) {
          continue;
        }
        if (i > 0 && j > 0) {
          if (one[i - 1] == two[j - 1]) {
            dp[i][j][z] = std::min(dp[i][j][z], dp[i - 1][j - 1][z]);
          } else {
            dp[i][j][z] = std::min(dp[i][j][z], dp[i - 1][j - 1][z] + 1);
            if (z > 0) {
              dp[i][j][z] = std::min(dp[i][j][z], dp[i - 1][j - 1][z - 1]);
            }
          }
        }
        if (z > 0) {
          if (i > 0) {
            dp[i][j][z] = std::min(dp[i][j][z], dp[i - 1][j][z - 1]);
          }
          if (j > 0) {
            dp[i][j][z] = std::min(dp[i][j][z], dp[i][j - 1][z - 1]);
          }
        }
      }
    }
  }
  int m = cBig;
  for (int i = 0; i != k + 1; ++i) {
    m = std::min(m, dp[one.size()][two.size()][i]);
  }
  return m;
}

int main() {
  std::string two;
  std::string one;
  int k;
  std::cin >> two >> one >> k;
  int answer = Counting(one, two, k);
  if (answer == cBig) {
    std::cout << "-1" << '\n';
  } else {
    std::cout << answer << '\n';
  }
}