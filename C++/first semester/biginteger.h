#include <iostream>
#include <string>
#include <vector>

class BigInteger {
 private:
  enum Sign : int { Plus = 1, Minus = -1, Null = 0 };
  // FIELDS
  std::vector<long long> number;
  Sign sign;
  void ChangeSign() {
    if (sign == Plus) {
      sign = Minus;
    } else if (sign == Minus) {
      sign = Plus;
    }
  }

  
  const long long baze = 1e9;
  // FIELDS END

  // INSTRUMENTS
  int BinSearch(const BigInteger& divider, const BigInteger& another) {
    BigInteger save("0");
    save.sign = Plus;
    int left = 1;
    int right = baze;
    while (right > left + 1) {
      int mid = left + (right - left) / 2;
      save = another * BigInteger(mid);
      if (save > divider) {
        right = mid;
      } else {
        left = mid;
      }
    }
    return left;
  }
  void GoToString(std::string& str) const {
    std::string work;
    for (size_t i = 0; i != number.size(); i++) {
      work = std::to_string(number[number.size() - i - 1]);
      while (i != 0 && work.size() < 9) {
        work = '0' + work;
      }
      str += work;
    }
  }
  bool Equal(const BigInteger& another) const {
    if (this == &another) {
      return true;
    }
    if (sign == another.sign && number.size() == another.number.size()) {
      for (size_t i = 0; i != number.size(); i++) {
        if (number[number.size() - i - 1] !=
            another.number[number.size() - i - 1]) {
          return false;
        }
      }
      return true;
    }
    return false;
  }
  bool Less(const BigInteger& another) const {
    if (this == &another || sign > another.sign) {
      return false;
    }
    if (sign < another.sign) {
      return true;
    }
    if (number.size() != another.number.size()) {
      return (sign + another.sign == 2 &&
              number.size() < another.number.size()) ||
             (sign + another.sign == -2 &&
              number.size() > another.number.size());
    }
    for (size_t i = 0; i != number.size(); i++) {
      if (number[number.size() - i - 1] !=
          another.number[number.size() - i - 1]) {
        return (sign + another.sign == 2 &&
                number[number.size() - i - 1] <
                    another.number[number.size() - i - 1]) ||
               (sign + another.sign == -2 &&
                number[number.size() - i - 1] >
                    another.number[number.size() - i - 1]);
      }
    }
    return false;
  }
  void KillZeros() {
    while (number.size() != 1 && number[number.size() - 1] == 0) {
      number.pop_back();
    }
    if (number[0] == 0 && number.size() == 1) {
      sign = Null;
    }
  }
  // INSTRUMENTS END

  // OPERATIONS MECHANISM
  BigInteger& sum(const BigInteger another) {
    if (number.size() < another.number.size()) {
      number.resize(another.number.size());
    }
    number.push_back(0);
    for (size_t i = 0; i != another.number.size(); i++) {
      if (number[i] + another.number[i] >= baze) {
        number[i + 1] += 1;
      }
      number[i] = (number[i] + another.number[i]) % baze;
    }
    for (size_t i = 0; i != number.size(); i++) {
      if (number[i] >= baze) {
        number[i + 1] += 1;
      }
      number[i] = (number[i]) % baze;
    }
    KillZeros();
    return *this;
  }
  BigInteger& dif(const BigInteger another) {
    if (babs(*this) >= babs(another)) {
      for (size_t i = 0; i != number.size(); i++) {
        if (i < another.number.size()) {
          number[i] -= another.number[i];
        }
        if (number[i] < 0) {
          number[i] += baze;
          number[i + 1] -= 1;
        }
      }
    } else {
      ChangeSign();
      if (number.size() < another.number.size()) {
        number.resize(another.number.size());
      }
      for (size_t i = 0; i != number.size(); i++) {
        number[i] = another.number[i] - number[i];
        if (number[i] < 0) {
          number[i] += baze;
          if (i + 1 != number.size()) {
            number[i + 1] += 1;
          }
        }
      }
    }
    KillZeros();
    return *this;
  }
  BigInteger multiply(const BigInteger& another) {
    unsigned long long ost = 0;
    unsigned long long remains = 0;
    BigInteger res("0");
    res.number.resize(number.size() + another.number.size());
    for (size_t i = 0; i != number.size(); i++) {
      ost = 0;
      for (size_t j = 0; j != another.number.size(); j++) {
        ost = static_cast<unsigned long long>(number[i]) *
                  static_cast<unsigned long long>(another.number[j]) +
              res.number[i + j];
        res.number[i + j] = ost % baze;
        remains = ost / baze;
        res.number[i + j + 1] += remains;
      }
    }
    res.KillZeros();
    return res;
  }
  BigInteger divide(const BigInteger& another) {
    BigInteger res("0");
    res.number.pop_back();
    BigInteger divider("0");
    divider.sign = Plus;
    divider.number.resize(another.number.size());
    size_t pos = 0;
    for (; pos != another.number.size(); pos++) {
      divider.number[another.number.size() - 1 - pos] =
          number[number.size() - 1 - pos];
    }
    if (divider < another) {
      divider *= baze;
      divider.number[0] = number[number.size() - pos - 1];
      pos += 1;
      divider.KillZeros();
    }
    while (pos != number.size() + 1) {
      divider.sign = Plus;
      if (divider.Less(another)) {
        res.number.push_back(0);
      } else {
        int calc = BinSearch(divider, another);
        BigInteger another_save = calc;
        res.number.push_back(calc);
        divider -= another * another_save;
        divider.KillZeros();
      }
      if (pos != number.size()) {
        divider *= baze;
        divider.number[0] = number[number.size() - pos - 1];
        divider.KillZeros();
      }
      pos += 1;
    }
    std::reverse(res.number.begin(), res.number.end());
    return res;
  }
  // OPERATIONS MECHANISM END

