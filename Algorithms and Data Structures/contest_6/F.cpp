#include <algorithm>
#include <iostream>
#include <vector>
const long long cBig = -1e9;

struct Object {
  long long w = 0;
  long long c = 0;
  long long f = 0;
  bool operator<(const Object& one) const { return f < one.f; }
};

void Dynamism(std::vector<Object>& arr, long long n, long long g, long long m) {
  std::sort(arr.begin(), arr.end());
  long long help = 0;
  long long h = 0;
  long long answer = 0;
  ++g;
  ++m;
  std::vector<std::vector<long long>> dp(g, std::vector<long long>(m, cBig));
  dp[0][0] -= cBig;
  for (long long i = 1; i != g; i++) {
    for (long long j = help; (n > j) && (arr[j].f - arr[help].f == 0); j++) {
      for (long long z = 0; z != m; z++) {
        h = z - arr[j].w;
        if (0 < dp[i - 1][z] - dp[i][z]) {
          dp[i][z] = dp[i - 1][z];
        }
        if ((-1 < h) && (dp[i - 1][h] + 1 != 0) &&
            (arr[j].c > -(dp[i - 1][h] - dp[i][z]))) {
          dp[i][z] = arr[j].c + dp[i - 1][h];
        }
      }
      if ((n - 1 > j) && (0 != arr[j + 1].f - arr[help].f)) {
        help = j;
        ++help;
        break;
      }
    }
  }
  for (long long i = 0; i != m; i++) {
    if (dp[g - 1][i] > answer) {
      answer = dp[g - 1][i];
    }
  }
  std::cout << answer;
}

int main() {
  long long n;
  long long g;
  long long m;
  std::cin >> n >> g >> m;
  std::vector<Object> arr(n);
  for (long long i = 0; i != n; i++) {
    std::cin >> arr[i].w >> arr[i].c >> arr[i].f;
  }
  Dynamism(arr, n, g, m);
}