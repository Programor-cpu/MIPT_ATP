#include <algorithm>
#include <iostream>
#include <vector>

std::vector<long long> Dynamism(const std::vector<long long>& arr, long long n,
                                bool type) {
  ++n;
  long long help = 2;
  std::vector<long long> dp(n);
  dp[1] += arr[0];
  for (long long i = help - 1; i != n - 1; ++i) {
    if (dp[help - 1] - arr[i] != 0) {
      if ((((dp[help - 1] - arr[i] < 0) && (help % 2 == 0)) ||
           ((dp[help - 1] - arr[i] > 0) && (help % 2 == 1))) &&
          type) {
        dp[help] = arr[i];
        ++help;
      } else if ((((dp[help - 1] - arr[i] > 0) && (help % 2 == 0)) ||
                  ((dp[help - 1] - arr[i] < 0) && (help % 2 == 1))) &&
                 !type) {
        dp[help] = arr[i];
        ++help;
      }
      dp[help - 1] = arr[i];
    }
  }
  while ((long long)dp.size() != help) {
    dp.pop_back();
  }
  return dp;
}

int main() {
  long long n;
  std::cin >> n;
  std::vector<long long> arr(n);
  for (long long i = 0; i != n; ++i) {
    std::cin >> arr[i];
  }
  std::vector<long long> one = Dynamism(arr, n, true);
  std::vector<long long> two = Dynamism(arr, n, false);
  if (one.size() > two.size()) {
    std::cout << one.size() - 1 << '\n';
    for (size_t i = 1; i != one.size(); ++i) {
      std::cout << one[i] << ' ';
    }
  } else {
    std::cout << two.size() - 1 << '\n';
    for (size_t i = 1; i != two.size(); ++i) {
      std::cout << two[i] << ' ';
    }
  }
}