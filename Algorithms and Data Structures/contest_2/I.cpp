#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

class UniqueQue {
 private:
  std::vector<long long int> st1_;
  std::vector<long long int> st2_;
  void Balance() {
    if (!st1_.empty() || !st2_.empty()) {
      if (st1_.size() > st2_.size()) {
        st2_.insert(st2_.begin(), st1_[st1_.size() - 1]);
        st1_.erase(st1_.begin() + st1_.size() - 1);
      } else if (st1_.size() < st2_.size() - 1) {
        st1_.push_back(st2_[0]);
        st2_.erase(st2_.begin());
      }
    }
  }

 public:
  UniqueQue() {}
  void Enqueue(long long i) {
    st1_.insert(st1_.begin(), i);
    Balance();
  }
  long long int Dequeue() {
    long long int n = st2_[st2_.size() - 1];
    st2_.erase(st2_.begin() + st2_.size() - 1);
    Balance();
    return n;
  }
  void Mid(long long i) {
    if (st2_.size() == st1_.size()) {
      st2_.insert(st2_.begin(), i);
    } else {
      st1_.push_back(i);
    }
    Balance();
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  UniqueQue pudge;
  long long int q;
  std::cin >> q;
  for (long long int i = 0; i < q; i++) {
    std::string ingoing;
    std::cin >> ingoing;
    if (ingoing == "+") {
      long long int x;
      std::cin >> x;
      pudge.Enqueue(x);
    }
    if (ingoing == "-") {
      long long int x = pudge.Dequeue();
      if (x == -1) {
        std::cout << "error" << '\n';
      } else {
        std::cout << x << '\n';
      }
    }
    if (ingoing == "*") {
      long long int x;
      std::cin >> x;
      pudge.Mid(x);
    }
  }
}