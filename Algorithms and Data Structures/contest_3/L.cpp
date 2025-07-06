#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

const long long cNeutral = 0;
const size_t cMultiply = 4;

class SegmentTree {
 public:
  std::vector<std::pair<int, int>> tree;
  SegmentTree(int s) {
    tree.assign(cMultiply * s, {0, 0});
    Build(0, s, 0);
  }
  void Build(int left, int right, int ver) {
    if (1 == right - left) {
      tree[ver].first = cNeutral;
      return;
    }
    Build(left, (left + right) / 2, 2 * ver + 1);
    Build((left + right) / 2, right, 2 * ver + 2);
    tree[ver].first = tree[2 * ver + 1].first ^ tree[2 * ver + 2].first;
  }

  int GetValue(int left, int right, int ver, int index, int count) {
    if (1 == right - left) {
      return tree[ver].first ^ count;
    }
    count = count ^ tree[ver].second;
    if (index >= (left + right) / 2) {
      return GetValue((left + right) / 2, right, 2 * ver + 2, index, count);
    }
    return GetValue(left, (left + right) / 2, 2 * ver + 1, index, count);
  }
  void Xor(int left, int right, int ver, int left_t, int right_t, int count) {
    if (right_t <= left || right <= left_t) {
      return;
    }
    if ((right_t <= right) && (left <= left_t)) {
      tree[ver].first ^= 1;
      if (right_t - left_t != 1) {
        tree[ver].second ^= 1;
      }
      return;
    }
    Xor(left, right, 2 * ver + 1, left_t, (left_t + right_t) / 2,
        count ^ tree[ver].second);
    Xor(left, right, 2 * ver + 2, (left_t + right_t) / 2, right_t,
        count ^ tree[ver].second);
    tree[ver].first = tree[2 * ver + 1].first ^ tree[2 * ver + 2].first;
  }
};
struct Request {
  int x1;
  int y1;
  int x2;
  int y2;
  Request(int x1, int y1, int x2 = -1, int y2 = -1)
      : x1(x1), y1(y1), x2(x2), y2(y2) {}
};

int main() {
  int h;
  int w;
  std::cin >> h >> w;
  int n;
  std::map<int, int> xes;
  std::map<int, int> ys;
  xes[1] = 0;
  ys[1] = 0;
  xes[h] = 0;
  ys[w] = 0;
  int q;
  std::cin >> n >> q;
  std::vector<int> x_arr(n);
  std::vector<int> y_arr(n);

  std::vector<Request> requests;
  int x;
  int y;
  int x1;
  int y1;
  int x2;
  int y2;

  for (int i = 0; i != n; i++) {
    std::cin >> x_arr[i] >> y_arr[i];
    xes[x_arr[i]] = 0;
    ys[y_arr[i]] = 0;
  }
  for (int i = 0; i != q; i++) {
    int ingoing;
    std::cin >> ingoing;
    if (ingoing == 1) {
      std::cin >> x1 >> y1 >> x2 >> y2;
      xes[x1] = 0;
      xes[x2] = 0;
      ys[y1] = 0;
      ys[y2] = 0;
      requests.push_back(Request(x1, y1, x2, y2));
    } else {
      std::cin >> x >> y;
      xes[x] = 0;
      ys[y] = 0;
      requests.push_back(Request(x, y));
    }
  }
  int i = 0;
  for (auto it = xes.begin(); it != xes.end(); it++) {
    it->second = i;
    i++;
  }
  std::vector<std::unordered_set<int>> y_for_x(i + 1);
  SegmentTree x_ones(i);
  i = 0;
  for (auto it = ys.begin(); it != ys.end(); it++) {
    it->second = i;
    i++;
  }
  h = xes[h];
  w = ys[w];
  SegmentTree y_ones(i);
  for (int j = 0; j != n; j++) {
    x_arr[j] = xes[x_arr[j]];
    y_arr[j] = ys[y_arr[j]];
    y_for_x[x_arr[j]].insert(y_arr[j]);
  }
  for (int j = 0; j != q; j++) {
    requests[j].x1 = xes[requests[j].x1];
    requests[j].y1 = ys[requests[j].y1];
    if (requests[j].x2 != -1) {
      requests[j].x2 = xes[requests[j].x2];
      requests[j].y2 = ys[requests[j].y2];
    }
  }
  for (int j = 0; j != q; j++) {
    if (requests[j].x2 != -1) {
      x_ones.Xor(requests[j].x1 + 1, requests[j].x2 + 1, 0, 0, h + 1, 0);
      y_ones.Xor(0, requests[j].y1 + 1, 0, 0, w + 1, 0);
      y_ones.Xor(requests[j].y2 + 1, w + 1, 0, 0, w + 1, 0);
    } else {
      int x = requests[j].x1;
      int y = requests[j].y1;
      bool is = (y_for_x[x].find(y) != y_for_x[x].end());
      if ((int)(x_ones.GetValue(0, h + 1, 0, x, 0) ^
                y_ones.GetValue(0, w + 1, 0, y, 0) ^ ((int)is)) == 1) {
        std::cout << "YES" << '\n';
      } else {
        std::cout << "NO" << '\n';
      }
    }
  }
}