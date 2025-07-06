#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

const int cBig = 2147483647;

void Dynamism(const std::vector<int>& arr, int n, int m) {
  ++n;
  int h = 2 + m;
  std::vector<std::vector<int>> dp(n, std::vector<int>(h));
  std::vector<std::vector<int>> id(n, std::vector<int>(h));
  std::vector<int> s(n);
  for (int i = 1; i != n; i++) {
    s[i] += (s[i - 1] + arr[i]);
  }
  --n;
  for (int i = 2; i != n + 1; i++) {
    if (n - i == 0) {
      m++;
    }
    for (int j = 1; j != std::min(i, m) + 1; j++) {
      if (j != 1) {
        int mx = cBig;
        for (int w = i - 1; w != 0; w--) {
          if (w + 1 == i) {
            dp[i][j] = dp[i - 1][j - 1];
            id[i][j] = i - 1;
            mx = dp[i - 1][j - 1];
          }
          int help = w;
          ++help;
          while (abs(arr[help] - arr[i]) >= abs(arr[help] - arr[w])) {
            help += 1;
          }
          int right = i - help;
          int left = help - 1 - w;
          int counter = 0;
          if (right != 0) {
            counter += ((arr[i] * right) - s[i - 1] + s[help - 1]);
          }
          if (left != 0) {
            counter += abs((arr[w] * left) - s[help - 1] + s[w]);
          }
          if (dp[w][j - 1] < mx - counter) {
            mx = dp[w][j - 1] + counter;
            dp[i][j] = mx;
            id[i][j] = w;
          }
        }

      } else {
        dp[i][j] = (i - 1) * arr[i] - s[i - 1];
      }
    }
  }
  std::cout << dp[n][m] << '\n';
  std::vector<int> answer;
  int c = id[n][m];
  --m;
  while (m != 0) {
    answer.push_back(arr[c]);
    c = id[c][m];
    m -= 1;
  }
  for (int i = (int)answer.size() - 1; i != -1; i--) {
    std::cout << answer[i] << ' ';
  }
}

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  std::vector<int> arr(1 + n);
  arr.push_back(cBig);
  for (int i = 0; i != n; ++i) {
    std::cin >> arr[i + 1];
  }
  Dynamism(arr, n + 1, m);
}