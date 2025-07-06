#include <algorithm>
#include <iostream>
#include <vector>
const long long cBig = 1e9 + 9;
const long long cSize = 4;
const std::vector<long long> cBaze = {1, 1, 1, 0};
const std::vector<long long> cEd = {1, 0, 0, 1};
// RINGS, FIELDS

void Multiply(std::vector<long long>& ans, const std::vector<long long>& one,
              const std::vector<long long>& two) {
  ans[0] = ((one[0] * two[0]) % cBig + (one[1] * two[2]) % cBig) % cBig;
  ans[1] = ((one[0] * two[1]) % cBig + (one[1] * two[3]) % cBig) % cBig;
  ans[2] = ((one[2] * two[0]) % cBig + (one[3] * two[2]) % cBig) % cBig;
  ans[3] = ((one[2] * two[1]) % cBig + (one[3] * two[3]) % cBig) % cBig;
}

void Pow(std::vector<long long>& ans, std::vector<long long>& one,
         std::vector<long long>& two, const long long& pow) {
  if (pow <= 0) {
    ans = cEd;
    return;
  }
  if (pow == 1) {
    ans = cBaze;
    return;
  }
  Pow(ans, one, two, pow / 2);
  one = ans;
  two = ans;
  if (pow % 2 == 0) {
    Multiply(ans, one, two);
    return;
  }
  Multiply(ans, one, two);
  one = ans;
  Multiply(ans, one, cBaze);
}

long long Fib(const long long& n) {
  std::vector<long long> ans(cSize);
  std::vector<long long> one(cSize);
  std::vector<long long> two(cSize);
  Pow(ans, one, two, n - 1);
  return ans[0];
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  int n;
  int q;
  long long c;
  long long a;
  long long b;
  std::cin >> n >> q;
  std::vector<std::pair<long long, long long>> arr(n);
  std::pair<long long, long long> in;
  for (int i = 0; i != n; ++i) {
    std::cin >> a >> b >> in.second;
    in.first = a - b;
    in.second %= cBig;
    arr[i] = in;
  }
  std::sort(arr.begin(), arr.end());
  for (int i = 0; i != q; ++i) {
    std::cin >> c;
    long long sum = 0;
    for (int j = 0; j != n; ++j) {
      if (-arr[j].first - c < 0) {
        break;
      }
      sum += Fib(-arr[j].first - c + 1) * arr[j].second;
      sum %= cBig;
    }
    std::cout << sum << '\n';
  }
}