 public:
  // CONSTRUCTORS
  BigInteger() {
    number = {0};
    sign = Null;
  }
  BigInteger(const BigInteger& primal) {
    number = primal.number;
    sign = primal.sign;
  }
  BigInteger(const long long& c) {
    long long ingoing = c;
    if (ingoing > 0) {
      sign = Plus;
    } else if (ingoing < 0) {
      sign = Minus;
      ingoing = std::abs(ingoing);
    } else {
      sign = Null;
      number.push_back(0);
      return;
    }
    while (ingoing != 0) {
      long long w = ingoing % baze;
      number.push_back(w);
      ingoing /= baze;
    }
    KillZeros();
  }
  BigInteger(const std::string& c) : sign(Plus) {
    std::string ingoing = c;
    if (ingoing[0] == '-') {
      sign = Minus;
      ingoing[0] = '0';
    } else if (ingoing[0] == '+') {
      ingoing[0] = '0';
    }
    std::string working_with;
    while (ingoing != "") {
      size_t begin = 0;
      if (ingoing.size() >= 9) {
        begin = ingoing.size() - 9;
      }
      working_with = ingoing.substr(begin, 9);
      number.push_back(std::abs(atoll(working_with.c_str())));
      for (int i = 0; i != 9 && ingoing != ""; i++) {
        ingoing.pop_back();
      }
    }
    KillZeros();
  }
  ~BigInteger() {
    sign = Null;
    number.clear();
  }
  // CONSTRUCTORS END
  // EQUAL
  BigInteger& operator=(const BigInteger& another) {
    if (this == &another) {
      return *this;
    }
    sign = another.sign;
    number = another.number;
    return *this;
  }
  // EQUAL END

  // STRING
  std::string toString() const {
    std::string ready;
    if (sign == -1) {
      ready += '-';
    }
    GoToString(ready);
    return ready;
  }
  // STRING END

  // BOOL LOGIC, COMPARE
  explicit operator bool() const {
    return (number[0] != 0 || number.size() != 1);
  }
  // EQUAL
  bool operator==(const BigInteger& another) { return (Equal(another)); }
  bool operator==(int another) { return (Equal(BigInteger(another))); }
  bool operator==(long long another) { return (Equal(another)); }
  // EQUAL END

  // NOT EQUAL
  bool operator!=(const BigInteger& another) { return !(*this == another); }
  bool operator!=(int another) { return !(*this == (BigInteger(another))); }
  bool operator!=(long long another) {
    return !(*this == (BigInteger(another)));
  }
  // NOT EQUAL END

  // LESS
  bool operator<(const BigInteger& another) { return (Less(another)); }
  bool operator<(int another) { return (Less(BigInteger(another))); }
  bool operator<(long long another) { return (Less(BigInteger(another))); }
  // LESS END

