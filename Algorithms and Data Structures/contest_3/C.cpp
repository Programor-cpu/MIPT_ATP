#include <algorithm>
#include <iostream>
#include <vector>

const long long cNeutral = 0;
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
    tree[ver] = tree[2 * ver + 1] + tree[2 * ver + 2];
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
    tree[ver] = tree[2 * ver + 1] + tree[2 * ver + 2];
  }
  long long Answering(int left, int right, int ver, int left_t, int right_t) {
    // std::cout << ver << '\n';
    // std::cout << left_t << ' ' << right_t << '\n';
    if (right_t <= left || right <= left_t) {
      return cNeutral;
    }
    if ((right_t <= right) && (left <= left_t)) {
      return tree[ver];
    }
    return Answering(left, right, 2 * ver + 1, left_t, (left_t + right_t) / 2) +
           Answering(left, right, 2 * ver + 2, (left_t + right_t) / 2, right_t);
  }
};

int main() {
  int n;
  std::cin >> n;
  std::vector<long long> arr(n);
  for (int i = 0; i != n; i++) {
    std::cin >> arr[i];
    if (i % 2 != 0) {
      arr[i] *= (-1);
    }
  }
  SegmentTree furion(arr);
  int q;
  std::cin >> q;
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
      if (ind % 2 != 0) {
        val *= (-1);
      }
      furion.SetValue(0, arr.size(), 0, val, ind);
    } else {
      int left;
      int right;
      std::cin >> left;
      left--;
      std::cin >> right;
      long long ans = furion.Answering(left, right, 0, 0, arr.size());
      if (left % 2 != 0) {
        std::cout << -ans << '\n';
      } else {
        std::cout << ans << '\n';
      }
    }
  }
}