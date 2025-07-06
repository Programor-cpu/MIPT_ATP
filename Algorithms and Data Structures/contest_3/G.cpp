#include <algorithm>
#include <iostream>
#include <vector>

const long long cNeutral = 0;
const size_t cMultiply = 4;
const long long int cBig = 100000000000000000;

class SegmentTree {
 public:
  std::vector<long long int> tree;
  SegmentTree(const std::vector<long long int>& arr) {
    tree.assign(cMultiply * arr.size(), 0);
    Build(0, arr.size(), 0, arr);
  }
  void Build(long long int left, long long int right, long long int ver,
             const std::vector<long long int>& arr) {
    if (1 == right - left) {
      tree[ver] = arr[left];
      return;
    }
    Build(left, (left + right) / 2, 2 * ver + 1, arr);
    Build((left + right) / 2, right, 2 * ver + 2, arr);
    tree[ver] = tree[2 * ver + 1] + tree[2 * ver + 2];
  }

  void SetValue(long long int left, long long int right, long long int ver,
                long long int value, long long int index) {
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
  long long int Answering(long long int left, long long int right,
                          long long int ver, long long int left_t,
                          long long int right_t) {
    if (right_t <= left || right <= left_t) {
      return cNeutral;
    }
    if ((right_t <= right) && (left <= left_t)) {
      return tree[ver];
    }
    return Answering(left, right, 2 * ver + 1, left_t, (left_t + right_t) / 2) +
           Answering(left, right, 2 * ver + 2, (left_t + right_t) / 2, right_t);
  }
  long long int Indexation(long long int ver, long long int left_t,
                           long long int right_t, long long int k) {
    if (right_t - left_t == 1) {
      return left_t;
    }
    long long int mid = (right_t + left_t) / 2;
    if (tree[2 * ver + 1] >= k) {
      return Indexation(2 * ver + 1, left_t, mid, k);
    }
    return Indexation(2 * ver + 2, mid, right_t, k - tree[2 * ver + 1]);
  }
};

struct Request {
  char sign;
  long long int x;
  long long int order;
  Request(char sign, long long int x, long long int order)
      : sign(sign), x(x), order(order){};
};

int main() {
  std::vector<Request> arr_requests;
  std::vector<std::pair<long long int, long long int>> pluses;
  long long int n;
  long long int x;
  std::cin >> n;
  char ingoing;
  for (long long int i = 0; i != n; i++) {
    std::cin >> ingoing;
    std::cin >> x;
    if (ingoing == '+') {
      pluses.push_back({x, i});
    }
    arr_requests.push_back(Request(ingoing, x, 0));
  }
  std::sort(pluses.begin(), pluses.end());
  for (long long int i = 0; i != (long long int)pluses.size(); i++) {
    arr_requests[pluses[i].second].order = i;
  }
  long long int s = pluses.size();
  std::vector<long long int> arr(s, 0);
  SegmentTree tr(arr);
  for (long long int i = 0; i != n; i++) {
    if (arr_requests[i].sign == '+') {
      tr.SetValue(0, s, 0, arr_requests[i].x, arr_requests[i].order);
    } else {
      long long int t =
          std::upper_bound(pluses.begin(), pluses.end(),
                           std::make_pair(arr_requests[i].x, cBig)) -
          pluses.begin();
      std::cout << tr.Answering(0, t, 0, 0, s) << '\n';
    }
  }
}