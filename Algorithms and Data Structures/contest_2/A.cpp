#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

//Doesn't work with current tests TL16.

class Heap {
 private:
  std::vector<long long int> heap_;
  std::vector<long long int> quotes_;
  long long int Heapsize() { return ((long long int)heap_.size()); }

  void SiftUp(long long int i) {
    while (heap_[(i - 1) / 2] > heap_[i]) {
      std::swap(heap_[(i - 1) / 2], heap_[i]);
      i = (i - 1) / 2;
    }
  }

  void SiftDown(long long int i) {
    if (Heapsize() == 2) {
      if (heap_[0] > heap_[1]) {
        std::swap(heap_[0], heap_[1]);
      }
    } else {
      while (Heapsize() > 2 * i + 1) {
        long long int l = 2 * i + 1;
        long long int pudge = l;
        long long int r = 2 * i + 2;
        if (heap_[l] > heap_[r] && Heapsize() > r) {
          pudge = r;
        }
        if (heap_[pudge] >= heap_[i]) {
          break;
        }
        std::swap(heap_[pudge], heap_[i]);
        i = pudge;
      }
    }
  }

 public:
  Heap() {}
  long long int GetMin() {
    quotes_.push_back(-1);
    return heap_[0];
  }

  void ExtractMin() {
    heap_[0] = heap_[(int)heap_.size() - 1];
    heap_.erase(heap_.begin() + (long long int)heap_.size() - 1);
    SiftDown(0);
    quotes_.push_back(-1);
  }

  void Insert(long long int pudge) {
    heap_.push_back(pudge);
    SiftUp(Heapsize() - 1);
    quotes_.push_back(pudge);
  }

  void DecreaseKey(long long int index, long long int delta) {
    long long int key = quotes_[index - 1];
    long long int need = 0;
    quotes_[index - 1] -= delta;
    for (long long int i = 0; i < (long long int)heap_.size(); i++) {
      if (heap_[i] == key) {
        need = i;
        break;
      }
    }
    heap_[need] -= delta;
    SiftUp(need);
    quotes_.push_back(-1);
  }
};

int main() {
  long long int q;
  std::cin >> q;
  Heap pudge;
  for (long long int i = 0; i < q; i++) {
    std::string ingoing;
    std::cin >> ingoing;
    if (ingoing == "getMin") {
      std::cout << pudge.GetMin() << '\n';
    }
    if (ingoing == "extractMin") {
      pudge.ExtractMin();
    }
    if (ingoing == "insert") {
      long long int a;
      std::cin >> a;
      pudge.Insert(a);
    }
    if (ingoing == "decreaseKey") {
      long long int index;
      long long int delta;
      std::cin >> index;
      std::cin >> delta;
      pudge.DecreaseKey(index, delta);
    }
  }
}