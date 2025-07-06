#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

bool Comp(std::pair<std::pair<long long int, long long int>, long long int> a,
          std::pair<std::pair<long long int, long long int>, long long int> b) {
  if (a.first.second == b.first.second) {
    return (a.first.first < b.first.first);
  }
  return (a.first.second > b.first.second);
}

int main() {
  long long int n;
  long long int time = 0;
  std::cin >> n;
  std::vector<std::pair<std::pair<long long int, long long int>, long long int>>
      works(n);
  for (long long int i = 0; i < n; i++) {
    long long int t;
    std::cin >> t;
    works[i].first.first = t;
    works[i].second = i + 1;
  }
  for (long long int i = 0; i < n; i++) {
    long long int t;
    std::cin >> t;
    works[i].first.second = t;
    time += t;
  }
  std::vector<std::pair<std::pair<long long int, long long int>, long long int>>
      firstones;
  std::vector<std::pair<std::pair<long long int, long long int>, long long int>>
      secondones;
  for (long long int i = 0; i < n; i++) {
    if (works[i].first.first <= works[i].first.second) {
      firstones.push_back(works[i]);
    } else {
      secondones.push_back(works[i]);
    }
  }
  std::sort(firstones.begin(), firstones.end());
  std::sort(secondones.begin(), secondones.end(), Comp);
  std::vector<std::pair<std::pair<long long int, long long int>, long long int>>
      workss;
  for (long long int i = 0; i < (long long int)firstones.size(); i++) {
    workss.push_back(firstones[i]);
  }
  for (long long int i = 0; i < (long long int)secondones.size(); i++) {
    workss.push_back(secondones[i]);
  }
  long long int m = workss[0].first.first;
  long long int current = workss[0].first.first;
  for (long long int i = 1; i < n; i++) {
    current = current + workss[i].first.first - workss[i - 1].first.second;
    m = std::max(current, m);
  }
  std::cout << (time + m);
  std::cout << '\n';
  for (long long int i = 0; i < n; i++) {
    std::cout << workss[i].second << ' ';
  }
  std::cout << '\n';
  for (long long int i = 0; i < n; i++) {
    std::cout << workss[i].second << ' ';
  }
}