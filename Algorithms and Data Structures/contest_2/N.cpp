
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

int main() {
  int n;
  int q;
  int ingoing;
  std::cin >> n >> q;
  std::vector<std::pair<int, int>> quotes(q);
  std::set<std::pair<int, int>> freespace;
  freespace.insert({n, 0});
  std::set<std::pair<int, int>> engaged;
  for (int i = 0; i != q; i++) {
    /*for (auto par : freespace) {
            std::cout << par.first << ' ' << par.second << '\n';
    }*/
    std::cin >> ingoing;
    if (ingoing > 0) {
      if (freespace.empty()) {
        std::cout << "-1" << '\n';
        quotes[i] = {-1, -1};
        continue;
      }
      auto iter = freespace.rbegin();
      std::pair<int, int> working_with = *iter;
      int length = working_with.first;
      int start = -working_with.second;
      if (ingoing > length) {
        std::cout << "-1" << '\n';
        quotes[i] = {-1, -1};
        continue;
      }
      engaged.insert({start, ingoing});
      // std::cout << start << ' ' << length << '\n';
      quotes[i] = {start, ingoing};
      freespace.erase(working_with);
      std::cout << start + 1 << '\n';
      if (ingoing == length) {
        continue;
      }
      freespace.insert({length - ingoing, -(start + ingoing)});
    } else {
      quotes[i] = {-1, -1};
      int index = -ingoing - 1;

      if (quotes[index].first == -1) {
        continue;
      }
      auto iter = engaged.lower_bound(quotes[index]);
      int left = iter->first;
      int right = left + iter->second - 1;
      // std::cout << left << ' ' << right << '\n';
      if (iter == engaged.begin()) {
        if (left != 0) {
          freespace.erase({left, 0});
        }
        left = 0;
      } else {
        iter--;
        int left_left = iter->first;
        int right_left = left_left + iter->second - 1;
        if (right_left + 1 < left) {
          freespace.erase({left - right_left - 1, -(right_left + 1)});
          left = right_left + 1;
        }
        iter++;
      }
      iter++;
      if (iter == engaged.end()) {
        if (right != n - 1) {
          freespace.erase({n - 1 - right, -(right + 1)});
        }
        right = n - 1;
      } else {
        int left_right = iter->first;
        // std::cout << left_right << ' ' << right_right << '\n';
        if (right + 1 < left_right) {
          freespace.erase({left_right - right - 1, -(right + 1)});
          right = left_right - 1;
        }
      }
      iter--;
      engaged.erase(iter);
      freespace.insert({right - left + 1, -left});
    }
  }
}
