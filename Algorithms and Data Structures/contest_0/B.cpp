#include <cmath>
#include <iostream>
#include <vector>
int main() {
  const int cEnf = 20;
  int n;
  std::cin >> n;
  std::vector<long double> arr;
  for (int i = 0; i < n; i++) {
    long double a;
    std::cin >> a;
    if (i == 0) {
      arr.push_back(log(a));
    } else {
      arr.push_back(log(a) + arr[i - 1]);
    }
  }
  int q;
  std::cin >> q;
  for (int i = 0; i < q; i++) {
    int l;
    int r;
    std::cin >> l;
    std::cin >> r;
    long double pr;
    long double poc = r - l + 1.00;
    if (l != 0) {
      pr = exp((arr[r] - arr[l - 1]) / poc);
    } else {
      pr = exp(arr[r] / poc);
    }
    std::cout.precision(cEnf);
    std::cout << pr << '\n';
  }
}