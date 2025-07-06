#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <vector>

long long int Calculations(std::vector<long long int>& hes) {
  std::stack<std::pair<long long int, long long int>> storage;
  std::vector<long long int> left;
  std::vector<long long int> right;
  long long int m = 0;
  for (long long int i = 0; i != (long long int)hes.size(); i++) {
    if (i == 0) {
      storage.push({hes[0], 0});
      left.push_back(-1);
    } else {
      while (!storage.empty() && storage.top().first >= hes[i]) {
        storage.pop();
      }
      if (!storage.empty()) {
        left.push_back(storage.top().second);
      } else {
        left.push_back(-1);
      }
      storage.push({hes[i], i});
    }
  }
  while (!storage.empty()) {
    storage.pop();
  }
  for (long long int i = (long long int)hes.size() - 1; i != -1; i--) {
    if (i == (long long int)hes.size() - 1) {
      storage.push(
          {hes[(long long int)hes.size() - 1], (long long int)hes.size() - 1});
      right.push_back((long long int)hes.size());
    } else {
      while (!storage.empty() && storage.top().first >= hes[i]) {
        storage.pop();
      }
      if (!storage.empty()) {
        right.push_back(storage.top().second);
      } else {
        right.push_back((long long int)hes.size());
      }
      storage.push({hes[i], i});
    }
  }
  for (long long int i = 0; i != (long long int)hes.size(); i++) {
    m = std::max(
        (right[(long long int)hes.size() - i - 1] - left[i] - 1) * hes[i], m);
  }
  return m;
}

int main() {
  long long int n;
  std::cin >> n;
  std::vector<long long int> hes;
  for (long long int i = 0; i != n; i++) {
    long long int ingoing;
    std::cin >> ingoing;
    hes.push_back(ingoing);
  }
  std::cout << Calculations(hes);
}
