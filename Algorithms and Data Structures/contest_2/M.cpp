#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

long long int FirstTheBest(std::vector<long long int>& hor, long long int& n) {
  long long int m;
  long long int ps = 0;
  if (n % 2 == 0) {
    m = n / 2;
  } else {
    m = n / 2 + 1;
  }
  for (int i = 0; i != m; i++) {
    ps += hor[i];
  }
  std::vector<long long int> cir;
  cir.push_back(ps);
  for (long long int i = 1; i != n + m - 1; i++) {
    ps = ps + hor[(m + i - 1 + n) % n] - hor[(i - 1 + n) % n];
    cir.push_back(ps);
  }
  long long int w = (long long int)cir.size();
  std::vector<long long int> conclusion(w - m + 1);
  std::vector<long long int> b(w);
  std::vector<long long int> c(w);
  for (long long int j = 0; j != w; j++) {
    if (j % m != 0) {
      b[j] = std::min(b[j - 1], cir[j]);
    } else {
      b[j] = cir[j];
    }
  }
  c[w - 1] = cir[w - 1];
  for (long long int j = w - 2; j != -1; j--) {
    if ((j + 1) % m != 0) {
      c[j] = std::min(c[j + 1], cir[j]);
    } else {
      c[j] = cir[j];
    }
  }
  for (long long j = 0; j + m - 1 != w; j++) {
    conclusion[j] = std::min(c[j], b[j + m - 1]);
  }
  return *std::max_element(begin(conclusion), end(conclusion));
}

int main() {
  std::vector<long long int> hor;
  long long int n;
  long long int a;
  long long int s = 0;
  std::cin >> n;
  for (long long int i = 0; i < n; i++) {
    std::cin >> a;
    s += a;
    hor.push_back(a);
  }

  std::cout << FirstTheBest(hor, n) << ' ' << s - FirstTheBest(hor, n);
}