#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const long long int cInf = 1000000000000;
const long long int cSize = 2 * pow(10, 5);
std::vector<std::pair<int, int>> quotes(cSize);

long long int Heapsize(std::vector<std::pair<long long int, int>>& heap) {
  return ((long long int)heap.size());
}

void Clear(std::vector<std::pair<long long int, int>>& heap1,
           std::vector<std::pair<long long int, int>>& heap2) {
  heap1.clear();
  heap2.clear();
}

void SiftUpMin(std::vector<std::pair<long long int, int>>& heap,
               long long int i) {
  while ((i != 0) && heap[(i - 1) / 2].first > heap[i].first) {
    int a = (i - 1) / 2;
    quotes[heap[(i - 1) / 2].second].first = i;
    quotes[heap[i].second].first = a;
    std::swap(heap[(i - 1) / 2], heap[i]);
    i = (i - 1) / 2;
  }
}

void SiftDownMin(std::vector<std::pair<long long int, int>>& heap,
                 long long int i) {
  while ((Heapsize(heap) > 2 * i + 2) &&
         (std::min(heap[2 * i + 1].first, heap[2 * i + 2].first) <
          heap[i].first)) {
    long long int l = 2 * i + 1;
    long long int pudge = l;
    long long int r = 2 * i + 2;
    if (Heapsize(heap) > l && heap[l].first > heap[r].first) {
      pudge = r;
    }
    if (heap[pudge].first >= heap[i].first) {
      break;
    }
    int a = quotes[heap[i].second].first;
    quotes[heap[i].second].first = quotes[heap[pudge].second].first;
    quotes[heap[pudge].second].first = a;
    std::swap(heap[pudge], heap[i]);
    i = pudge;
  }
  if ((Heapsize(heap) <= 2 * i + 2) && (Heapsize(heap) > 2 * i + 1) &&
      heap[2 * i + 1].first < heap[i].first) {
    quotes[heap[2 * i + 1].second].first = i;
    quotes[heap[i].second].first = 2 * i + 1;
    std::swap(heap[i], heap[2 * i + 1]);
  }
}

void SiftUpMax(std::vector<std::pair<long long int, int>>& heap,
               long long int i) {
  while ((i != 0) && heap[(i - 1) / 2].first < heap[i].first) {
    int a = (i - 1) / 2;
    quotes[heap[(i - 1) / 2].second].second = i;
    quotes[heap[i].second].second = a;
    std::swap(heap[(i - 1) / 2], heap[i]);
    i = (i - 1) / 2;
  }
}

void SiftDownMax(std::vector<std::pair<long long int, int>>& heap,
                 long long int i) {
  while ((Heapsize(heap) > 2 * i + 2) &&
         (std::max(heap[2 * i + 1].first, heap[2 * i + 2].first) >
          heap[i].first)) {
    long long int l = 2 * i + 1;
    long long int pudge = l;
    long long int r = 2 * i + 2;
    if (Heapsize(heap) > l && heap[l].first < heap[r].first) {
      pudge = r;
    }
    if (heap[pudge].first <= heap[i].first) {
      break;
    }
    int a = quotes[heap[i].second].second;
    quotes[heap[i].second].second = quotes[heap[pudge].second].second;
    quotes[heap[pudge].second].second = a;
    std::swap(heap[pudge], heap[i]);
    i = pudge;
  }
  if ((Heapsize(heap) <= 2 * i + 2) && (Heapsize(heap) > 2 * i + 1) &&
      heap[2 * i + 1].first > heap[i].first) {
    quotes[heap[2 * i + 1].second].second = i;
    quotes[heap[i].second].second = 2 * i + 1;
    std::swap(heap[i], heap[2 * i + 1]);
  }
}

long long int GetMinMax(std::vector<std::pair<long long int, int>>& heap) {
  return heap[0].first;
}

std::pair<long long int, int> ExtractMin(
    std::vector<std::pair<long long int, int>>& heap) {
  std::pair<long long int, int> save = heap[0];
  quotes[heap[(long long int)heap.size() - 1].second].first = 0;
  std::swap(heap[(long long int)heap.size() - 1], heap[0]);
  heap.erase(heap.begin() + (long long int)heap.size() - 1);
  SiftDownMin(heap, 0);
  return save;
}

