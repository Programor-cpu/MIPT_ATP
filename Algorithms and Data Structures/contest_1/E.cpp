#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cBig = 2147483647;

void ArrPrepare(std::vector<std::pair<int, int>>& arr) {
  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    std::pair<int, int> ingoing;
    std::cin >> ingoing.first;
    ingoing.second = i;
    arr.push_back(ingoing);
  }
  std::sort(arr.begin(), arr.end());
}

std::pair<std::pair<int, int>, int> Search(
    unsigned int s, std::vector<std::pair<int, int>>& a,
    std::vector<std::pair<int, int>>& b, std::vector<std::pair<int, int>>& c) {
  std::pair<std::pair<int, int>, int> answer;
  answer.first.first = cBig;
  answer.first.second = cBig;
  answer.second = cBig;
  unsigned int sum;
  for (int i = 0; i < (int)a.size(); i++) {
    if (s <= (unsigned int)a[i].first) {
      break;
    }
    int j = 0;
    int k = (int)c.size() - 1;
    std::pair<std::pair<int, int>, int> current;
    while ((j < (int)b.size()) && (k >= 0)) {
      sum = a[i].first + b[j].first + c[k].first;
      current.first.first = a[i].second;
      current.first.second = b[j].second;
      current.second = c[k].second;
      if (s == sum) {
        answer = std::min(current, answer);
        k -= 1;
      } else if (s > sum) {
        j += 1;
      } else {
        k -= 1;
      }
    }
  }
  if (answer.first.first != cBig) {
    return answer;
  }
  return {{-1, -1}, -1};
}

int main() {
  unsigned int s;
  std::cin >> s;
  std::vector<std::pair<int, int>> a;
  std::vector<std::pair<int, int>> b;
  std::vector<std::pair<int, int>> c;
  ArrPrepare(a);
  ArrPrepare(b);
  ArrPrepare(c);
  std::pair<std::pair<int, int>, int> answer = Search(s, a, b, c);
  if (answer.first.first != -1) {
    std::cout << answer.first.first << ' ' << answer.first.second << ' '
              << answer.second;
  } else {
    std::cout << -1;
  }
}