  // MORE EQUAL
  bool operator>=(const BigInteger& another) { return !(*this < another); }
  bool operator>=(int another) { return !(*this < BigInteger(another)); }
  bool operator>=(long long another) { return !(*this < BigInteger(another)); }
  // MORE EQUAL END

  // LESS EQUAL
  bool operator<=(const BigInteger& another) {
    return (*this == another || *this < another);
  }
  bool operator<=(int another) {
    return (*this == BigInteger(another) || *this < BigInteger(another));
  }
  bool operator<=(long long another) {
    return (*this == BigInteger(another) || *this < BigInteger(another));
  }
  // LESS EQUAL END

  // MORE
  bool operator>(const BigInteger& another) {
    return (*this != another && !(*this < another));
  }
  bool operator>(int another) {
    return (*this != BigInteger(another) && !(*this < BigInteger(another)));
  }
  bool operator>(long long another) {
    return (*this != BigInteger(another) && !(*this < BigInteger(another)));
  }
  // MORE END

  // BOOL LOGIC, COMPARE END

  explicit operator int() const {
    if (sign == -1) {
      return static_cast<int>(-number[0]);
    } else {
      return static_cast<int>(number[0]);
    }
  }
  BigInteger babs(const BigInteger& n) const {
    if (n.Equal(BigInteger(0))) {
      return BigInteger(0);
    }
    BigInteger n_copy = n;
    if (n_copy < 0) {
      return -n_copy;
    }
    return n_copy;
  }
  // PLUS
  BigInteger& operator+=(const BigInteger& another) {
    if (sign == 0) {
      number = another.number;
      sign = another.sign;
      return *this;
    }
    if (another.sign == 0) {
      return *this;
    }
    if (sign * another.sign > 0) {
      return sum(another);
    }
    return dif(another);
  }
  BigInteger operator+(const BigInteger& one) const {
    BigInteger copy = *this;
    return (copy += one);
  }
  BigInteger operator+(int one) const {
    BigInteger copy = *this;
    return (copy += one);
  }
  // PLUS END

  // MINUS
  BigInteger& operator-=(const BigInteger& another) {
    if (sign == 0) {
      number = another.number;
      sign = another.sign;
      ChangeSign();
      return *this;
    }
    if (another.sign == 0) {
      return *this;
    }
    if (sign * another.sign > 0) {
      return dif(another);
    }
    return sum(another);
  }
  BigInteger operator-(const BigInteger& one) const {
    BigInteger copy = *this;
    return (copy -= one);
  }
  BigInteger operator-(int one) const {
    BigInteger copy = *this;
    return (copy -= one);
  }
  // MINUS END

  // MULTIPLY
  BigInteger& operator*=(const BigInteger& another) {
    if (sign * another.sign == 0) {
      sign = Null;
      *this = 0;
      return *this;
    }
    Sign save;
    if (sign * another.sign == -1) {
      save = Minus;
    } else {
      save = Plus;
    }
    *this = multiply(another);
    sign = save;
    return *this;
  }
  BigInteger operator*(const BigInteger& one) const {
    BigInteger copy = *this;
    return (copy *= one);
  }
  BigInteger operator*(int one) const {
    BigInteger copy = *this;
    return (copy *= one);
  }
  // MULTIPLY END

  // DEVIDE
  BigInteger& operator/=(const BigInteger& another) {
    if (*this == another) {
      *this = 1;
      return *this;
    }
    if (sign == 0 || babs(*this) < babs(another)) {
      sign = Null;
      *this = 0;
      return *this;
    }
    Sign save;
    if (sign * another.sign == -1) {
      save = Minus;
    } else {
      save = Plus;
    }
    *this = divide(babs(another));
    sign = save;
    return *this;
  }
  BigInteger operator/(const BigInteger& one) const {
    BigInteger copy = *this;
    return (copy /= one);
  }
  BigInteger operator/(int one) const {
    BigInteger copy = *this;
    return (copy /= one);
  }
  // DEVIDE END

  // MOD
  BigInteger& operator%=(const BigInteger& another) {
    *this -= another * (*this / another);
    return *this;
  }
  BigInteger operator%(const BigInteger& one) const {
    BigInteger copy = *this;
    return (copy %= one);
  }
  BigInteger operator%(int one) const {
    BigInteger copy = *this;
    return (copy %= one);
  }
  // MOD END

