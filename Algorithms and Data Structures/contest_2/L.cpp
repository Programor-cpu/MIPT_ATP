#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                     std::pair<std::pair<int, int>, std::pair<int, int>>>>
    st1;
std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                     std::pair<std::pair<int, int>, std::pair<int, int>>>>
    st2;

int Size(
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st1,
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st2) {
  return (st1.size() + st2.size());
}
void Clear(
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st1,
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st2) {
  while (!st1.empty()) {
    st1.pop();
  }
  while (!st2.empty()) {
    st2.pop();
  }
}

void Enqueue(
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st,
    std::pair<std::pair<int, int>, std::pair<int, int>> i) {
  if (st.empty()) {
    st.push({i, i});
  } else {
    std::pair<std::pair<int, int>, std::pair<int, int>> current_maximum =
        std::max(i, st.top().second);
    st.push({i, current_maximum});
  }
}
std::pair<std::pair<int, int>, std::pair<int, int>> GetMax(
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st1,
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st2) {
  std::pair<std::pair<int, int>, std::pair<int, int>> m = {{-1, -1}, {-1, -1}};
  if (!st1.empty() && !st2.empty()) {
    m = std::max(st1.top().second, st2.top().second);
  } else if (!st1.empty()) {
    m = st1.top().second;
  } else if (!st2.empty()) {
    m = st2.top().second;
  }

  return m;
}

std::pair<std::pair<int, int>, std::pair<int, int>> Front(
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st1,
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st2) {
  if (st1.empty() && st2.empty()) {
    return {{-1, -1}, {-1, -1}};
  }

  if (st2.empty()) {
    int n = st1.size();
    for (int i = 0; i < n; i++) {
      Enqueue(st2, st1.top().first);
      st1.pop();
    }
  }
  std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
            std::pair<std::pair<int, int>, std::pair<int, int>>>
      r = st2.top();
  return r.first;
}

std::pair<std::pair<int, int>, std::pair<int, int>> Dequeue(
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st1,
    std::stack<std::pair<std::pair<std::pair<int, int>, std::pair<int, int>>,
                         std::pair<std::pair<int, int>, std::pair<int, int>>>>&
        st2) {
  std::pair<std::pair<int, int>, std::pair<int, int>> n = Front(st1, st2);
  std::pair<std::pair<int, int>, std::pair<int, int>> check = {{-1, -1},
                                                               {-1, -1}};
  if (n != check) {
    st2.pop();
    return n;
  }
  return {{-1, -1}, {-1, -1}};
}

std::vector<std::vector<
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
Extra2(std::vector<std::vector<std::vector<
           std::pair<std::pair<int, int>, std::pair<int, int>>>>>& second,
       int ar, int n, int m, int k) {
  std::vector<std::vector<
      std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
      third(
          n,
          std::vector<
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>(
              m,
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>(
                  k)));
  for (int y = 0; y != m; y++) {
    for (int i = 0; i != k; i++) {
      if (ar != 1) {
        for (int j = (int)second.size() - 1; j != -1; j--) {
          Enqueue(st1, second[j][y][i]);
          if (Size(st1, st2) > ar) {
            Dequeue(st1, st2);
          }
          third[j][y][i] = GetMax(st1, st2);
        }
        Clear(st1, st2);
      } else {
        Clear(st1, st2);
        return second;
      }
    }
  }
  return third;
}

std::vector<std::vector<
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
Extra1(std::vector<std::vector<std::vector<
           std::pair<std::pair<int, int>, std::pair<int, int>>>>>& first,
       int br, int ar, int n, int m, int k) {
  std::vector<std::vector<
      std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
      second(
          n,
          std::vector<
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>(
              m,
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>(
                  k)));
  for (int y = 0; y != n; y++) {
    for (int i = 0; i != k; i++) {
      if (br != 1) {
        for (int j = (int)first[y].size() - 1; j != -1; j--) {
          Enqueue(st1, first[y][j][i]);
          if (Size(st1, st2) > br) {
            Dequeue(st1, st2);
          }
          second[y][j][i] = GetMax(st1, st2);
        }
        Clear(st1, st2);
      } else {
        Clear(st1, st2);
        return Extra2(first, ar, n, m, k);
      }
    }
  }
  return Extra2(second, ar, n, m, k);
}

