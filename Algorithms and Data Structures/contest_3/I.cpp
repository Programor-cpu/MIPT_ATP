#include <algorithm>
#include <iostream>
#include <vector>

const long long cNeutral = 0;
const size_t cMultiply = 4;

class SegmentTreeMerge {
 private:
  std::vector<std::vector<int>> tree_;
  void Build(int left, int right, int ver, const std::vector<int>& arr) {
    if (1 == right - left) {
      tree_[ver][0] = arr[left];
      return;
    }
    Build(left, (left + right) / 2, 2 * ver + 1, arr);
    Build((left + right) / 2, right, 2 * ver + 2, arr);
    tree_[ver].resize(tree_[2 * ver + 2].size() + tree_[2 * ver + 1].size());
    std::merge(tree_[2 * ver + 1].begin(), tree_[2 * ver + 1].end(),
               tree_[2 * ver + 2].begin(), tree_[2 * ver + 2].end(),
               tree_[ver].begin());
  }

 public:
  SegmentTreeMerge(const std::vector<int>& arr) {
    tree_.assign(cMultiply * arr.size(), {0});
    Build(0, arr.size(), 0, arr);
  }
  void SetValue(int left, int right, int ver, int value, int index) {
    if (1 == right - left) {
      tree_[ver][0] = value;
      return;
    }
    if (index >= (left + right) / 2) {
      SetValue((left + right) / 2, right, 2 * ver + 2, value, index);
    } else {
      SetValue(left, (left + right) / 2, 2 * ver + 1, value, index);
    }
    tree_[ver].resize(tree_[2 * ver + 2].size() + tree_[2 * ver + 1].size());
    std::merge(tree_[2 * ver + 1].begin(), tree_[2 * ver + 1].end(),
               tree_[2 * ver + 2].begin(), tree_[2 * ver + 2].end(),
               tree_[ver].begin());
  }
  int Answering(int val, int left, int right, int ver, int left_t,
                int right_t) {
    if (right_t <= left || right <= left_t || val < tree_[ver][0]) {
      return cNeutral;
    }
    if (((right_t <= right) && (left <= left_t))) {
      return lower_bound(tree_[ver].begin(), tree_[ver].end(), val + 1) -
             tree_[ver].begin();
    }
    return Answering(val, left, right, 2 * ver + 1, left_t,
                     (left_t + right_t) / 2) +
           Answering(val, left, right, 2 * ver + 2, (left_t + right_t) / 2,
                     right_t);
  }
};

int main() {
  int n;
  std::cin >> n;
  int q;
  int l;
  int r;
  int x;
  int y;
  std::cin >> q;
  std::vector<int> arr(n);
  for (int i = 0; i != n; i++) {
    std::cin >> arr[i];
  }
  SegmentTreeMerge furion(arr);
  while (q != 0) {
    --q;
    std::cin >> l >> r >> x >> y;
    --l;
    std::cout << furion.Answering(y, l, r, 0, 0, arr.size()) -
                     furion.Answering(x - 1, l, r, 0, 0, arr.size())
              << '\n';
  }
}