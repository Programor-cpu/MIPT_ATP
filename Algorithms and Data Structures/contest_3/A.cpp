#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

const int cBig = 2147483647;

void Fillmp(std::vector<int>& mp, int n) {
  int power = 0;
  mp[0] = 0;
  for (int i = 0; i != n; i++) {
    if ((int)std::pow(2, power + 1) - 1 < i) {
      power += 1;
    }
    mp[i + 1] = power;
  }
}

void CalcSparse(std::vector<std::vector<int>>& sparse,
                std::vector<std::vector<int>>& coords, int n,
                int another_size) {
  for (int i = 0; i != another_size - 1; i++) {
    for (int j = 0; (int)std::pow(2, i + 1) + j - 1 != n; j++) {
      int jw = j + (int)std::pow(2, i);
      sparse[i + 1][j] = std::min(sparse[i][j], sparse[i][jw]);
      if (sparse[i + 1][j] == sparse[i][j]) {
        coords[i + 1][j] = coords[i][j];
      } else {
        coords[i + 1][j] = coords[i][jw];
      }
    }
  }
}

int Conclusion(std::vector<std::vector<int>>& sparse,
               const std::vector<int>& mp, int left_border, int right_border) {
  if (left_border == right_border) {
    return cBig;
  }
  int final_power = mp[right_border - left_border];
  int two_power = std::pow(2, final_power);
  int result = std::min(sparse[final_power][right_border - two_power],
                        sparse[final_power][left_border]);
  return result;
}

int main() {
  std::vector<int> numbers;
  int n;
  int left;
  int right;
  std::cin >> n;
  std::vector<int> mp(1 + n);
  Fillmp(mp, n);
  int another_size = 0;
  while (n >= (int)std::pow(2, another_size + 1)) {
    another_size += 1;
  }
  another_size++;
  std::vector<std::vector<int>> sparse(another_size, std::vector<int>(n, 0));
  int q;
  std::cin >> q;
  for (int i = 0; i != n; i++) {
    int ingoing;
    std::cin >> ingoing;
    numbers.push_back(ingoing);
  }
  std::vector<std::vector<int>> coords = sparse;
  for (int i = 0; i != n; i++) {
    coords[0][i] = i;
    sparse[0][i] = numbers[i];
  }
  CalcSparse(sparse, coords, n, another_size);
  while (q != 0) {
    q--;
    std::cin >> left;
    std::cin >> right;
    int power_of_two = std::pow(2, mp[right - left + 1]);
    int m;
    if (numbers[coords[mp[right - left + 1]][left - 1]] <=
        numbers[coords[mp[right - left + 1]][right - power_of_two]]) {
      m = coords[mp[right - left + 1]][left - 1];
    } else {
      m = coords[mp[right - left + 1]][right - power_of_two];
    }
    int second_st = std::min(Conclusion(sparse, mp, left - 1, m),
                             Conclusion(sparse, mp, m + 1, right));
    std::cout << second_st << '\n';
  }
}