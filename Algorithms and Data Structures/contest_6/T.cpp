#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const int cNum = 48;
const int cPat = 5;

void Multiply(std::vector<std::vector<int>>& ans,
              const std::vector<std::vector<int>>& one,
              const std::vector<std::vector<int>>& two, long long mod) {
  for (size_t i = 0; i != one.size(); ++i) {
    for (size_t l = 0; l != two.size(); ++l) {
      ans[i][l] = 0;
      for (size_t r = 0; r != two.size(); ++r) {
        ans[i][l] += one[i][r] * two[r][l];
        ans[i][l] %= mod;
      }
    }
  }
}

void Divide(std::string& number) {
  for (int i = (int)number.size() - 1; i != -1; --i) {
    if ((number[i]) % 2 == 0) {
      number[i] = char((number[i] + cNum) / 2);
    } else {
      if (i != (int)number.size() - 1) {
        number[i + 1] = char((number[i + 1]) + cPat);
      }
      if (i != 0 || number[i] != '1') {
        number[i] = char((number[i] + cNum) / 2);
      } else {
        number.erase(0, 1);
      }
    }
  }
}

void Pow(std::vector<std::vector<int>>& ans,
         const std::vector<std::vector<int>>& start,
         std::vector<std::vector<int>>& one, std::string& pow, const int& mod) {
  if (pow == "1") {
    ans = start;
    return;
  }
  char c = pow[pow.length() - 1];
  if (c == '0' || c == '2' || c == '4' || c == '6' || c == '8') {
    Divide(pow);
    Pow(ans, start, one, pow, mod);
    one = ans;
    Multiply(ans, one, one, mod);
    return;
  }
  Divide(pow);
  Pow(ans, start, one, pow, mod);
  one = ans;
  Multiply(ans, one, one, mod);
  one = ans;
  Multiply(ans, one, start, mod);
}

std::vector<int> Tobits(int num, int n) {
  std::vector<int> answer(n, 0);
  long long i = 1;
  while (num != 0) {
    answer[n - i] = num % 2;
    ++i;
    num /= 2;
  }
  return answer;
}

void Minusone(std::string& number) {
  int i = 1;
  while (number[number.size() - i] == '0' && i != (int)number.size()) {
    number[number.size() - i] = '9';
    ++i;
  }
  if (number[number.size() - i] == '1' && i == (int)number.size()) {
    number.erase(0, 1);
  } else {
    number[number.size() - i] = char(number[number.size() - 1] - 1);
  }
}

int main() {
  std::string n;
  int ans = 0;
  int m;
  int z;
  std::cin >> n >> m >> z;
  if (n == "1") {
    ans = 1;
    while (m != 0) {
      ans *= 2;
      --m;
    }
    ans %= z;
    std::cout << ans;
  } else {
    Minusone(n);
    std::vector<std::vector<int>> matrix(1 << m, std::vector<int>(1 << m, 0));
    for (int i = 0; i != (1 << m); ++i) {
      for (int j = 0; j != (1 << m); ++j) {
        std::vector<int> one = Tobits(i, m);
        std::vector<int> two = Tobits(j, m);
        bool flag = true;
        for (int z = 1; z != m; ++z) {
          if ((one[z] + one[z - 1] + two[z] + two[z - 1]) % 4 == 0) {
            flag = false;
            break;
          }
        }
        if (flag) {
          matrix[i][j] = 1;
        }
      }
    }
    std::vector<std::vector<int>> str(1, std::vector<int>(1 << m, 1));
    std::vector<std::vector<int>> answer(1, std::vector<int>(1 << m, 0));
    std::vector<std::vector<int>> final = matrix;
    std::vector<std::vector<int>> one(1 << m, std::vector<int>(1 << m));
    Pow(final, matrix, one, n, z);
    Multiply(answer, str, final, z);
    for (int i = 0; i != (1 << m); ++i) {
      ans += answer[0][i];
      ans %= z;
    }
    std::cout << ans;
  }
}