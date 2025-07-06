#include <cmath>
#include <iostream>

const int cSign = 20;
const long double cDelta = 0.000000000000001;
const long double cLeftGr = -100000000.00;
const long double cRightGr = 100000000.00;

long double BinarySearchForRoot(int a, int b, int c, int d) {
  if (a < 0) {
    d = -d;
    c = -c;
    b = -b;
    a = -a;
  }
  long double l = cLeftGr;
  long double r = cRightGr;
  long double m = 0;
  while (r > l + cDelta) {
    m = (r + l) / 2;
    if (a * pow(m, 3) + b * pow(m, 2) + c * m + d >= 0) {
      r = m;
    } else {
      l = m;
    }
  }
  return l;
}

int main() {
  int a;
  int b;
  int c;
  int d;
  std::cin >> a;
  std::cin >> b;
  std::cin >> c;
  std::cin >> d;
  std::cout.precision(cSign);
  std::cout << BinarySearchForRoot(a, b, c, d);
}