  // INKREMENTS AND DEKREMENTS
  BigInteger& operator++() {
    *this += 1;
    return *this;
  }
  BigInteger operator++(int) {
    BigInteger temper = *this;
    *this += 1;
    return temper;
  }
  BigInteger& operator--() {
    *this -= 1;
    return *this;
  }
  BigInteger operator--(int) {
    BigInteger temper = *this;
    *this -= 1;
    return temper;
  }
  BigInteger operator-() {
    BigInteger temper = *this;
    temper.ChangeSign();
    return temper;
  }
  // INKREMENTS AND DEKREMENTS END
};
BigInteger operator""_bi(const char* num, size_t size) {
  std::string temp = num;
  temp.resize(size);
  return BigInteger(num);
}
BigInteger operator""_bi(unsigned long long int num) {
  std::string temp = std::to_string(num);
  return BigInteger(num);
}
// STREAM
std::istream& operator>>(std::istream& in, BigInteger& a) {
  std::string num_str;
  in >> num_str;
  a = num_str;
  return in;
}
std::ostream& operator<<(std::ostream& out, const BigInteger& a) {
  std::string num_str = a.toString();
  out << num_str;
  return out;
}
// STREAM END
// REMAIN LOGIC
bool operator==(int another, const BigInteger& aother) {
  return (BigInteger(another) == aother);
}
bool operator==(long long another, const BigInteger& aother) {
  return (BigInteger(another) == aother);
}
bool operator==(const BigInteger& one, const BigInteger& two) {
  return (BigInteger(one) == BigInteger(two));
}
bool operator==(const BigInteger& one, int two) {
  return (BigInteger(one) == BigInteger(two));
}
bool operator!=(int another, const BigInteger& aother) {
  return (BigInteger(another) != aother);
}
bool operator!=(long long another, const BigInteger& aother) {
  return (BigInteger(another) != aother);
}
bool operator!=(const BigInteger& one, const BigInteger& two) {
  return (BigInteger(one) != BigInteger(two));
}
bool operator!=(const BigInteger& one, int two) {
  return (BigInteger(one) != BigInteger(two));
}
bool operator<(int another, const BigInteger& aother) {
  return (BigInteger(another) < aother);
}
bool operator<(long long another, const BigInteger& aother) {
  return (BigInteger(another) < aother);
}
bool operator<(const BigInteger& one, const BigInteger& two) {
  return (BigInteger(one) < BigInteger(two));
}
bool operator<(const BigInteger& one, int two) {
  return (BigInteger(one) < BigInteger(two));
}
bool operator>=(int another, const BigInteger& aother) {
  return (BigInteger(another) >= aother);
}
bool operator>=(long long another, const BigInteger& aother) {
  return (BigInteger(another) >= aother);
}
bool operator>=(const BigInteger& one, const BigInteger& two) {
  return (BigInteger(one) >= BigInteger(two));
}
bool operator>=(const BigInteger& one, int two) {
  return (BigInteger(one) >= BigInteger(two));
}
bool operator<=(int another, const BigInteger& aother) {
  return (BigInteger(another) <= aother);
}
bool operator<=(long long another, const BigInteger& aother) {
  return (BigInteger(another) <= aother);
}
bool operator<=(const BigInteger& one, const BigInteger& two) {
  return (BigInteger(one) <= BigInteger(two));
}
bool operator<=(const BigInteger& one, int two) {
  return (BigInteger(one) <= BigInteger(two));
}
bool operator>(int another, const BigInteger& aother) {
  return (BigInteger(another) > aother);
}
bool operator>(long long another, const BigInteger& aother) {
  return (BigInteger(another) > aother);
}
bool operator>(const BigInteger& one, const BigInteger& two) {
  return (BigInteger(one) > BigInteger(two));
}
bool operator>(const BigInteger& one, int two) {
  return (BigInteger(one) > BigInteger(two));
}
// LOGIC END
// REMAIN OPS
BigInteger operator+(int one, const BigInteger& two) {
  BigInteger a(one);
  return (a += two);
}
BigInteger operator-(int one, const BigInteger& two) {
  BigInteger a(one);
  return (a -= two);
}
BigInteger operator*(int one, const BigInteger& two) {
  BigInteger a(one);
  return (a *= two);
}
BigInteger operator/(long long one, const BigInteger& two) {
  BigInteger a(one);
  return (a /= two);
}
BigInteger operator%(int one, const BigInteger& two) {
  BigInteger a(one);
  return (a %= two);
}
// OPS END

