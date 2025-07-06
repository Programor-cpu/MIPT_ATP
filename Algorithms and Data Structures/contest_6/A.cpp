#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

int main() {
  int k;
  int n;
  int m;
  std::cin >> k;
  while (k != 0) {
    --k;
    std::cin >> n >> m;
    std::cout << n * m - 1 << '\n';
  }
}