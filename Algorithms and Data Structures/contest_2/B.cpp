#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const long long int cMod = pow(2, 30);

int main() {
  long long int n;
  long long int k;
  long long int a;
  long long int x;
  long long int y;
  std::cin >> n;
  std::cin >> k;
  std::cin >> a;
  std::cin >> x;
  std::cin >> y;
  std::vector<long long int> heapmax(k);
  for (int i = 0; i < k; i++) {
    a = (a * x + y) % cMod;
    heapmax[i] = a;
  }
  make_heap(heapmax.begin(), heapmax.end());
  for (int i = 0; i < n - k; i++) {
    a = (a * x + y) % cMod;
    if (a < heapmax[0]) {
      heapmax[0] = a;
      make_heap(heapmax.begin(), heapmax.end());
    }
  }
  sort(heapmax.begin(), heapmax.end());
  for (int i = 0; i < k; i++) {
    std::cout << heapmax[i] << ' ';
  }
}