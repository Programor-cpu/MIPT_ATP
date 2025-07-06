#include <algorithm>
#include <iostream>
#include <vector>

const long long cNeutral = -2;
const size_t cMultiply = 4;

class SegmentTree {
 public:
  std::vector<long long> tree;
  SegmentTree(const std::vector<long long>& arr) {
    tree.assign(cMultiply * arr.size(), 0);
    Build(0, arr.size(), 0, arr);
  }
  void Build(int left, int right, int ver, const std::vector<long long>& arr) {
    if (1 == right - left) {
      tree[ver] = arr[left];
      return;
    }
    Build(left, (left + right) / 2, 2 * ver + 1, arr);
    Build((left + right) / 2, right, 2 * ver + 2, arr);
    tree[ver] = std::max(tree[2 * ver + 1], tree[2 * ver + 2]);
    // std::cout << ver << " " << tree[ver] << '\n';
  }

  void SetValue(int left, int right, int ver, long long value, int index) {
    if (1 == right - left) {
      tree[ver] = value;
      return;
    }
    if (index >= (left + right) / 2) {
      SetValue((left + right) / 2, right, 2 * ver + 2, value, index);
    } else {
      SetValue(left, (left + right) / 2, 2 * ver + 1, value, index);
    }
    tree[ver] = std::max(tree[2 * ver + 1], tree[2 * ver + 2]);
  }
  int Answering(long long val, int left, int right, int ver, int index) {
    if (index >= right) {
      return cNeutral;
    }
    if (tree[ver] < val) {
      return cNeutral;
    }
    if (1 == right - left) {
      return left;
    }
    long long possible_ans =
        Answering(val, left, (left + right) / 2, 2 * ver + 1, index);
    if (possible_ans == cNeutral) {
      possible_ans =
          Answering(val, (left + right) / 2, right, 2 * ver + 2, index);
    }
    return possible_ans;
  }
};

int main() {
  int n;
  std::cin >> n;
  std::vector<long long> arr(n);
  int q;
  std::cin >> q;
  for (int i = 0; i != n; i++) {
    std::cin >> arr[i];
  }
  SegmentTree furion(arr);
  while (q != 0) {
    q--;
    int req_tp;
    std::cin >> req_tp;
    if (req_tp == 0) {
      int ind;
      std::cin >> ind;
      ind--;
      long long val;
      std::cin >> val;
      furion.SetValue(0, arr.size(), 0, val, ind);
    } else {
      int ind;
      std::cin >> ind;
      ind--;
      long long val;
      std::cin >> val;
      std::cout << furion.Answering(val, 0, arr.size(), 0, ind) + 1 << '\n';
    }
  }
}