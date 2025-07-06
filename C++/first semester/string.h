#include <iostream>
#include <cstring>
#include <algorithm>

const size_t cMultiply = 2;


class String {
public:
    String() : info_(new char[1]), size_of_(0), capacity_of_(0) {
        info_[size_of_] = '\0';
    }

    String(char build) : info_(new char[2]), size_of_(1), capacity_of_(1) {
        info_[0] = build;
        info_[size_of_] = '\0';
    }

    String(size_t length, char end = '\0') :
        info_(new char[length + 1]),
        size_of_(length),
        capacity_of_(length) {
        memset(info_, end, size_of_);
        info_[size_of_] = '\0';
    }

    String(const char* another) :
        info_(new char[strlen(another) + 1]),
        size_of_(strlen(another)),
        capacity_of_(strlen(another)) {
        memcpy(info_, another, size_of_);
        info_[size_of_] = '\0';
    }

    String(const String& another) :
        info_(new char[another.size_of_ + 1]),
        size_of_(another.size_of_),
        capacity_of_(another.size_of_) {
        memcpy(info_, another.info_, size_of_);
        info_[size_of_] = '\0';
    }

    String& operator=(const String& another) {
        if (&another == this) {
            return *this;
        }
        size_of_ = another.size_of_;
        if (size_of_ > capacity_of_) {
            capacity_of_ = size_of_;
            delete[] info_;
            info_ = new char[capacity_of_ + 1];
        }
        memcpy(info_, another.info_, size_of_);
        info_[size_of_] = '\0';
        return *this;
    }

    String& operator +=(const String& another) {
        size_t another_sz = another.size_of_;
        IncreaseMemory(another_sz);
        memcpy(info_ + size_of_, another.info_, another_sz);
        size_of_ += another_sz;
        info_[size_of_] = '\0';
        return *this;
    }


    void push_back(char pushed) {
        IncreaseMemory(1);
        info_[size_of_] = pushed;
        ++size_of_;
        info_[size_of_] = '\0';
    }

    void pop_back() {
        if (empty()) return;
        --size_of_;
        info_[size_of_] = '\0';
    }

    void shrink_to_fit() {
        if (capacity_of_ <= size_of_) return;
        capacity_of_ = size_of_;
        ChangeMemory();
    }

    char& operator [](size_t index) {
        return info_[index];
    }

    const char& operator [](size_t index) const {
        return info_[index];
    }

    bool empty() const {
        return size_of_ == 0;
    }

    void clear() {
        size_of_ = 0;
        info_[size_of_] = '\0';
    }

    char* data() const {
        return info_;
    }

    size_t size() const {
        return size_of_;
    }

    size_t capacity() const {
        return capacity_of_;
    }

    size_t length() const {
        return size_of_;
    }

    char& front() {
        return info_[0];
    }

    const char& front() const {
        return info_[0];
    }

    char& back() {
        return info_[size_of_ - 1];
    }

    const char& back() const {
        return info_[size_of_ - 1];
    }

    size_t find(const String& looking_for) const {
        return find_substr(looking_for);
    }
    size_t rfind(const String& looking_for) const {
        return find_substr(looking_for, true);
    }

    String substr(size_t begin, size_t num) const {
        String Inreturn(num);
        memcpy(Inreturn.info_, info_ + begin, num);
        return Inreturn;
    }

    ~String() {
        delete[] info_;
    }

private:
    void ChangeMemory() {
        char* old_info_ = info_;
        info_ = new char[capacity_of_ + 1];
        memcpy(info_, old_info_, size_of_ + 1);
        delete[] old_info_;
    }
    void IncreaseMemory(size_t delta) {
        bool is_changed = false;
        while (capacity_of_ < size_of_ + delta) {
            capacity_of_ *= cMultiply;
            ++capacity_of_;
            is_changed = true;
        }
        if (is_changed) ChangeMemory();
    }

    size_t find_substr(const String& looking_for, bool is_rfind = false) const {
        if (info_ == nullptr || looking_for.size_of_ > size_of_) {
            return size_of_;
        }
        size_t last_pos = size_of_;
        size_t first_pos = size_of_;
        for (size_t main_pointer = 0; main_pointer + looking_for.size_of_ <= size_of_; ++main_pointer) {
            size_t help_pointer = 0;
            while (help_pointer < looking_for.size_of_ && looking_for.info_[help_pointer] == info_[main_pointer + help_pointer]) {
                help_pointer++;
            }
            if (help_pointer == looking_for.size_of_) {
                last_pos = main_pointer;
                if (first_pos == size_of_) {
                    first_pos = main_pointer;
                }
            }
        }
        return (is_rfind ? last_pos : first_pos);
    }
    char* info_;
    size_t size_of_;
    size_t capacity_of_;

};
String operator +(const String& one, const String& two) {
    String string_sum = one;
    string_sum += two;
    return string_sum;
}

bool operator ==(const String& one, const String& two) {
    if (one.size() != two.size()) return false;
    for (size_t i = 0; i != one.size(); ++i) {
        if (one[i] != two[i]) return false;
    }
    return true;
}

bool operator !=(const String& one, const String& two) {
    return !(one == two);
}
bool operator >=(const String& one, const String& two) {
    if (one.size() == 0 && two.size() == 0) {
        return true;
    }
    size_t min_sz = std::min(one.size(), two.size());
    for (size_t i = 0; i != min_sz; ++i) {
        if (one[i] != two[i]) {
            return one[i] > two[i];
        }
    }
    return one.size() >= two.size();
}
bool operator <=(const String& one, const String& two) {
    return two >= one;
}
bool operator >(const String& one, const String& two) {
    return !(one <= two);
}
bool operator <(const String& one, const String& two) {
    return !(one >= two);
}
std::ostream& operator<<(std::ostream& out, const String& string_out) {
    out << string_out.data();
    return out;
}
std::istream& operator>>(std::istream& in, String& string_in) {
    char input;
    string_in.clear();
    string_in.shrink_to_fit();
    while (in.get(input) && isspace(input)) {}
    if (input == '\0') {
        return in;
    }
    string_in.push_back(input);
    while (in.get(input) && !isspace(input)) {
        string_in.push_back(input);
    }
    return in;
}
