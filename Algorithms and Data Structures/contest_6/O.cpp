#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
const int cLength = 1000005;
const int cCritical = 1e9 + 7;

int Difseq(const std::vector<int>& arr, int n) {
  std::vector<long long> dp(n + 1, 0);
  std::vector<long long> lol(cLength, -1);
  dp[0] = 1;
  for (int i = 1; i != n + 1; ++i) {
    dp[i] = ((2 * dp[i - 1]) + cCritical) % cCritical;
    if (lol[arr[i - 1]] + 1 != 0) {
      dp[i] = ((dp[i] - dp[lol[arr[i - 1]]]) + cCritical) % cCritical;
    }
    lol[arr[i - 1]] = i - 1;
  }
  --dp[n];
  return dp[n];
}

int main() {
  int n;
  std::cin >> n;
  std::vector<int> arr(n);
  for (int i = 0; i != n; ++i) {
    std::cin >> arr[i];
  }
  std::cout << Difseq(arr, n);
}