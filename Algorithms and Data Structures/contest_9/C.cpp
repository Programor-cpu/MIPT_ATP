#include <algorithm>
#include <iostream>
#include <vector>

void DFS(long long ver, const std::vector<std::vector<long long>>& gr,
         std::vector<bool>& vl, std::vector<bool>& vr,
         std::vector<long long>& ml, std::vector<long long>& mr) {
  vl[ver] = true;
  for (long long to = 0; to != (long long)gr[ver].size(); ++to) {
    if (!vr[gr[ver][to]]) {
      vr[gr[ver][to]] = true;
      if (mr[gr[ver][to]] + 2 != 0 && !vl[mr[gr[ver][to]]]) {
        DFS(mr[gr[ver][to]], gr, vl, vr, ml, mr);
      }
    }
  }
}

int main() {
  long long n;
  long long k;
  long long m;
  long long ver;
  std::cin >> n >> k;
  std::vector<long long> ml(n, -2);
  std::vector<long long> mr(k, -2);
  std::vector<bool> vl(n, false);
  std::vector<bool> vr(k, false);
  std::vector<long long> left_covrik(0);
  std::vector<long long> right_covrik(0);
  std::vector<std::vector<long long>> gr(n, std::vector<long long>());
  for (long long i = 0; i != n; ++i) {
    std::cin >> m;
    gr[i].resize(m);
    for (long long j = 0; j != m; ++j) {
      std::cin >> gr[i][j];
      gr[i][j] = gr[i][j] - 1;
    }
  }
  for (long long i = 0; i != n; ++i) {
    std::cin >> ver;
    if (ver != 0) {
      ml[i] = ver - 1;
      mr[ver - 1] = i;
    }
  }
  for (long long i = 0; i != n; ++i) {
    if (0 == ml[i] + 2) {
      DFS(i, gr, vl, vr, ml, mr);
    }
  }

  for (long long i = 0; i != std::max(n, k); ++i) {
    if (i < n) {
      if (!vl[i]) {
        left_covrik.push_back(1 + i);
      }
    }
    if (i < k) {
      if (vr[i]) {
        right_covrik.push_back(1 + i);
      }
    }
  }
  std::cout << left_covrik.size() + right_covrik.size() << '\n';
  std::cout << left_covrik.size() << ' ';
  for (long long i = 0; i != (long long)left_covrik.size(); ++i) {
    std::cout << left_covrik[i] << ' ';
  }
  std::cout << '\n';
  std::cout << right_covrik.size() << ' ';
  for (long long i = 0; i != (long long)right_covrik.size(); ++i) {
    std::cout << right_covrik[i] << ' ';
  }
  std::cout << '\n';
}