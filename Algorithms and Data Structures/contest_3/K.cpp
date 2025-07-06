#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#pragma clang optimize on

const long long cBig = 1e18;

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
  Request() : type(false), one(0), two(0) {}
};

class FenwickTreeFenwick {
 private:
  class FenwickInner {
   public:
    std::vector<std::pair<int, long long>> arr;
    FenwickInner() {}
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
  long long n_;
  std::vector<FenwickInner> arr_;
  void Add(long long x, long long y) {
    ++x;
    ++y;
    for (; x <= n_; x = (x | (x + 1))) {
      arr_[x].arr.push_back({y, 0});
    }
  }

 public:
  FenwickTreeFenwick(long long n)
      : n_(n), arr_(std::vector<FenwickInner>(n + 1)) {}
  void Upd(long long x, long long y, long long value) {
    ++x;
    ++y;
    for (; x <= n_; x = (x | (x + 1))) {
      arr_[x].Upd(y, value);
    }
  }

  long long Sum(long long x, long long y) {
    ++x;
    ++y;
    long long su = 0;
    for (; x > 0; x = (x & (x + 1)) - 1) {
      su += arr_[x].Sum(y);
    }
    return su;
  }
  void Create(std::vector<Point> arr_of_points) {
    for (long long i = 0; i != (long long)arr_of_points.size(); i++) {
      Add(arr_of_points[i].x, arr_of_points[i].y);
    }
    for (auto& fen : arr_) {
      std::sort(fen.arr.begin(), fen.arr.end());
      fen.arr.insert(fen.arr.begin(), std::make_pair(-1, (long long)(-1)));
    }
  }
};

int main() {
  long long n;
  std::cin >> n;
  std::vector<Point> arr(n);
  std::map<long long, long long> xes;
  std::map<long long, long long> ys;
  for (long long i = 0; i != n; i++) {
    std::cin >> arr[i].x >> arr[i].y >> arr[i].w;
    xes[arr[i].x];
    ys[arr[i].y];
  }

  long long m;
  std::cin >> m;
  std::vector<Request> request(m);
  for (long long i = 0; i != m; i++) {
    std::string ingoing;
    std::cin >> ingoing;
    request[i].type = (ingoing == "change");  // change - 1 get - 0
    std::cin >> request[i].one >> request[i].two;
    if (!request[i].type) {
      xes[request[i].one];
      ys[request[i].two];
    } else {
      --request[i].one;
    }
  }
  long long i = 0;
  for (auto it = xes.begin(); it != xes.end(); it++) {
    it->second = i;
    ++i;
  }
  i = 0;
  for (auto it = ys.begin(); it != ys.end(); it++) {
    it->second = i;
    ++i;
  }
  for (long long i = 0; i != n; i++) {
    arr[i].x = xes[arr[i].x];
    arr[i].y = ys[arr[i].y];
  }
  for (long long i = 0; i != m; i++) {
    if (!request[i].type) {
      request[i].one = xes[request[i].one];
      request[i].two = ys[request[i].two];
    }
  }
  FenwickTreeFenwick tool(xes.size());
  tool.Create(arr);
  for (long long i = 0; i != n; i++) {
    tool.Upd(arr[i].x, arr[i].y, arr[i].w);
  }
  for (long long i = 0; i != m; i++) {
    if (!request[i].type) {
      std::cout << tool.Sum(request[i].one, request[i].two) << '\n';
    } else {
      tool.Upd(arr[request[i].one].x, arr[request[i].one].y,
               request[i].two - arr[request[i].one].w);
      arr[request[i].one].w = request[i].two;
    }
  }
}