class Rational {
 private:
  BigInteger p;
  BigInteger q;
  void Order() {
    if (q < 0) {
      p = -p;
      q = -q;
    }
    if (p == 0) {
      q = 1;
    }
  }
  void Priming_Eucledean() {
    BigInteger p_c = p;
    if (p_c < 0) {
      p_c *= -1;
    }
    BigInteger q_c = q;
    if (q_c < 0) {
      q_c *= -1;
    }
    while (p_c * q_c != 0) {
      if (p_c >= q_c) {
        p_c %= q_c;
      } else {
        q_c %= p_c;
      }
    }
    BigInteger mod = p_c + q_c;
    p /= mod;
    q /= mod;
  }

 public:
  Rational() : p(0), q(1) {}
  Rational(const int& num) : p(num), q(1) {}
  Rational(const BigInteger& num) : p(num), q(1) {}
  Rational(const Rational& num) : p(num.p), q(num.q) {}
  Rational& operator=(const Rational& another) {
    if (this == &another) {
      return *this;
    }
    p = another.p;
    q = another.q;
    return *this;
  }
  std::string toString() const {
    Rational c = *this;
    std::string ready = c.p.toString();
    if (c.q.toString() != "1") {
      ready += '/' + c.q.toString();
    }
    return ready;
  }
  std::string asDecimal(size_t precesion = 0) const {
    Rational c = *this;
    std::string ready;
    if (p < 0) {
      ready += '-';
    }
    BigInteger int_part = c.p / c.q;
    ready += int_part.toString();
    BigInteger drob_part = c.p % c.q;
    if (precesion != 0) {
      ready += ".";
    }
    size_t amount = 0;
    if (drob_part < 0) {
      drob_part *= -1;
    }
    while (amount < precesion) {
      if (drob_part == 0) {
        ready += std::string(precesion - amount, '0');
        break;
      }
      if (drob_part < c.q) {
        drob_part *= 10;
      }
      std::string help = (drob_part / c.q).toString();
      amount += help.size();
      ready += help;
      drob_part %= c.q;
    }
    return ready;
  }
  Rational& operator+=(const Rational& another) {
    p = (p * another.q + q * another.p);
    q = q * another.q;
    Order();
    Priming_Eucledean();
    return *this;
  }
  Rational& operator-=(const Rational& another) {
    p = (p * another.q - q * another.p);
    q = q * another.q;
    Order();
    Priming_Eucledean();
    return *this;
  }
  Rational& operator*=(const Rational& another) {
    p *= another.p;
    q *= another.q;
    Order();
    Priming_Eucledean();
    return *this;
  }
  Rational& operator/=(const Rational& another) {
    p *= another.q;
    q *= another.p;
    Order();
    Priming_Eucledean();
    return *this;
  }
  Rational operator-() {
    Rational temper = *this;
    temper.p *= -1;
    temper.Order();
    return temper;
  }
  bool operator==(const Rational& another) const {
    return (p * another.q == q * another.p);
  }
  bool operator!=(const Rational& another) const {
    return (p * another.q != q * another.p);
  }
  bool operator>=(const Rational& another) const {
    return (p * another.q >= q * another.p);
  }
  bool operator<=(const Rational& another) const {
    return (p * another.q <= q * another.p);
  }
  bool operator<(const Rational& another) const {
    return (p * another.q < q * another.p);
  }
  bool operator>(const Rational& another) const {
    return (p * another.q > q * another.p);
  }
  explicit operator double() const {
    std::string temp = this->asDecimal(10);
    double doub = atof(temp.data());
    return doub;
  }
};
Rational operator+(const Rational& one, const Rational& another) {
  Rational a(one);
  return (a += another);
}
Rational operator-(const Rational& one, const Rational& another) {
  Rational a(one);
  return (a -= another);
}
Rational operator*(const Rational& one, const Rational& another) {
  Rational a(one);
  return (a *= another);
}
Rational operator/(const Rational& one, const Rational& another) {
  Rational a(one);
  return (a /= another);
}