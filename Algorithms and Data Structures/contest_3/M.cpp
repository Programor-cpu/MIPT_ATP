#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int cNeutral = 0;
const int cDel = (int)pow(10, 9);

class SegmentTreePersistent {
 private:
  SegmentTreePersistent* left_ = NULL;
  SegmentTreePersistent* right_ = NULL;
  int s_ = 0;
  int index_ = 0;

 public:
  SegmentTreePersistent(SegmentTreePersistent* left,
                        SegmentTreePersistent* right, int add)
      : left_(left), right_(right), s_(cNeutral), index_(add) {}
  SegmentTreePersistent() {}
  SegmentTreePersistent(int s, int index)
      : left_(NULL), right_(NULL), s_(s), index_(index) {}
  /*~SegmentTreePersistent() {
      delete left_;
      std::cout << "del" << '\n';
      delete right_;
      std::cout << "del" << '\n';
  }*/
  SegmentTreePersistent* Build(int left, int right, std::vector<int>& arr) {
    if (0 == right - left) {
      return new SegmentTreePersistent(arr[left], 0);
    }
    return new SegmentTreePersistent(Build(left, (left + right) / 2, arr),
                                     Build((left + right) / 2 + 1, right, arr),
                                     0);
  }

  SegmentTreePersistent* SetValue(int left, int right,
                                  SegmentTreePersistent*& ver, int val,
                                  int index, int add) {
    if (0 == right - left) {
      return new SegmentTreePersistent(val, add);
    }
    if (index > (left + right) / 2) {
      return new SegmentTreePersistent(
          ver->left_,
          SetValue((left + right) / 2 + 1, right, ver->right_, val, index, add),
          add);
    }
    return new SegmentTreePersistent(
        SetValue(left, (left + right) / 2, ver->left_, val, index, add),
        ver->right_, add);
  }
  int Find(int pos, int left_t, int right_t, SegmentTreePersistent*& ver) {
    if (0 == right_t - left_t) {
      return ver->s_;
    }
    int mid = (left_t + right_t) / 2;
    if (pos > mid) {
      return Find(pos, mid + 1, right_t, ver->right_);
    }
    return Find(pos, left_t, mid, ver->left_);
  }
  void Deletetree(int index) {
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
  int n;
  std::cin >> n;
  int ver;
  int j;
  int ind = 1;
  int exch;
  std::vector<int> arr(n + 1);
  std::vector<SegmentTreePersistent*> chains;
  SegmentTreePersistent furion;
  for (int i = 0; i != n; i++) {
    std::cin >> arr[i];
  }
  chains.push_back(furion.Build(0, n - 1, arr));
  int q;
  std::cin >> q;
  while (q != 0) {
    q--;
    std::string ingoing;
    std::cin >> ingoing;
    if (ingoing == "create") {
      std::cin >> ver >> j >> exch;
      ver--;
      j--;
      chains.push_back(furion.SetValue(0, n - 1, chains[ver], exch, j, ind));
      ind++;
    } else {
      std::cin >> ver >> j;
      ver--;
      j--;
      std::cout << furion.Find(j, 0, n - 1, chains[ver]) << '\n';
    }
  }
  for (int i = (int)chains.size() - 1; i != -1; i--) {
    if (chains[i] != NULL) {
      chains[i]->Deletetree(i);
    }
  }
}