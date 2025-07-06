#include <algorithm>
#include <iostream>
#include <vector>
const int cBig = 1e9 + 7;
const int cSide = 2;

bool DPFill(const std::vector<std::vector<int>>& field,
            const std::vector<std::vector<int>>& checker, int i, int j, int z,
            int dif) {
  return (((i == 0 || j == 0 || (checker[z][(1 - dif) / 2]) == 1)) &&
          (field[j][i] - dif != 0));
}

int FillCheck(int i, int n) {
  return ((1 & i) + (1 & (i >> 1)) + (1 & (i >> n)));
}

int CountVariants(const std::vector<std::vector<int>>& field, int n, int m) {
  int result = 0;
  int size = (1 << (n + 1));
  int bit_n = (1 << n);
  int bit_z = 0;
  int index_first = 0;
  int index_second = 0;
  std::vector<std::vector<int>> dp(size, std::vector<int>(cSide, 0));
  if (field[0][0] > -1) {
    ++dp[bit_n][0];
  }
  if (field[0][0] < 1) {
    ++dp[0][0];
  }
  std::vector<std::vector<int>> checker(size, std::vector<int>(cSide, 0));
  for (int i = 0; i != size; ++i) {
    if (FillCheck(i, n) == cSide - 1) {
      ++checker[i][1];
      continue;
    }
    if (FillCheck(i, n) == cSide) {
      ++checker[i][0];
      continue;
    }
  }
  for (int i = 0; i != m; ++i) {
    for (int j = 0; j != n; ++j) {
      index_first = (j + n * i) % cSide;
      index_second = (index_first + 1) % cSide;
      for (int z = 0; z != size; ++z) {
        bit_z = (z >> 1);
        if (DPFill(field, checker, i, j, z, 1)) {
          dp[bit_z][index_first] += dp[z][index_second];
          dp[bit_z][index_first] %= cBig;
        }
        if (DPFill(field, checker, i, j, z, -1)) {
          dp[(bit_n | bit_z)][index_first] += dp[z][index_second];
          dp[(bit_n | bit_z)][index_first] %= cBig;
        }
      }
      for (int z = 0; z != size; ++z) {
        dp[z][index_second] = 0;
      }
    }
  }
  for (int i = 0; i != size; ++i) {
    result += dp[i][(n * m - 1) % cSide];
    result %= cBig;
  }
  return result;
}

int main() {
  int n;
  int m;
  char input;
  std::cin >> n >> m;
  if (m != 1 && n != 1) {
    std::vector<std::vector<int>> field(n, std::vector<int>(m, 0));
    for (int i = 0; i != n; ++i) {
      for (int j = 0; j != m; ++j) {
        std::cin >> input;
        if (input == '+') {
          --field[i][j];
          continue;
        }
        if (input == '-') {
          ++field[i][j];
          continue;
        }
      }
    }
    std::cout << CountVariants(field, n, m) << '\n';
  } else {
    std::cout << '0' << '\n';
  }
}
