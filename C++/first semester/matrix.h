#include <array>
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
        }
        else if (sign == Minus) {
            sign = Plus;
        }
    }

    const long long baze = 1e4;
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
            }
            else {
                left = mid;
            }
        }
        return left;
    }
    void GoToString(std::string& str) const {
        std::string work;
        for (size_t i = 0; i != number.size(); i++) {
            work = std::to_string(number[number.size() - i - 1]);
            while (i != 0 && work.size() < 4) {
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
        }
        else {
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
        BigInteger res("0");
        res.number.resize(number.size() + another.number.size());
        for (size_t i = 0; i != number.size(); i++) {
            ost = 0;
            for (size_t j = 0; j != another.number.size(); j++) {
                ost = static_cast<unsigned long long>(number[i]) *
                    static_cast<unsigned long long>(another.number[j]) +
                    res.number[i + j];
                res.number[i + j] = ost % baze;
                ost /= baze;
                res.number[i + j + 1] += ost;
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
            }
            else {
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
    friend class Rational;
    // OPERATIONS MECHANISM END

public:
    // CONSTRUCTORS
    BigInteger() {
        number = { 0 };
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
        }
        else if (ingoing < 0) {
            sign = Minus;
            ingoing = std::abs(ingoing);
        }
        else {
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
        }
        else if (ingoing[0] == '+') {
            ingoing[0] = '0';
        }
        std::string working_with;
        while (ingoing != "") {
            size_t begin = 0;
            if (ingoing.size() >= 4) {
                begin = ingoing.size() - 4;
            }
            working_with = ingoing.substr(begin, 4);
            number.push_back(std::abs(atoll(working_with.c_str())));
            for (int i = 0; i != 4 && ingoing != ""; i++) {
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
        }
        else {
            return static_cast<int>(number[0]);
        }
    }
    BigInteger babs(const BigInteger& n) const {
        if (n.sign == 0) {
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
        copy += one;
        return copy;
    }
    BigInteger operator+(int one) const {
        BigInteger copy = *this;
        copy += one;
        return copy;
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
        copy -= one;
        return copy;
    }
    BigInteger operator-(int one) const {
        BigInteger copy = *this;
        copy -= one;
        return copy;
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
        }
        else {
            save = Plus;
        }
        *this = multiply(another);
        sign = save;
        return *this;
    }
    BigInteger operator*(const BigInteger& one) const {
        BigInteger copy = *this;
        copy *= one;
        return copy;
    }
    BigInteger operator*(int one) const {
        BigInteger copy = *this;
        copy *= one;
        return copy;
    }
    // MULTIPLY END

    // DEVIDE
    BigInteger& operator/=(const BigInteger& another) {
        if (this == &another) {
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
        }
        else {
            save = Plus;
        }
        *this = divide(babs(another));
        sign = save;
        return *this;
    }
    BigInteger operator/(const BigInteger& one) const {
        BigInteger copy = *this;
        copy /= one;
        return copy;
    }
    BigInteger operator/(int one) const {
        BigInteger copy = *this;
        copy /= one;
        return copy;
    }
    // DEVIDE END

    // MOD
    BigInteger& operator%=(const BigInteger& another) {
        *this -= another * (*this / another);
        return *this;
    }
    BigInteger operator%(const BigInteger& one) const {
        BigInteger copy = *this;
        copy %= one;
        return copy;
    }
    BigInteger operator%(int one) const {
        BigInteger copy = *this;
        copy %= one;
        return copy;
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
    a += two;
    return a;
}
BigInteger operator-(int one, const BigInteger& two) {
    BigInteger a(one);
    a -= two;
    return a;
}
BigInteger operator*(int one, const BigInteger& two) {
    BigInteger a(one);
    a *= two;
    return a;
}
BigInteger operator/(long long one, const BigInteger& two) {
    BigInteger a(one);
    a /= two;
    return a;
}
BigInteger operator%(int one, const BigInteger& two) {
    BigInteger a(one);
    a %= two;
    return a;
}
// OPS END

class Rational {
private:
    BigInteger p;
    BigInteger q;
    void Order() {
        if (p.sign == 0) {
            q = 1;
            return;
        }
        if (q.sign < 0) {
            p.ChangeSign();
            q.ChangeSign();
        }
    }
    void GCDCalc(BigInteger& p_c, BigInteger& q_c) {
        if (p_c.sign * q_c.sign != 0) {
            p_c %= q_c;
            GCDCalc(q_c, p_c);
        }
    }
    void Priming_Eucledean() {
        BigInteger p_c = p;
        if (p_c.sign < 0) {
            p_c.ChangeSign();
        }
        BigInteger q_c = q;
        if (q_c.sign < 0) {
            q_c.ChangeSign();
        }
        GCDCalc(q_c, p_c);
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
        if (this == &another) {
            p = 1;
            q = 1;
            return *this;
        }
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
        return (p * another.q < q* another.p);
    }
    bool operator>(const Rational& another) const {
        return (p * another.q > q * another.p);
    }
    explicit operator double() const {
        std::string temp = this->asDecimal(10);
        double doub = atof(temp.data());
        return doub;
    }
    friend std::ostream& operator<<(std::ostream& out, const Rational& a) {
        out << a.p;
        if (a.q != 1) {
            out << '/' << a.q;
        }
        return out;
    }
};
Rational operator+(const Rational& one, const Rational& another) {
    Rational a(one);
    a += another;
    return a;
}
Rational operator-(const Rational& one, const Rational& another) {
    Rational a(one);
    a -= another;
    return a;
}
Rational operator*(const Rational& one, const Rational& another) {
    Rational a(one);
    a *= another;
    return a;
}
Rational operator/(const Rational& one, const Rational& another) {
    Rational a(one);
    a /= another;
    return a;
}
std::istream& operator>>(std::istream& in, Rational& a) {
    int num;
    in >> num;
    a = Rational(num);
    return in;
}

constexpr bool isPrimeLoop(size_t i, size_t k) {
    return (k * k > i) ? true : (i % k == 0) ? false : isPrimeLoop(i, k + 1);
}

constexpr bool isPrime(size_t i) { return isPrimeLoop(i, 2); }

// RINGS, FIELDS
template <size_t N>
class Residue {
private:
    size_t number_;

public:
    Residue() { number_ = 0; }
    Residue(const int num) {
        if (num < 0) {
            number_ = N - static_cast<size_t>(abs(num) % static_cast<int>(N));
        }
        else {
            number_ = (static_cast<size_t>(num) % N);
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
        number_ %= N;
        return *this;
    }
    Residue& operator-=(const Residue& another) {
        number_ += 2 * N;
        number_ -= another.number_;
        number_ %= N;
        return *this;
    }
    Residue& operator*=(const Residue& another) {
        number_ *= another.number_;
        number_ %= N;
        return *this;
    }
    Residue pow(const Residue& baze, const Residue& indicator) {
        if (indicator.number_ == 0) {
            return Residue(1);
        }
        if (indicator.number_ == 1) {
            return baze;
        }
        if (indicator.number_ % 2 == 0) {
            return pow(baze, Residue(indicator.number_ / 2)) * pow(baze, Residue(indicator.number_ / 2));
        }
        return baze * pow(baze, Residue(indicator.number_ / 2))* pow(baze, Residue(indicator.number_ / 2));
    }
    Residue& operator/=(const Residue& another) {
        static_assert(isPrime(N));
        *this *= pow(another, N - 2);
        return *this;
    }
    explicit operator int() const { return number_; }
};
template <size_t N>
Residue<N> operator+(const Residue<N>& one, const Residue<N>& another) {
    Residue<N> a(one);
    a += another;
    return a;
}
template <size_t N>
Residue<N> operator-(const Residue<N>& one, const Residue<N>& another) {
    Residue<N> a(one);
    a -= another;
    return a;
}
template <size_t N>
Residue<N> operator*(const Residue<N>& one, const Residue<N>& another) {
    Residue<N> a(one);
    a *= another;
    return a;
}
template <size_t N>
Residue<N> operator/(const Residue<N>& one, const Residue<N>& another) {
    Residue<N> a(one);
    a /= another;
    return a;
}
template <size_t N>
std::ostream& operator<<(std::ostream& out, const Residue<N>& a) {
    out << static_cast<int>(a);
    return out;
}
template <size_t N>
std::istream& operator>>(std::istream& in, Residue<N>& a) {
    int num;
    in >> num;
    a = Residue<N>(num);
    return in;
}

constexpr bool isSquare(size_t n, size_t m) { return (n == m); }

template <size_t N, size_t M, typename Field = Rational>
class Matrix {
protected:
    friend Matrix<N, M / 2, Field>;
    std::array<std::array<Field, M>, N> matrix_;
    void MinusRow(size_t decreased, size_t by, const Field& multy) {
        for (size_t j = 0; j < M; ++j) {
            matrix_[decreased][j] -= matrix_[by][j] * multy;
        }
    }
    void MultiplyRow(size_t i, const Field& number) {
        for (size_t j = 0; j < M; ++j) {
            matrix_[i][j] *= number;
        }
    }
    void GaussMethod(bool type) {
        size_t column = 0;
        size_t i = 0;
        while (i != N && column != M) {
            bool bad_column = true;
            if (matrix_[i][column] == 0) {
                for (size_t j = i + 1; j < N; ++j) {
                    if (matrix_[j][column] != 0) {
                        std::swap(matrix_[j], matrix_[i]);
                        if (type) {
                            MultiplyRow(i, -1);
                        }
                        bad_column = false;
                        break;
                    }
                }
            }
            else {
                bad_column = false;
            }
            if (bad_column && type) {
                return;
            }
            for (size_t j = i + 1; j < N; ++j) {
                if (matrix_[j][column] != 0) {
                    MinusRow(j, i, matrix_[j][column] / matrix_[i][column]);
                }
            }
            ++column;
            ++i;
        }
    }

public:
    Matrix() {
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                matrix_[i][j] = static_cast<Field>(0);
            }
        }
    }
    Matrix(const std::vector<std::vector<Field>>& one) {
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                matrix_[i][j] = one[i][j];
            }
        }
    }
    Matrix(const std::initializer_list<std::initializer_list<Field>> one) {
        size_t i = 0;
        for (const auto& r : one) {
            size_t j = 0;
            for (const auto& c : r) {
                matrix_[i][j] = c;
                ++j;
            }
            ++i;
        }
    }
    // PLUS
    Matrix& operator+=(const Matrix& another) {
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                matrix_[i][j] += another.matrix_[i][j];
            }
        }
        return *this;
    }
    // PLUS END
    // MINUS
    Matrix& operator-=(const Matrix& another) {
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                matrix_[i][j] -= another.matrix_[i][j];
            }
        }
        return *this;
    }
    // MINUS END 
    // MULTIPLY BY NUMBER
    Matrix& operator*=(const Field& number) {
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                matrix_[i][j] *= number;
            }
        }
        return *this;
    }
    Matrix& operator*=(const Matrix<M, M>& other) {
        return *this = *this * other;
    }
    // MULTIPLY BY NUMBER END

    // TRACE
    Field trace() const {
        static_assert(isSquare(N, M));
        Field sum = 0;
        for (size_t i = 0; i != N; ++i) {
            sum += matrix_[i][i];
        }
        return sum;
    }
    // TRACE END

    // DET
    Field det() const {
        static_assert(isSquare(N, M));
        Matrix<N, M, Field> copy = *this;
        Field determinator = 1;
        copy.GaussMethod(true);
        for (size_t i = 0; i != N; ++i) {
            determinator *= copy[i][i];
        }
        return determinator;
    }
    // DET END
    // TRANCEPOSE
    Matrix<M, N, Field> transposed() const {
        Matrix<M, N, Field> transopsed_copy;
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                transopsed_copy[j][i] = matrix_[i][j];
            }
        }
        return transopsed_copy;
    }
    // TRANCEPOSE END
    // RANK
    size_t rank() const {
        size_t rank = 0;
        Matrix<N, M, Field> copy = *this;
        copy.GaussMethod(false);
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                if (copy.matrix_[i][j] != 0) {
                    ++rank;
                    break;
                }
            }
        }
        return rank;
    }
    // RANK END
    // INVERT
    void invert() {
        static_assert(isSquare(N, M));
        Matrix<N, 2 * M, Field> extended;
        for (size_t i = 0; i != N; ++i) {
            extended[i][M + i] = 1;
            for (size_t j = 0; j != M; ++j) {
                extended[i][j] = matrix_[i][j];
            }
        }
        extended.GaussMethod(false);
        extended.MultiplyRow(N - 1, Field(1) / extended[N - 1][N - 1]);
        for (size_t i = 1; i != N; ++i) {
            for (size_t j = i + 1; j != N + 1; ++j) {
                Field num = extended[N - j][N - i];
                extended.MinusRow(N - j, N - i, num);
            }
            extended.MultiplyRow(N - i - 1,
                (Field(1) / extended[N - i - 1][N - i - 1]));
        }
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                matrix_[i][j] = extended[i][j + M];
            }
        }
    }
    // INVERT END
    Matrix inverted() const {
        Matrix copy(*this);
        copy.invert();
        return copy;
    }
    bool operator==(const Matrix& another) const {
        for (size_t i = 0; i != N; ++i) {
            for (size_t j = 0; j != M; ++j) {
                if (matrix_[i][j] != another.matrix_[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
    bool operator!=(const Matrix& another) const { return !(*this == another); }

    std::array<Field, M> getRow(unsigned index) const { return matrix_[index]; }
    std::array<Field, N> getColumn(unsigned index) const {
        std::array<Field, N> column;
        for (size_t i = 0; i != N; ++i) {
            column[i] = matrix_[i][index];
        }
        return column;
    }

    std::array<Field, M>& operator[](const size_t index) {
        return matrix_[index];
    }
    const std::array<Field, M>& operator[](const size_t index) const {
        return matrix_[index];
    }

    // MATRIX MULTIPLY
    template <size_t L>
    Matrix<N, L, Field> operator*(const Matrix<M, L, Field>& two) const {
        Matrix<N, L, Field> make;
        for (size_t n = 0; n != N; ++n) {
            for (size_t l = 0; l != L; ++l) {
                for (size_t m = 0; m != M; ++m) {
                    make[n][l] += matrix_[n][m] * two[m][l];
                }
            }
        }
        return make;
    }
    // MATRIX MULTIPLY END
};
template <size_t N, size_t M, typename Field = Rational>
Matrix<N, M, Field> operator*(const Matrix<N, M, Field>& matrix,
    const Field& number) {
    Matrix<N, M, Field> a(matrix);
    a *= number;
    return a;
}
template <size_t N, size_t M, typename Field = Rational>
Matrix<N, M, Field> operator*(const Field& number,
    const Matrix<N, M, Field>& matrix) {
    return matrix * number;
}
template <size_t N, size_t M, typename Field = Rational>
std::ostream& operator<<(std::ostream& out, const Matrix<N, M, Field>& matrix) {
    for (size_t i = 0; i != N; ++i) {
        for (size_t j = 0; j != M; ++j) {
            out << matrix[i][j] << " ";
        }
        out << "\n";
    }
    return out;
}
template <size_t N, size_t M, typename Field = Rational>
Matrix<N, M, Field> operator+(const Matrix<N, M, Field>& one,
    const Matrix<N, M, Field>& another) {
    Matrix<N, M, Field> a(one);
    a += another;
    return a;
}
template <size_t N, size_t M, typename Field = Rational>
Matrix<N, M, Field> operator-(const Matrix<N, M, Field>& one,
    const Matrix<N, M, Field>& another) {
    Matrix<N, M, Field> a(one);
    a -= another;
    return a;
}
template <size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;