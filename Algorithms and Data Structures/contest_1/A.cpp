#include <cmath>
#include <iostream>
#include <vector>

void Merge(std::vector<std::pair<int, int>>& arr, int f, int l) {
  int mid = f + (l - f) / 2;
  std::vector<std::pair<int, int>> merged;
  int s = f;
  int k = mid;
  while (s < mid && k < l) {
    if (arr[s].first < arr[k].first) {
      merged.push_back(arr[s]);
      s += 1;
    } else {
      merged.push_back(arr[k]);
      k += 1;
    }
  }
  while (s < mid) {
    merged.push_back(arr[s]);
    s += 1;
  }

  while (k < l) {
    merged.push_back(arr[k]);
    k += 1;
  }
  for (int i = 0; i < (int)merged.size(); i++) {
    arr[f + i] = merged[i];
  }
}

void MergeSort(std::vector<std::pair<int, int>>& arr, int f, int l) {
  if (l - f != 1) {
    int mid = f + (l - f) / 2;

    MergeSort(arr, f, mid);
    MergeSort(arr, mid, l);
    Merge(arr, f, l);
  }
}

std::vector<std::pair<int, int>> Unification(
    std::vector<std::pair<int, int>>& arr, int n) {
  std::pair<int, int> counter;
  std::vector<std::pair<int, int>> unified;
  int k = 0;
  while (k != n - 1) {
    if (arr[k].second >= arr[k + 1].first) {
      arr[k + 1].second = std::max(arr[k + 1].second, arr[k].second);
      arr[k + 1].first = arr[k].first;
      if (k == n - 2) {
        counter.first = arr[k + 1].first;
        counter.second = arr[k + 1].second;
        unified.push_back(arr[k + 1]);
      }
    } else {
      counter.first = arr[k].first;
      counter.second = arr[k].second;
      unified.push_back(arr[k]);
    }
    k += 1;
  }
  if (arr[n - 1].first > counter.second) {
    unified.push_back(arr[n - 1]);
  }
  return unified;
}

int main() {
  int n;
  std::cin >> n;
  std::vector<std::pair<int, int>> arr(n);
  for (int i = 0; i < n; i++) {
    std::cin >> arr[i].first;
    std::cin >> arr[i].second;
  }
  MergeSort(arr, 0, n);
  std::vector<std::pair<int, int>> unified = Unification(arr, n);

  std::cout << (int)unified.size() << '\n';
  for (int i = 0; i < (int)unified.size(); i++) {
    std::cout << unified[i].first << " " << unified[i].second << '\n';
  }
}