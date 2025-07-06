#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

const int cMin = -2147483648;
const int cMax = 2147483647;

void LIS(const std::vector<int>& arr, int n) {
  int len = 0;
  std::vector<int> dp(n + 1, cMax);
  dp[0] = cMin;
  std::vector<int> pref(n);
  std::vector<int> positions(n + 1, -1);
  for (int i = 0; i != n; ++i) {
    int find = std::upper_bound(dp.begin(), dp.end(), arr[i]) - dp.begin();
    if (arr[i] >= dp[find - 1] && arr[i] <= dp[find]) {
      len = std::max(len, find);
      dp[find] = arr[i];
      positions[find] = i;
      pref[i] = positions[find - 1];
    }
  }
  std::cout << len << '\n';
  std::stack<int> sequence;
  for (int i = positions[len]; i != -1;) {
    sequence.push(i + 1);
    i = pref[i];
  }
  for (int i = 0; i != len; ++i) {
    std::cout << sequence.top() << ' ';
    sequence.pop();
  }
}

int main() {
  int n;
  std::cin >> n;
  std::vector<int> arr(n);
  for (int i = 0; i != n; ++i) {
    std::cin >> arr[i];
    arr[i] *= -1;
  }
  LIS(arr, n);
}