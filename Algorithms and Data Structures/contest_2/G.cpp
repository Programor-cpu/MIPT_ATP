#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int main() {
  std::vector<std::pair<std::string, std::pair<int, int>>> hor;
  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    std::string a;
    std::cin >> a;
    hor.push_back({a, {(i - 1) % n, (i + 1) % n}});
  }
  for (int j = 0; j < n - 3; j++) {
    int i;
    std::cin >> i;
    std::cout << hor[(hor[(i - 1 + n) % n].second.first + n) % n].first << ' '
              << hor[(hor[(i - 1 + n) % n].second.second + n) % n].first
              << '\n';
    hor[(hor[(i - 1 + n) % n].second.first + n) % n].second.second =
        hor[(i - 1 + n) % n].second.second;
    hor[(hor[(i - 1 + n) % n].second.second) % n].second.first =
        hor[(i - 1 + n) % n].second.first;
  }
}