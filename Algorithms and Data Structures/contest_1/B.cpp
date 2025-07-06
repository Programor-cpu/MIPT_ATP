#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cOne = 123;
const int cTwo = 45;
const int cThree = pow(10, 7);
const int cFour = 4321;

int DeParture(int first, int last, std::vector<int>& arr) {
  if (first != last) {
    int pudge = arr[first + (last - first) / 2];
    arr[first + (last - first) / 2] = arr[last];
    arr[last] = pudge;
  }
  int j = first - 1;
  int rightnum = arr[last];
  for (int i = first; i < last + 1; i++) {
    if (arr[i] <= rightnum) {
      j += 1;
      std::swap(arr[i], arr[j]);
    }
  }
  return j;
}

int QuickSelect(int k, std::vector<int>& arr) {
  k -= 1;
  int last = arr.size() - 1;
  int first = 0;
  while (true) {
    int p = DeParture(first, last, arr);
    if (k < p) {
      last = p - 1;
    } else if (k > p) {
      first = p + 1;
    } else {
      return arr[k];
    }
  }
}

int main() {
  int n;
  int k;
  std::vector<int> arr;
  int a;

  std::cin >> n;
  std::cin >> k;
  std::cin >> a;
  arr.push_back(a);
  std::cin >> a;
  arr.push_back(a);
  for (int i = 2; i <= n - 1; i++) {
    arr.push_back((arr[i - 1] * cOne + arr[i - 2] * cTwo) % (cThree + cFour));
  }
  int answer = QuickSelect(k, arr);
  std::cout << answer;
}