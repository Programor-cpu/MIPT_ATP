#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int Bitconjunction(int x, int y) { return x & y; }
int Bitdisjunction(int x, int y) { return x | y; }

class FenwickTreeTwoDimensional {
 public:
  int n;
  std::vector<std::vector<int>> matrix;
  FenwickTreeTwoDimensional(int n)
      : n(n),
        matrix(std::vector<std::vector<int>>(n, std::vector<int>(n, 0))) {}
  void Add(const int& x, const int& y) {
    int xw = x;
    while (xw < n) {
      int yw = y;
      while (yw < n) {
        matrix[yw][xw] += 1;
        yw = Bitdisjunction(yw + 1, yw);
      }
      xw = Bitdisjunction(xw + 1, xw);
    }
  }
  int Get(const int& x, const int& y) {
    int s = 0;
    int xw = x;
    while (xw > -1) {
      int yw = y;
      while (yw > -1) {
        s += matrix[yw][xw];
        yw = Bitconjunction(yw + 1, yw);
        yw -= 1;
      }
      xw = Bitconjunction(xw + 1, xw);
      xw -= 1;
    }
    return s;
  }
};

int main() {
  int n;
  std::cin >> n;
  FenwickTreeTwoDimensional stepanov(n);
  int q;
  int x;
  int y;
  int x_low;
  int y_low;
  std::string ingoing;
  std::cin >> q;
  while (q != 0) {
    q--;
    std::cin >> ingoing;
    if (ingoing == "ADD") {
      std::cin >> x >> y;
      x--;
      y--;
      stepanov.Add(x, y);
    } else {
      std::cin >> x_low >> y_low >> x >> y;
      x_low--;
      y_low--;
      x--;
      y--;
      if (x_low > x) {
        std::swap(x_low, x);
      }
      if (y_low > y) {
        std::swap(y_low, y);
      }
      std::cout << stepanov.Get(x, y) - stepanov.Get(x_low - 1, y) -
                       stepanov.Get(x, y_low - 1) +
                       stepanov.Get(x_low - 1, y_low - 1)
                << '\n';
    }
  }
}