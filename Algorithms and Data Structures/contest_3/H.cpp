#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

const int cMultiply = 4;

struct Segment {
  long long howmany = 0;
  long long covered = 0;
};

struct Info {
  int ver = 0;
  int left_t = 0;
  int right_t = 0;
  int left = 0;
  int right = 0;
  int type = 0;
};
class SegmentTree {
 private:
  std::vector<Segment> tree_;

 public:
  SegmentTree(const std::vector<int>& arr) {
    tree_.assign(cMultiply * arr.size(), {0, 0});
  }
  long long HowMany() { return tree_[0].covered; }
  void Update(Info give, std::vector<int>& ys) {
    int ver = give.ver;
    int right_t = give.right_t;
    int left_t = give.left_t;
    int right = give.right;
    int left = give.left;
    int type = give.type;
    if (right_t <= left || right <= left_t) {
      return;
    }
    if ((right_t <= right) && (left <= left_t)) {
      if (type == 0) {
        ++tree_[ver].howmany;
      } else {
        --tree_[ver].howmany;
      }
      if (tree_[ver].howmany != 0) {
        tree_[ver].covered = ys[right_t] - ys[left_t];
        return;
      }
      if (right_t == left_t + 1) {
        tree_[ver].covered = 0;
        return;
      }
      tree_[ver].covered =
          tree_[2 * ver + 1].covered + tree_[2 * ver + 2].covered;
      return;
    }
    int mid = (left_t + right_t) / 2;
    if (mid >= left) {
      Update({2 * ver + 1, left_t, mid, left, right, type}, ys);
    }
    if (mid < right) {
      Update({2 * ver + 2, mid, right_t, left, right, type}, ys);
    }
    if (tree_[ver].howmany == 0) {
      tree_[ver].covered =
          tree_[2 * ver + 1].covered + tree_[2 * ver + 2].covered;
      return;
    }
    tree_[ver].covered = ys[right_t] - ys[left_t];
  }
};

struct Event {
  int x;
  int y_b;
  int y_e;
  int type;
};

bool Comp(const Event& left, const Event& right) {
  if (left.x < right.x) {
    return true;
  }
  if (left.x == right.x && left.y_b < right.y_b) {
    if (left.y_b == right.y_b && left.y_e < right.y_e) {
      if (left.y_e == right.y_e && left.type < right.type) {
        return true;
      }
    }
  }
  return false;
}

int main() {
  int n;
  std::vector<Event> arr;
  std::vector<int> ys;
  std::map<int, int> cipher;
  std::cin >> n;
  long long sum = 0;
  int x1;
  int x2;
  int y1;
  int y2;
  for (int i = 0; i != n; i++) {
    std::cin >> x1 >> y1 >> x2 >> y2;
    if (x1 != x2 && y1 != y2) {
      Event now;
      now.x = x1;
      now.y_b = y1;
      cipher[y1] = 0;
      ys.push_back(y1);
      now.y_e = y2;
      cipher[y2] = 0;
      ys.push_back(y2);
      now.type = 0;
      arr.push_back(now);
      now.x = x2;
      now.type = 1;
      arr.push_back(now);
    }
  }

  std::sort(ys.begin(), ys.end());
  ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
  int j = 0;
  for (auto it = cipher.begin(); it != cipher.end(); it++) {
    it->second = j;
    j++;
  }
  std::sort(arr.begin(), arr.end(), Comp);
  SegmentTree furion(ys);
  furion.Update({0, 0, (int)ys.size(), cipher[arr[0].y_b], cipher[arr[0].y_e],
                 arr[0].type},
                ys);
  for (int i = 1; i != (int)arr.size(); i++) {
    sum += (arr[i].x - arr[i - 1].x) * furion.HowMany();
    furion.Update({0, 0, (int)ys.size(), cipher[arr[i].y_b], cipher[arr[i].y_e],
                   arr[i].type},
                  ys);
  }
  std::cout << sum;
}