#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#pragma clang optimize on

class Treap {
 public:
  int number = 0;
  int x = 0;
  int y = 0;
  Treap* left = nullptr;
  Treap* right = nullptr;
  Treap* up = nullptr;
  Treap(int one, int two, int n)
      : number(n), x(one), y(two), left(nullptr), right(nullptr) {}
};

void Build(Treap*& tree, std::vector<std::pair<int, int>>& coords,
           std::vector<Treap*>& vers) {
  Treap* ongoing = tree;
  for (int i = 1; i != (int)coords.size(); i++) {
    if (coords[i].second <= ongoing->y) {
      Treap* get = ongoing;
      while (vers[i]->y <= get->y) {
        if (get->up == nullptr) {
          break;
        }
        get = get->up;
      }
      ongoing = vers[i];
      if (vers[i]->y < get->y) {
        ongoing->left = get;
        get->up = ongoing;
      } else {
        ongoing->left = get->right;
        get->right->up = ongoing;
        ongoing->up = get;
        get->right = ongoing;
      }
    } else {
      ongoing->right = vers[i];
      ongoing->right->up = ongoing;
      ongoing = ongoing->right;
    }
  }
}

void Destroy(Treap* ver) {
  if (ver == nullptr) {
    return;
  }
  Destroy(ver->left);
  Destroy(ver->right);
  delete ver;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  int n;
  std::cin >> n;
  std::vector<Treap*> vers(n);
  std::vector<std::pair<int, int>> coords(n);
  for (int i = 0; i != n; ++i) {
    std::cin >> coords[i].first >> coords[i].second;
    vers[i] = new Treap(coords[i].first, coords[i].second, i + 1);
  }
  Treap* tree = vers[0];
  std::cout << "YES" << '\n';
  Build(tree, coords, vers);
  int statusup = 0;
  int statusl = 0;
  int statusr = 0;
  for (int i = 0; i != n; i++) {
    if (vers[i]->left != nullptr) {
      statusl = vers[i]->left->number;
    } else {
      statusl = 0;
    }
    if (vers[i]->right != nullptr) {
      statusr = vers[i]->right->number;
    } else {
      statusr = 0;
    }
    if (vers[i]->up != nullptr) {
      statusup = vers[i]->up->number;
    } else {
      statusup = 0;
    }
    std::cout << statusup << ' ' << statusl << ' ' << statusr << '\n';
  }
  while (tree->up != nullptr) {
    tree = tree->up;
  }
  Destroy(tree);
}