std::pair<long long int, int> ExtractMax(
    std::vector<std::pair<long long int, int>>& heap) {
  std::pair<long long int, int> save = heap[0];
  quotes[heap[(long long int)heap.size() - 1].second].second = 0;
  std::swap(heap[(long long int)heap.size() - 1], heap[0]);
  heap.erase(heap.begin() + (long long int)heap.size() - 1);
  SiftDownMax(heap, 0);
  return save;
}

void InsertMin(std::vector<std::pair<long long int, int>>& heap,
               std::pair<long long int, int> pudge) {
  heap.push_back(pudge);
  quotes[pudge.second].first = Heapsize(heap) - 1;
  SiftUpMin(heap, Heapsize(heap) - 1);
}
void InsertMax(std::vector<std::pair<long long int, int>>& heap,
               std::pair<long long int, int> pudge) {
  heap.push_back(pudge);
  quotes[pudge.second].second = Heapsize(heap) - 1;
  SiftUpMax(heap, Heapsize(heap) - 1);
}

void Killmax(std::vector<std::pair<long long int, int>>& heap, int k) {
  heap[k].first = cInf;
  SiftUpMax(heap, k);
  quotes[heap[(long long int)heap.size() - 1].second].second = 0;
  std::swap(heap[0], heap[(long long int)heap.size() - 1]);
  heap.erase(heap.begin() + (long long int)heap.size() - 1);
  SiftDownMax(heap, 0);
}
void Killmin(std::vector<std::pair<long long int, int>>& heap, int j) {
  heap[j].first = -1 * cInf;
  SiftUpMin(heap, j);
  quotes[heap[(long long int)heap.size() - 1].second].first = 0;
  std::swap(heap[0], heap[(long long int)heap.size() - 1]);
  heap.erase(heap.begin() + (long long int)heap.size() - 1);
  SiftDownMin(heap, 0);
}

int main() {
  long long int q;
  std::cin >> q;
  std::vector<std::pair<long long int, int>> heapmin;
  std::vector<std::pair<long long int, int>> heapmax;
  for (long long int i = 0; i < q; i++) {
    std::string ingoing;
    std::cin >> ingoing;
    if (ingoing == "get_min") {
      if (!heapmin.empty()) {
        std::cout << GetMinMax(heapmin) << '\n';
      } else {
        std::cout << "error" << '\n';
      }
    }
    if (ingoing == "get_max") {
      if (!heapmax.empty()) {
        std::cout << GetMinMax(heapmax) << '\n';
      } else {
        std::cout << "error" << '\n';
      }
    }
    if (ingoing == "extract_min") {
      if (!heapmin.empty()) {
        std::pair<long long int, int> t = ExtractMin(heapmin);
        Killmax(heapmax, quotes[t.second].second);
        std::cout << t.first << '\n';
      } else {
        std::cout << "error" << '\n';
      }
    }
    if (ingoing == "extract_max") {
      if (!heapmin.empty()) {
        std::pair<long long int, int> t = ExtractMax(heapmax);
        Killmin(heapmin, quotes[t.second].first);
        std::cout << t.first << '\n';
      } else {
        std::cout << "error" << '\n';
      }
    }
    if (ingoing == "insert") {
      long long int a;
      std::cin >> a;
      std::cout << "ok" << '\n';
      InsertMin(heapmin, {a, i});
      InsertMax(heapmax, {a, i});
    }
    if (ingoing == "clear") {
      heapmax.clear();
      heapmin.clear();
      std::cout << "ok" << '\n';
    }
    if (ingoing == "size") {
      std::cout << Heapsize(heapmin) << '\n';
    }
    // for (int j = 0; j < heapmin.size(); j++) {
    //    std::cout << heapmin[j].first << "(" <<
    //    quotes[heapmin[j].second].first<<")"<<' ';
    //}
    // std::cout << '\n';
    // for (int j = 0; j < heapmax.size(); j++) {
    //   std::cout << heapmax[j].first << "(" <<
    //   quotes[heapmax[j].second].second << ")" << ' ';
    //}
    // std::cout << '\n';
  }
}