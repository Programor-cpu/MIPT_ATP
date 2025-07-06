#include <algorithm>
#include <iostream>
#include <vector>

const long long int cNeutral = 0;
const size_t cMultiply = 4;

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
                long long value, long long int index) {
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
};

int main() {
  long long int n;
  std::cin >> n;
  long long int count = 0;
  long long int answer = 0;
  std::vector<std::pair<long long int, std::pair<long long int, long long int>>>
      arr(n);
  std::vector<long long int> ends(n);
  long long int current = -1;
  for (long long int i = 0; i != n; i++) {
    std::cin >> arr[i].second.first;
    std::cin >> arr[i].first;
  }
  std::sort(arr.begin(), arr.end());
  for (long long int i = 0; i != n; i++) {
    if (current == arr[i].first) {
      ends[arr[i - 1].second.second] += 1;
      arr[i].second.second = arr[i - 1].second.second;
    } else {
      arr[i].second.second = i;
      ends[i] += 1;
    }
    current = arr[i].first;
    arr[i].first = arr[i].second.first;
    arr[i].second.first = current * (-1);
  }
  std::sort(arr.begin(), arr.end());
  SegmentTree furion(ends);
  for (long long int i = 0; i != n; i++) {
    answer += furion.Answering(0, arr[i].second.second + 1, 0, 0, ends.size());
    furion.SetValue(0, ends.size(), 0, ends[arr[i].second.second] - 1,
                    arr[i].second.second);
    ends[arr[i].second.second] -= 1;
  }
  long long int help = 0;
  for (long long int i = 0; i != n - 1; i++) {
    if (arr[i].first == arr[i + 1].first &&
        arr[i].second.first == arr[i + 1].second.first) {
      help += 1;
      count += help;
    } else {
      help = 0;
    }
  }
  std::cout << answer - n - count;
}