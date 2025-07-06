#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const long long cNeutral = 0;
const long long cDel = (long long)pow(10, 9);

class SegmentTreePersistent {
 private:
  SegmentTreePersistent* left_ = NULL;
  SegmentTreePersistent* right_ = NULL;
  long long s_ = 0;
  long long index_ = 0;

 public:
  SegmentTreePersistent(SegmentTreePersistent* left,
                        SegmentTreePersistent* right, long long add)
      : left_(left), right_(right), s_((left->s_) + (right->s_)), index_(add) {}
  SegmentTreePersistent() {}
  SegmentTreePersistent(long long s, long long index)
      : left_(NULL), right_(NULL), s_(s), index_(index) {}
  /*~SegmentTreePersistent() {
      delete left_;
      std::cout << "del" << '\n';
      delete right_;
      std::cout << "del" << '\n';
  }*/
  SegmentTreePersistent* Build(long long left, long long right) {
    if (0 == right - left) {
      return new SegmentTreePersistent(cNeutral, 0);
    }
    return new SegmentTreePersistent(Build(left, (left + right) / 2),
                                     Build((left + right) / 2 + 1, right), 0);
  }

  SegmentTreePersistent* SetValue(long long left, long long right,
                                  SegmentTreePersistent*& ver, long long index,
                                  long long add) {
    if (0 == right - left) {
      return new SegmentTreePersistent(ver->s_ + 1, add);
    }
    if (index > (left + right) / 2) {
      return new SegmentTreePersistent(
          ver->left_,
          SetValue((left + right) / 2 + 1, right, ver->right_, index, add),
          add);
    }
    return new SegmentTreePersistent(
        SetValue(left, (left + right) / 2, ver->left_, index, add), ver->right_,
        add);
  }
  long long Sum(long long left, long long right, SegmentTreePersistent* ver,
                long long left_t, long long right_t) {
    if (right_t <= left || right <= left_t) {
      return cNeutral;
    }
    if ((right_t <= right) && (left <= left_t)) {
      return ver->s_;
    }
    return Sum(left, right, ver->left_, left_t, (left_t + right_t) / 2) +
           Sum(left, right, ver->right_, (left_t + right_t) / 2, right_t);
  }
  long long Find(long long k, long long left_t, long long right_t,
                 SegmentTreePersistent*& left_ver,
                 SegmentTreePersistent*& right_ver) {
    if (0 == right_t - left_t) {
      return left_t;
    }
    if (0 == k) {
      return s_;
    }
    long long b = right_ver->left_->s_ - left_ver->left_->s_;
    long long mid = (left_t + right_t) / 2;
    if (k > b) {
      return Find(k - b, mid + 1, right_t, left_ver->right_, right_ver->right_);
    }
    return Find(k, left_t, mid, left_ver->left_, right_ver->left_);
  }
  void Deletetree(long long index) {
    if (index != index_) {
      return;
    }
    if (left_ != NULL) {
      left_->Deletetree(index);
    }
    if (right_ != NULL) {
      right_->Deletetree(index);
    }
    if (index_ == index) {
      delete this;
    }
  }
};

int main() {
  long long sum = 0;
  long long n;
  long long a;
  long long p;
  long long q;
  long long b;
  std::cin >> n >> a >> p >> q >> b;
  std::vector<std::pair<long long, long long>> arr(n);
  std::vector<long long> copy_arr(n);
  std::vector<SegmentTreePersistent*> chains;
  SegmentTreePersistent furion;
  chains.push_back(furion.Build(0, n));
  arr[0].first = a;
  arr[0].second = 0;
  for (long long i = 1; i != n; i++) {
    a = ((a * p + q) % cDel);
    arr[i].first = a;
    arr[i].second = i;
  }
  sort(arr.begin(), arr.end());
  for (long long i = 0; i != n; i++) {
    copy_arr[arr[i].second] = i;
  }

  for (long long i = 0; i != n; i++) {
    chains.push_back(
        furion.SetValue(0, arr.size(), chains.back(), copy_arr[i], i + 1));
  }
  /*for (long long i = 0; i != n; i++) {
      std::cout << "(" << arr[i].first << ", " << arr[i].second.second<<") ";
  }
  std::cout << '\n';*/
  while (b != 0) {
    b--;
    long long m;
    long long x;
    long long y;
    long long k;
    long long px;
    long long qx;
    long long py;
    long long qy;
    long long pk;
    long long qk;
    long long l;
    long long r;
    std::cin >> m >> x >> px >> qx >> y >> py >> qy >> k >> pk >> qk;
    for (long long i = 1; i <= m; i++) {
      if (i >= 2) {
        x = 1 + ((l - 1) * px + qx) % n;
        y = 1 + ((r - 1) * py + qy) % n;
        l = std::min(x, y);
        r = std::max(x, y);
        k = 1 + ((k - 1) * pk + qk) % (r - l + 1);
      } else {
        l = std::min(x, y);
        r = std::max(x, y);
      }
      // std::cout << furion.Find(k, 0, arr.size(), chains[l-1], chains[r]) <<
      // '\n';
      sum += arr[furion.Find(k, 0, arr.size(), chains[l - 1], chains[r])].first;
    }
  }
  std::cout << sum;
  for (long long i = (long long)chains.size() - 1; i != -1; i--) {
    if (chains[i] != NULL) {
      chains[i]->Deletetree(i);
    }
  }
}