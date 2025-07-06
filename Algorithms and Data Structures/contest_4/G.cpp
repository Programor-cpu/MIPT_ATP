#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const long long cOne = 16;
const long long cTwo = 32;

class Treap {
 public:
  long long number = 0;
  long long size_tree = 1;
  long long y;
  Treap* left = nullptr;
  Treap* right = nullptr;
  long long key = 0;
  Treap() {
    y = (long long)rand() + (((long long)rand()) << cOne) +
        (((long long)rand()) << cTwo);
    // std::cout << y << std::endl;
  }
};

Treap* Merge(Treap* ver_first, Treap* ver_second) {
  if (ver_first == nullptr) {
    return ver_second;
  }
  if (ver_second == nullptr) {
    return ver_first;
  }
  if (ver_first->y >= ver_second->y) {
    ver_first->right = Merge(ver_first->right, ver_second);
    ver_first->size_tree =
        1 + ver_first->right->size_tree +
        (ver_first->left != nullptr ? ver_first->left->size_tree : 0);
    return ver_first;
  }
  ver_second->left = Merge(ver_first, ver_second->left);
  ver_second->size_tree =
      1 + (ver_second->right != nullptr ? ver_second->right->size_tree : 0) +
      ver_second->left->size_tree;
  return ver_second;
}
void Split(long long spl, long long k, Treap* tree, Treap*& ver_first,
           Treap*& ver_second) {
  if (tree == nullptr) {
    ver_first = nullptr;
    ver_second = nullptr;
    return;
  }
  tree->key = k;
  if (tree->left != nullptr) {
    tree->key += tree->left->size_tree;
  }
  if (tree->key <= spl) {
    Split(spl, tree->key + 1, tree->right, tree->right, ver_second);
    ver_first = tree;
  } else {
    Split(spl, k, tree->left, ver_first, tree->left);
    ver_second = tree;
  }
  tree->size_tree = 1 + (tree->left != nullptr ? tree->left->size_tree : 0) +
                    (tree->right != nullptr ? tree->right->size_tree : 0);
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
  srand(0);
  long long n;
  long long answer = 0;
  long long length;
  std::cin >> n;
  long long q;

  std::vector<long long> lengths(n);
  Treap* root = nullptr;
  for (long long i = 0; i != n; i++) {
    std::cin >> length;
    answer += length * length;
    lengths[i] = length;
    // std::cout << vers[i].y << std::endl;
  }
  std::cout << answer << '\n';
  std::cin >> q;
  std::vector<Treap> vers(n);
  vers.reserve(n + q);
  for (long long i = 0; i != n; i++) {
    vers[i] = Treap();
    vers[i].number = lengths[i];
    if (i == 0) {
      root = vers.data();
    } else {
      root = Merge(root, &vers[i]);
    }
  }
  long long index;
  long long req;
  for (long long i = 0; i != q; i++) {
    std::cin >> req;
    if (req == 1) {
      std::cin >> index;
      --index;
      Treap* root1 = nullptr;
      Treap* root2 = nullptr;
      Treap* root3 = nullptr;
      Treap* root4 = nullptr;
      Treap* root5 = nullptr;
      Treap* root6 = nullptr;
      Treap* root7 = nullptr;
      Treap* root8 = nullptr;
      Split(index, 0, root, root6, root7);
      Split(index - 1, 0, root6, root8, root3);
      Split(index - 2, 0, root8, root1, root2);
      Split(0, 0, root7, root4, root5);
      if (root2 != nullptr) {
        answer -= (root2->number) * (root2->number);
      }
      answer -= (root3->number) * (root3->number);
      if (root4 != nullptr) {
        answer -= (root4->number) * (root4->number);
      }
      if (root2 != nullptr && root4 != nullptr) {
        root2->number += root3->number / 2;
        root4->number += (root3->number / 2 + root3->number % 2);
        answer += (root2->number) * (root2->number);
        answer += (root4->number) * (root4->number);
      } else if (root2 != nullptr) {
        root2->number += root3->number;
        answer += (root2->number) * (root2->number);
      } else {
        root4->number += root3->number;
        answer += (root4->number) * (root4->number);
      }
      root8 = Merge(root1, root2);
      root7 = Merge(root4, root5);
      root = Merge(root8, root7);
      std::cout << answer << '\n';
    } else {
      std::cin >> index;
      --index;
      Treap* root1 = nullptr;
      Treap* root2 = nullptr;
      Treap* root3 = nullptr;
      Treap* root4 = nullptr;
      Split(index, 0, root, root4, root3);
      Split(index - 1, 0, root4, root1, root2);
      vers.push_back(Treap());
      Treap* root5 = &vers[vers.size() - 1];
      answer -= (root2->number) * (root2->number);
      root5->number = root2->number / 2 + root2->number % 2;
      root2->number /= 2;
      answer += (root5->number) * (root5->number);
      answer += (root2->number) * (root2->number);
      root4 = Merge(root1, root2);
      root1 = Merge(root4, root5);
      root = Merge(root1, root3);
      std::cout << answer << '\n';
    }
  }
}