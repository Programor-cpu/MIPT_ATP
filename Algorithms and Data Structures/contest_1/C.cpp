#include <cmath>
#include <iostream>
#include <vector>

const unsigned long long cByte = 256;
const unsigned long long cEight = 8;

unsigned long long GetByte(unsigned long long n, int i) {
  for (int j = 0; j < i; j++) {
    n = n / cByte;
  }
  return n % cByte;
}

std::vector<unsigned long long> SortingByRank(
    std::vector<unsigned long long>& arr, int n) {
  unsigned long long index;
  std::vector<unsigned long long> exact = arr;
  std::vector<unsigned long long> temper(n);
  int c = 0;
  while (c != cEight) {
    std::vector<unsigned long long> nums(cByte);
    for (int i = 0; i < n; i++) {
      index = GetByte(exact[i], c);
      nums[index] += 1;
    }
    for (int i = 1; i < (int)cByte; i++) {
      nums[i] = nums[i] + nums[i - 1];
    }
    for (int i = n - 1; i > -1; i--) {
      index = GetByte(exact[i], c);
      temper[nums[index] - 1] = exact[i];
      nums[index] -= 1;
    }
    exact = temper;
    c += 1;
  }
  return exact;
}

int main() {
  int n;
  std::cin >> n;
  std::vector<unsigned long long> arr(n);
  for (int i = 0; i < n; i++) {
    unsigned long long a;
    std::cin >> a;
    arr[i] = a;
  }
  std::vector<unsigned long long> sortedarr = SortingByRank(arr, n);

  for (int i = 0; i < (int)sortedarr.size(); i++) {
    std::cout << sortedarr[i] << ' ';
  }
}