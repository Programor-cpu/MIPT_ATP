#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#pragma clang optimize on

const long long cBig = 1e18;

class FenwickImplicit {
 public:
  std::vector<std::pair<int, long long>> arr;
  FenwickImplicit() {}
  void Upd(int x, long long value) {
    int x_looking =
        std::upper_bound(arr.begin(), arr.end(), std::make_pair(x, cBig)) -
        arr.begin() - 1;
    for (; x_looking < (int)arr.size();
         x_looking = (x_looking | (x_looking + 1))) {
      arr[x_looking].second += value;
    }
  }

  long long Sum(int x) {
    long long su = 0;
    int x_looking =
        std::upper_bound(arr.begin(), arr.end(), std::make_pair(x, cBig)) -
        arr.begin() - 1;
    for (; x_looking > 0; x_looking = (x_looking & (x_looking + 1)) - 1) {
      su += arr[x_looking].second;
    }
    return su;
  }
};

class FenwickTreeFenwick {
 public:
  long long n;
  std::vector<FenwickImplicit> arr;
  FenwickTreeFenwick(long long n)
      : n(n), arr(std::vector<FenwickImplicit>(n + 1)) {}
  void Upd(long long x, long long y, long long value) {
    x++;
    y++;
    for (; x <= n; x = (x | (x + 1))) {
      arr[x].Upd(y, value);
    }
  }

  long long Sum(long long x, long long y) {
    x++;
    y++;
    long long su = 0;
    for (; x > 0; x = (x & (x + 1)) - 1) {
      su += arr[x].Sum(y);
    }
    return su;
  }
  void Add(long long x, long long y) {
    x++;
    y++;
    for (; x <= n; x = (x | (x + 1))) {
      arr[x].arr.push_back({y, 0});
    }
  }
};

struct Point {
  long long x;
  long long y;
  long long w;
  Point() : x(0), y(0), w(0) {}
};
struct Request {
  bool type;
  long long one;
  long long two;
  long long three;
  long long four;
  Request() : type(false), one(0), two(0), three(0), four(0) {}
};

int main() {
  long long n;
  long long m;
  std::cin >> n;
  std::cin >> m;
  std::vector<Point> arr(n);
  std::map<long long, long long> xes;
  std::map<long long, long long> ys;
  for (long long i = 0; i != n; i++) {
    std::cin >> arr[i].x;
    arr[i].y = i;
    arr[i].w = 1;
    xes[arr[i].x];
    ys[arr[i].y];
  }

  std::vector<Request> request(m);
  for (long long i = 0; i != m; i++) {
    std::string ingoing;
    std::cin >> ingoing;
    request[i].type = (ingoing == "SET");  // SET - 1 GET - 0
    if (!request[i].type) {
      std::cin >> request[i].one >> request[i].two >> request[i].three >>
          request[i].four;
      --request[i].one;
      --request[i].two;
      ys[request[i].one];
      ys[request[i].two];
      xes[request[i].three];
      xes[request[i].four];
    } else {
      std::cin >> request[i].one >> request[i].two;
      --request[i].one;
      ys[request[i].one];
      xes[request[i].two];
    }
  }
  long long i = 0;
  for (auto it = xes.begin(); it != xes.end(); it++) {
    it->second = i;
    i++;
  }
  i = 0;
  for (auto it = ys.begin(); it != ys.end(); it++) {
    it->second = i;
    i++;
  }
  for (long long i = 0; i != n; i++) {
    arr[i].x = xes[arr[i].x];
    arr[i].y = ys[arr[i].y];
  }
  for (long long i = 0; i != m; i++) {
    if (!request[i].type) {
      request[i].one = ys[request[i].one];
      request[i].two = ys[request[i].two];
      request[i].three = xes[request[i].three];
      request[i].four = xes[request[i].four];
    } else {
      request[i].one = ys[request[i].one];
      request[i].two = xes[request[i].two];
    }
  }
  FenwickTreeFenwick tool(xes.size());
  for (long long i = 0; i != n; i++) {
    tool.Add(arr[i].x, arr[i].y);
  }
  for (long long i = 0; i != m; i++) {
    if (request[i].type) {
      tool.Add(request[i].two, request[i].one);
    }
  }
  for (auto& fen : tool.arr) {
    std::sort(fen.arr.begin(), fen.arr.end());
    fen.arr.insert(fen.arr.begin(), std::make_pair(-1, (long long)(-1)));
  }

  for (long long i = 0; i != n; i++) {
    tool.Upd(arr[i].x, arr[i].y, 1);
  }
  for (long long i = 0; i != m; i++) {
    if (!request[i].type) {
      std::cout << tool.Sum(std::max(request[i].three, request[i].four),
                            std::max(request[i].one, request[i].two)) -
                       tool.Sum(std::min(request[i].three, request[i].four) - 1,
                                std::max(request[i].one, request[i].two)) -
                       tool.Sum(std::max(request[i].three, request[i].four),
                                std::min(request[i].one, request[i].two) - 1) +
                       tool.Sum(std::min(request[i].three, request[i].four) - 1,
                                std::min(request[i].one, request[i].two) - 1)
                << '\n';
    } else {
      tool.Upd(arr[request[i].one].x, arr[request[i].one].y, -1);
      arr[request[i].one].x = request[i].two;
      tool.Upd(arr[request[i].one].x, arr[request[i].one].y, 1);
    }
  }
}