std::vector<std::vector<
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
Calculate(
    std::vector<std::vector<
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>& arr,
    int ar, int br, int cr) {
  int n = (int)arr.size();
  int m = (int)arr[0].size();
  int k = (int)arr[0][0].size();
  std::vector<std::vector<
      std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
      first(
          n,
          std::vector<
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>(
              m,
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>(
                  k)));

  for (int y = 0; y != n; y++) {
    for (int i = 0; i != m; i++) {
      if (cr != 1) {
        for (int j = (int)arr[y][i].size() - 1; j != -1; j--) {
          Enqueue(st1, arr[y][i][j]);
          if (Size(st1, st2) > cr) {
            Dequeue(st1, st2);
          }
          first[y][i][j] = GetMax(st1, st2);
        }
        Clear(st1, st2);
      } else {
        Clear(st1, st2);
        return Extra1(arr, br, ar, n, m, k);
        ;
      }
    }
  }

  return Extra1(first, br, ar, n, m, k);
}

/*std::pair<int, std::pair<int, int>> Moving(
    std::vector<std::vector<
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>& map,
    std::vector<std::vector<
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>&
    parse,
    std::pair<int, std::pair<int, int>>& coords, int n, int m, int k) {
    int x = coords.first;
    int y = coords.second.first;
    int z = coords.second.second;
    std::pair<int, std::pair<int, int>> moveto = {
        parse[x][y][z].first.second,
        {parse[x][y][z].second.first, parse[x][y][z].second.second} };
    int value = map[x][y][z].first.first;
    if ((parse[x][y][z].first.second < n && parse[x][y][z].second.first < m &&
        parse[x][y][z].second.second < k) &&
        (parse[x][y][z].first.first > value)) {
        return Moving(map, parse, moveto, n, m, k);
    }
    return { x, {y, z} };
}*/

int main() {
  int n;
  int m;
  int k;
  int a;
  int b;
  int c;
  int ingoing;
  std::cin >> n >> m >> k;
  std::cin >> a >> b >> c;
  std::vector<std::vector<
      std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
      vector3d(
          n,
          std::vector<
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>(
              m,
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>(
                  k)));
  for (int i = 0; i != n; i++) {
    for (int j = 0; j != m; j++) {
      for (int w = 0; w != k; w++) {
        std::cin >> ingoing;
        vector3d[i][j][w] = {{ingoing, i}, {j, w}};
      }
    }
  }

  std::cin >> ingoing;
  int x;
  int y;
  int z;
  std::vector<std::pair<int, std::pair<int, int>>> coords;
  for (int i = 0; i != ingoing; i++) {
    std::cin >> x >> y >> z;
    coords.push_back({x, {y, z}});
  }
  /*std::vector<std::vector<
      std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
      anew3d(
          n - xmin + a,
          std::vector<
          std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>(
              m - ymin + b,
              std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>(
                  k - zmin + c)));
  for (int i = xmin; i != n; i++) {
      for (int j = ymin; j != m; j++) {
          for (int w = zmin; w != k; w++) {
              anew3d[i - xmin][j - ymin][w - zmin] = vector3d[i][j][w];
              anew3d[i - xmin][j - ymin][w - zmin].first.second -= xmin;
              anew3d[i - xmin][j - ymin][w - zmin].second.first -= ymin;
              anew3d[i - xmin][j - ymin][w - zmin].second.second -= zmin;
          }
      }
  }*/

  std::vector<std::vector<
      std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>>>
      result = Calculate(vector3d, a, b, c);

  for (int i = n - 1; i != -1; i--) {
    for (int j = m - 1; j != -1; j--) {
      for (int w = k - 1; w != -1; w--) {
        result[i][j][w] =
            result[result[i][j][w].first.second][result[i][j][w].second.first]
                  [result[i][j][w].second.second];
      }
    }
  }

  for (int i = 0; i != (int)coords.size(); i++) {
    std::cout << result[coords[i].first][coords[i].second.first]
                       [coords[i].second.second]
                           .first.second
              << " "
              << result[coords[i].first][coords[i].second.first]
                       [coords[i].second.second]
                           .second.first
              << " "
              << result[coords[i].first][coords[i].second.first]
                       [coords[i].second.second]
                           .second.second;
    std::cout << '\n';
  }
}