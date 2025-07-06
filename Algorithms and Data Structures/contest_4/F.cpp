#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <vector>

const long long cNeutral = 0;
const long long cD = pow(10, 9);

class SegmentTreeImplicit {
 public:
  long long left;
  long long right;
  long long value = 0;
  SegmentTreeImplicit* l = nullptr;
  SegmentTreeImplicit* r = nullptr;

  SegmentTreeImplicit(long long left, long long right)
      : left(left), right(right) {}

  void Add(long long index, long long val) {
    value += val;
    long long mid = (right + left) / 2;
    if (right - left != 1) {
      if (index < mid) {
        if (l == nullptr) {
          l = new SegmentTreeImplicit(left, mid);
        }
        l->Add(index, val);
      } else {
        if (r == nullptr) {
          r = new SegmentTreeImplicit(mid, right);
        }
        r->Add(index, val);
      }
    }
  }
  long long Answer(long long left_t, long long right_t) const {
    if (std::max(left, left_t) >= std::min(right, right_t)) {
      return cNeutral;
    }
    if ((right_t >= right) && (left >= left_t)) {
      return value;
    }
    if (r != nullptr && l != nullptr) {
      return l->Answer(left_t, right_t) + r->Answer(left_t, right_t);
    }
    if (r != nullptr) {
      return r->Answer(left_t, right_t);
    }
    if (l != nullptr) {
      return l->Answer(left_t, right_t);
    }
    return 0;
  }
};
void Destroy(SegmentTreeImplicit* k) {
  if (k->right - k->left != 1) {
    if (k->l != nullptr) {
      Destroy(k->l);
    }
    if (k->r != nullptr) {
      Destroy(k->r);
    }
  }
  delete k;
}

int main() {
  std::unordered_set<long long> arr;
  long long n;
  char req;
  long long ingoing;
  std::cin >> n;
  bool prev = false;
  long long pr = 0;
  long long left = 0;
  long long right = 0;
  SegmentTreeImplicit furion(0, cD);
  while (n != 0) {
    n--;
    std::cin >> req;
    if (req == '+') {
      std::cin >> ingoing;
      if (prev) {
        ingoing += pr;
        ingoing %= cD;
      }
      if (arr.find(ingoing) == arr.end()) {
        arr.insert(ingoing);
        furion.Add(ingoing, ingoing);
      }
      prev = false;
    } else {
      std::cin >> left >> right;
      pr = furion.Answer(left, right + 1);
      prev = true;
      std::cout << pr << '\n';
    }
  }
  if (furion.r != nullptr) {
    Destroy(furion.r);
  }
  if (furion.l != nullptr) {
    Destroy(furion.l);
  }
}