#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

int main() {
  long long int s;
  std::cin >> s;
  long long int d;
  std::cin >> d;
  std::vector<std::pair<long long int, int>> sheeps(s);
  std::vector<std::pair<long long int, int>> dogs(d);
  std::vector<std::pair<long long int, std::pair<long long int, long long int>>>
      conclusion;
  for (int i = 0; i < s; i++) {
    long long int ingoing;
    std::cin >> ingoing;
    sheeps[i].first = ingoing;
    sheeps[i].second = i + 1;
  }
  for (int i = 0; i < d; i++) {
    long long int ingoing;
    std::cin >> ingoing;
    dogs[i].first = ingoing;
    dogs[i].second = i + 1;
  }
  std::sort(sheeps.begin(), sheeps.end());
  std::sort(dogs.begin(), dogs.end());
  std::vector<
      std::pair<std::pair<long long int, int>, std::pair<long long int, int>>>
      guards;
  int p = 0;
  int v = (int)dogs.size() / 2;
  for (int i = 0; i < (int)sheeps.size(); i++) {
    if (dogs[p].first >= sheeps[i].first) {
      continue;
    }
    while (true) {
      if (v >= (int)dogs.size()) {
        break;
      }
      if (dogs[p].first < sheeps[i].first && dogs[v].first > sheeps[i].first) {
        conclusion.push_back(
            {sheeps[i].second, {dogs[p].second, dogs[v].second}});
        v += 1;
        p += 1;
        break;
      }
      v += 1;
    }
  }

  std::cout << (int)conclusion.size() << '\n';
  for (int i = 0; i < (int)conclusion.size(); i++) {
    std::cout << conclusion[i].first << ' ' << conclusion[i].second.first << ' '
              << conclusion[i].second.second << '\n';
  }
}