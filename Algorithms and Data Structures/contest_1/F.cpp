#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cGen = 8;

unsigned int NextRand24(unsigned int& a, unsigned int& b, unsigned int& cur) {
  cur = cur * a + b;
  return cur >> cGen;
}
unsigned int NextRand32(unsigned int& a, unsigned int& b, unsigned int& cur) {
  unsigned long x = NextRand24(a, b, cur);
  unsigned long y = NextRand24(a, b, cur);
  return (x << cGen) ^ y;
}

int main() {
  int n;
  unsigned int a;
  unsigned int b;
  unsigned int cur = 0;
  std::vector<long long int> coords;
  std::cin >> n;
  long long int s = 0;
  std::cin >> a;
  std::cin >> b;
  long long int mid = n / 2;
  for (int i = 0; i < n; i++) {
    long long int w = NextRand32(a, b, cur);
    coords.push_back(w);
  }
  nth_element(coords.begin(), coords.begin() + mid, coords.end());
  for (int i = 0; i < n; i++) {
    s += std::llabs(coords[i] - coords[mid]);
  }
  std::cout << s;
}