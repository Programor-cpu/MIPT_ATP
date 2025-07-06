#include <algorithm>
#include <iostream>
#include <vector>

const long long cNeutral = 0;
const size_t cMultiply = 4;

class SegmentTree {
 public:
  std::vector<int> tree;
  SegmentTree(const std::vector<int>& arr) {
    tree.assign(cMultiply * arr.size(), 0);
    Build(0, arr.size(), 0, arr);
  }
  void Build(int left, int right, int ver, const std::vector<int>& arr) {
    if (1 == right - left) {
      tree[ver] = arr[left];
      return;
    }
    Build(left, (left + right) / 2, 2 * ver + 1, arr);
    Build((left + right) / 2, right, 2 * ver + 2, arr);
    tree[ver] = tree[2 * ver + 1] + tree[2 * ver + 2];
  }

  void SetValue(int left, int right, int ver, int value, int index) {
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
  int Answering(int left, int right, int ver, int left_t, int right_t) {
    if (right_t <= left || right <= left_t) {
      return cNeutral;
    }
    if ((right_t <= right) && (left <= left_t)) {
      return tree[ver];
    }
    return Answering(left, right, 2 * ver + 1, left_t, (left_t + right_t) / 2) +
           Answering(left, right, 2 * ver + 2, (left_t + right_t) / 2, right_t);
  }
  int Indexation(int ver, int left_t, int right_t, int k) {
    if (right_t - left_t == 1) {
      return left_t;
    }
    int mid = (right_t + left_t) / 2;
    if (tree[2 * ver + 1] >= k) {
      return Indexation(2 * ver + 1, left_t, mid, k);
    }
    return Indexation(2 * ver + 2, mid, right_t, k - tree[2 * ver + 1]);
  }
};

int main() {
  std::vector<int> arr;
  int ingoing;
  while (std::cin >> ingoing) {
    arr.push_back(ingoing);
  }
  int n = arr.size();
  std::vector<int> boolean(n);
  for (int i = 0; i != n; i++) {
    boolean[i] = 1;
  }
  std::vector<int> answer(n);
  SegmentTree t(boolean);
  for (int i = 0; i != n; i++) {
    int index = t.Indexation(0, 0, n, arr[i] + 1);
    t.SetValue(0, n, 0, 0, index);
    answer[index] = i + 1;
  }
  std::vector<int> inverted(n);
  std::vector<int> pi(n);
  for (int i = 0; i != n; i++) {
    inverted[answer[i] - 1] = i + 1;
  }
  for (int i = 0; i != n; i++) {
    boolean[i] = 0;
  }
  SegmentTree w(boolean);
  for (int i = 0; i != n; i++) {
    w.SetValue(0, n, 0, 1, inverted[i] - 1);
    pi[inverted[i] - 1] = w.Answering(inverted[i], n, 0, 0, n);
  }
  for (int i = 0; i != n; i++) {
    std::cout << pi[i] << " ";
  }
}