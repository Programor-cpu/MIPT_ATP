#include <cmath>
#include <iostream>
#include <vector>

void Merge(std::vector<long long int>& arr, long long int f, long long int l,
           long long int& amount) {
  long long int mid = f + (l - f) / 2;
  std::vector<long long int> merged;
  long long int s = f;
  long long int k = mid;
  while (s < mid && k < l) {
    if (arr[s] <= arr[k]) {
      merged.push_back(arr[s]);
      s += 1;
      amount += k - mid;
    } else {
      merged.push_back(arr[k]);
      k += 1;
    }
  }
  while (s < mid) {
    merged.push_back(arr[s]);
    s += 1;
    amount += l - mid;
  }

  while (k < l) {
    merged.push_back(arr[k]);
    k += 1;
  }
  for (long long int i = 0; i < (long long int)merged.size(); i++) {
    arr[f + i] = merged[i];
  }
}

void MergeSort(std::vector<long long int>& arr, long long int f,
               long long int l, long long int& amount) {
  if (l - f != 1) {
    long long int mid = f + (l - f) / 2;

    MergeSort(arr, f, mid, amount);
    MergeSort(arr, mid, l, amount);
    Merge(arr, f, l, amount);
  }
}

int main() {
  long long int n;
  long long int amount = 0;
  std::cin >> n;
  std::vector<long long int> arr(n);
  for (long long int i = 0; i < n; i++) {
    std::cin >> arr[i];
  }
  MergeSort(arr, 0, n, amount);
  std::cout << amount;
}