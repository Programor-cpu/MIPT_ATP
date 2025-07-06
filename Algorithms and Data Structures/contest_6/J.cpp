#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
const unsigned int cBig = 1e9 + 7;

unsigned int Counting(const std::vector<unsigned int>& arr, unsigned int n,
                      unsigned int transpose) {
  std::vector<std::vector<unsigned int>> dp(
      n + 1, std::vector<unsigned int>(transpose + 1, 0));
  std::vector<std::vector<unsigned int>> dp_old(
      n + 1, std::vector<unsigned int>(transpose + 1, 0));
  int answer = 0;
  for (int i = 0; i != (int)n + 1; ++i) {
    dp_old[i][0] = 1;
  }
  for (unsigned int i = 0; i != (unsigned int)arr.size(); ++i) {
    for (unsigned int k = i + 1;
         k != n - ((unsigned int)arr.size() - i - 1) + 1; ++k) {
      unsigned int t;
      if (arr[i] > k) {
        t = arr[i] - k;
      } else {
        t = k - arr[i];
      }
      for (unsigned int j = 0; j < transpose + 1; ++j) {
        if (j >= t) {
          dp[k][j] = dp_old[k - 1][j - t];
        }
        dp[k][j] += dp[k - 1][j];
        dp[k][j] %= cBig;
      }
    }
    dp_old = dp;
    fill(dp.begin(), dp.end(), std::vector<unsigned int>(transpose + 1, 0));
  }
  for (int i = transpose % 2; i <= (int)transpose; i += 2) {
    answer += dp_old[n][i];
    answer %= cBig;
  }
  return answer;
}

int main() {
  unsigned int n;
  unsigned int k;
  unsigned int in;
  std::cin >> n >> k;
  std::vector<unsigned int> arr;
  for (unsigned int i = 0; i != n; ++i) {
    std::cin >> in;
    if (in == 1) {
      arr.push_back(i + 1);
    }
  }
  std::cout << Counting(arr, n, k);
}
