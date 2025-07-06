#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cMin = -100000000;

void Boarding(std::vector<int>& tr, std::vector<int>& w, std::vector<int>& d,
              std::vector<int>& bd, int& bw, std::pair<int, int> inf) {
  int currentw = 0;
  int windex = inf.first;
  int type = inf.second;
  int k = (int)w.size();
  int n = (int)tr.size();
  if (windex == k) {
    for (int i = 0; i < n; i++) {
      currentw += tr[i];
    }
    if (bw > currentw) {
      bd = d;
      bw = currentw;
    }
  } else {
    if (type == 1) {
      Boarding(tr, w, d, bd, bw, {windex + 1, 1});
    }
    for (int j = 0; j < n; j++) {
      if (tr[j] - w[windex] >= 0) {
        tr[j] = tr[j] - w[windex];
        d[windex] = j;
        Boarding(tr, w, d, bd, bw, {windex + 1, type});
        tr[j] = tr[j] + w[windex];
        d[windex] = cMin;
      }
    }
    if (type == 0) {
      Boarding(tr, w, d, bd, bw, {windex + 1, 0});
    }
  }
}

void Input(int& bw, std::vector<int>& trackscapacity1,
           std::vector<int>& weights1, std::vector<int>& bcanvas,
           std::vector<int>& indexes1) {
  for (int i = 0; i < (int)trackscapacity1.size(); i++) {
    int ingoing;
    std::cin >> ingoing;
    trackscapacity1[i] = ingoing;
    bw += ingoing;
  }
  for (int i = 0; i < (int)weights1.size(); i++) {
    int ingoing;
    std::cin >> ingoing;
    weights1[i] = ingoing;
    bcanvas[i] = cMin;
    indexes1[i] = i + 1;
  }
}

void Output(const int& days1,
            const std::vector<std::vector<int>>& transportations1,
            const std::vector<std::vector<int>>& transportationsindexes1) {
  std::cout << days1 << '\n';
  for (int j = 0; j < days1; j++) {
    int howmany = 0;
    for (int i = 0; i < (int)transportations1[j].size(); i++) {
      if (transportations1[j][i] != cMin) {
        howmany += 1;
      }
    }
    std::cout << howmany << ' ';
    int u = 0;
    for (int y = 0; y < (int)transportations1[j].size(); y++) {
      if (transportations1[j][y] != cMin) {
        std::cout << transportationsindexes1[j][u] << ' '
                  << transportations1[j][y] + 1 << ' ';
        u += 1;
      }
    }
    std::cout << '\n';
  }
}

int Operations(std::vector<std::vector<int>>& transportations,
               std::vector<std::vector<int>>& transportationsindexes,
               std::pair<int, std::pair<int, int>> inf,
               std::vector<std::vector<int>> info) {
  int days = 0;
  int type = inf.first;
  int bw = inf.second.first;
  int k = inf.second.second;
  std::vector<int> trackscapacity = info[0];
  std::vector<int> weights = info[1];
  std::vector<int> indexes = info[2];
  std::vector<int> bcanvas = info[3];
  while (k != 0) {
    int amount = 0;
    std::vector<int> d = bcanvas;
    std::vector<int> bd = bcanvas;
    int a = bw;
    Boarding(trackscapacity, weights, d, bd, a, {0, type});
    days += 1;
    transportations.push_back(bd);
    std::vector<int> weightsn1;
    std::vector<int> spix1;
    std::vector<int> indexn1;

    for (int i = 0; i < k; i++) {
      if (bd[i] != cMin) {
        amount += 1;
        spix1.push_back(indexes[i]);
      } else {
        weightsn1.push_back(weights[i]);
        indexn1.push_back(indexes[i]);
      }
    }
    weights = weightsn1;
    indexes = indexn1;
    k = k - amount;
    transportationsindexes.push_back(spix1);
  }
  return days;
}

int main() {
  int n;
  while (std::cin >> n) {
    int k;
    std::cin >> k;
    int bw = 0;
    std::vector<int> trackscapacity(n);

    std::vector<int> weights(k);
    std::vector<int> indexes(k);
    std::vector<int> bcanvas(k);
    std::vector<std::vector<int>> transportations1;
    std::vector<std::vector<int>> transportations2;
    std::vector<std::vector<int>> transportationsindexes1;
    std::vector<std::vector<int>> transportationsindexes2;
    Input(bw, trackscapacity, weights, bcanvas, indexes);
    if (*std::max_element(begin(weights), end(weights)) <=
        *std::max_element(begin(trackscapacity), end(trackscapacity))) {
      int days1 =
          Operations(transportations1, transportationsindexes1, {0, {bw, k}},
                     {trackscapacity, weights, indexes, bcanvas});
      int days2 =
          Operations(transportations2, transportationsindexes2, {1, {bw, k}},
                     {trackscapacity, weights, indexes, bcanvas});
      if (days1 > days2) {
        days1 = days2;
        transportations1 = transportations2;
        transportationsindexes1 = transportationsindexes2;
      }
      Output(days1, transportations1, transportationsindexes1);
    } else {
      std::cout << -1 << "\n";
    }
  }
}