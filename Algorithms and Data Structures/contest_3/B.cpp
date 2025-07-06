#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const long long cNeutral = 0;
const size_t cMultiply = 4;
const int cDistance = 42195;
const int cAmount = pow(10, 5);
const int cSign = 20;

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
  std::cout.precision(cSign);
  int q;
  std::cin >> q;
  std::vector<int> dist(cDistance, 0);
  std::vector<int> part(cAmount, -1);
  std::string ingoing;
  int num;
  int meter;
  SegmentTree furion(dist);
  while (q != 0) {
    q--;
    std::cin >> ingoing;
    if (ingoing == "RUN") {
      std::cin >> num >> meter;
      meter--;
      num--;
      if (part[num] != -1 && dist[part[num]] != 0) {
        dist[part[num]] -= 1;
        furion.SetValue(0, cDistance, 0, dist[part[num]], part[num]);
      }
      part[num] = meter;
      dist[meter] += 1;
      furion.SetValue(0, cDistance, 0, dist[meter], meter);
    } else {
      std::cin >> num;
      num--;
      meter = part[num];
      double x = 0;
      if (part[num] != -1 &&
          furion.Answering(0, cDistance, 0, 0, cDistance) != 1) {
        x = ((double)furion.Answering(0, meter, 0, 0, cDistance)) /
            ((double)furion.Answering(0, cDistance, 0, 0, cDistance) - 1);
        std::cout << x << '\n';
      } else {
        if (part[num] == -1) {
          std::cout << 0 << '\n';
        } else {
          std::cout << 1 << '\n';
        }
      }
    }
  }
}