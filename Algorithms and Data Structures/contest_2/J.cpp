#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

class MinQue {
 private:
  std::stack<std::pair<int, int>> st1_;
  std::stack<std::pair<int, int>> st2_;

 public:
  MinQue() {}
  int Size() { return (st1_.size() + st2_.size()); }
  void Clear() {
    while (!st1_.empty()) {
      st1_.pop();
    }
    while (!st2_.empty()) {
      st2_.pop();
    }
  }

  void Enqueue(int i) {
    if (st1_.empty()) {
      st1_.push({i, i});
    } else {
      int current_minmum = std::min(i, st1_.top().second);
      st1_.push({i, current_minmum});
    }
  }

  int GetMin() {
    int m = -1;
    if (!st1_.empty() && !st2_.empty()) {
      m = std::min(st1_.top().second, st2_.top().second);
    } else if (!st1_.empty()) {
      m = st1_.top().second;
    } else if (!st2_.empty()) {
      m = st2_.top().second;
    }

    return m;
  }

  int Front() {
    if (st1_.empty() && st2_.empty()) {
      return -1;
    }

    if (st2_.empty()) {
      int n = st1_.size();
      for (int i = 0; i < n; i++) {
        if (st2_.empty()) {
          st2_.push({st1_.top().first, st1_.top().first});
        } else {
          int current_minmum = std::min(st1_.top().first, st2_.top().second);
          st2_.push({st1_.top().first, current_minmum});
        }
        st1_.pop();
      }
    }
    std::pair<int, int> r = st2_.top();
    return r.first;
  }

  int Dequeue() {
    int n = Front();
    if (n != -1) {
      st2_.pop();
      return n;
    }
    return -1;
  }
};

int main() {
  MinQue pudge;
  int q;
  std::cin >> q;
  for (int i = 0; i < q; i++) {
    std::string ingoing;
    std::cin >> ingoing;
    if (ingoing == "enqueue") {
      int x;
      std::cin >> x;
      pudge.Enqueue(x);
      std::cout << "ok" << '\n';
    }
    if (ingoing == "dequeue") {
      int x = pudge.Dequeue();
      if (x == -1) {
        std::cout << "error" << '\n';
      } else {
        std::cout << x << '\n';
      }
    }
    if (ingoing == "front") {
      int x = pudge.Front();
      if (x == -1) {
        std::cout << "error" << '\n';
      } else {
        std::cout << x << '\n';
      }
    }
    if (ingoing == "size") {
      std::cout << pudge.Size() << '\n';
    }
    if (ingoing == "clear") {
      pudge.Clear();
      std::cout << "ok" << '\n';
    }
    if (ingoing == "min") {
      if (pudge.GetMin() != -1) {
        std::cout << pudge.GetMin() << '\n';
      } else {
        std::cout << "error" << '\n';
      }
    }
  }
}