#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <vector>
const long long cBig = 1e9 + 7;
const long long cB = 17;
// RINGS, FIELDS
class Residue {
 private:
  size_t number_;
  size_t mod_ = cBig;

 public:
  Residue() { number_ = 0; }
  Residue(int num) {
    if (num < 0) {
      number_ = mod_ - static_cast<size_t>(abs(num) % static_cast<int>(mod_));
    } else {
      number_ = (static_cast<size_t>(num) % mod_);
    }
  }
  bool operator==(const Residue& another) const {
    return (number_ == another.number_);
  }
  bool operator!=(const Residue& another) const {
    return (number_ != another.number_);
  }
  Residue& operator+=(const Residue& another) {
    number_ += another.number_;
    number_ %= mod_;
    return *this;
  }
  Residue& operator-=(const Residue& another) {
    number_ += 2 * mod_;
    number_ -= another.number_;
    number_ %= mod_;
    return *this;
  }
  Residue& operator*=(const Residue& another) {
    number_ *= another.number_;
    number_ %= mod_;
    return *this;
  }
  Residue Pow(Residue& baze, const Residue& indicator) {
    if (indicator.number_ == 0) {
      return Residue(1);
    }
    if (indicator.number_ == 1) {
      return baze;
    }
    if (indicator.number_ % 2 == 0) {
      Residue g = Pow(baze, Residue(indicator.number_ / 2));
      g *= g;
      return g;
    }
    Residue g = Pow(baze, Residue(indicator.number_ / 2));
    g *= g;
    g *= baze;
    return g;
  }

  explicit operator int() const { return static_cast<int>(number_); }
};

Residue operator+(const Residue& one, const Residue& another) {
  Residue a(one);
  a += another;
  return a;
}
Residue operator-(const Residue& one, const Residue& another) {
  Residue a(one);
  a -= another;
  return a;
}
Residue operator*(const Residue& one, const Residue& another) {
  Residue a(one);
  a *= another;
  return a;
}
std::ostream& operator<<(std::ostream& out, const Residue& a) {
  out << static_cast<int>(a);
  return out;
}

class Matrix {
 public:
  size_t n = 0;
  size_t m = 0;
  std::vector<std::vector<Residue>> sod;
  Matrix() = default;
  Matrix(size_t n_g, size_t m_g)
      : n(n_g),
        m(m_g),
        sod(std::vector<std::vector<Residue>>(
            n_g, std::vector<Residue>(m_g, Residue(0)))) {}
  void Create() {
    for (size_t i = 0; i != n; ++i) {
      for (size_t j = 0; j != m; ++j) {
        if (abs((int)i - (int)j) < 2) {
          sod[i][j] = 1;
        }
      }
    }
  }
  void Ed() {
    for (size_t i = 0; i != n; ++i) {
      for (size_t j = 0; j != m; ++j) {
        if (abs((int)i - (int)j) == 0) {
          sod[i][j] = 1;
        }
      }
    }
  }

  Matrix operator*(const Matrix& two) const {
    Matrix make(n, two.m);
    for (size_t i = 0; i != n; ++i) {
      for (size_t l = 0; l != two.m; ++l) {
        for (size_t r = 0; r != m; ++r) {
          make.sod[i][l] += sod[i][r] * two.sod[r][l];
        }
      }
    }
    return make;
  }
  bool operator==(const Matrix& another) const {
    for (size_t i = 0; i != n; ++i) {
      for (size_t j = 0; j != m; ++j) {
        if (sod[i][j] != another.sod[i][j]) {
          return false;
        }
      }
    }
    return true;
  }
};

std::ostream& operator<<(std::ostream& out, const Matrix& matrix) {
  for (size_t i = 0; i != matrix.n; ++i) {
    for (size_t j = 0; j != matrix.n; ++j) {
      out << matrix.sod[i][j] << " ";
    }
    out << "\n";
  }
  return out;
}
Matrix Pow(Matrix m, long long pow) {
  Matrix answer = Matrix(m.n, m.m);
  answer.Ed();
  if (pow == 0) {
    return answer;
  }
  Matrix temp = m;
  while (pow != 0) {
    if ((pow & 1) != 0) {
      answer = answer * temp;
      // std::cout << answer << '\n';
    };
    temp = temp * temp;
    pow >>= 1;
  }
  return answer;
}
struct Border {
  long long start;
  long long end;
  long long high;
};

int main() {
  long long n;
  long long k;
  long long d = 0;
  long long c = 0;
  std::cin >> n >> k;
  std::vector<Border> arr(n);
  std::vector<Residue> curpos(cB + 1);
  std::vector<Matrix> m(cB + 1);
  curpos[0] = 1;
  std::vector<Residue> curposn(cB + 1);
  Border in;
  for (long long i = 0; i != n; ++i) {
    std::cin >> in.start >> in.end >> in.high;
    arr[i] = in;
  }
  for (long long i = 0; i != cB + 1; ++i) {
    Matrix a(i, i);
    a.Create();
    m[i] = a;
  }
  for (long long i = 0; i != n; ++i) {
    c = arr[i].high;
    ++c;
    d = arr[i].end - arr[i].start;
    // std::cout << m[c] << d<< '\n';
    Matrix temp = Pow(m[c], d);

    curposn.assign(cB, 0);
    for (int j = 0; j != c; ++j) {
      for (int w = 0; w != c; ++w) {
        curposn[j] += curpos[w] * temp.sod[j][w];
      }
    }

    std::swap(curpos, curposn);
    for (int j = 0; j != cB; ++j) {
      if (j - arr[i].high >= 1) {
        curpos[j] = 0;
      };
    }
  }
  std::cout << curpos[0] << '\n';
}
