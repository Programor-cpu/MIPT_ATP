#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int cBig = pow(10, 9);
const int cNeutral = 0;

class AVLTree {
 public:
  AVLTree* right;
  AVLTree* left;
  std::string one;
  std::string two;
  int h;
  AVLTree(std::string log, std::string pass)
      : right(nullptr), left(nullptr), one(log), two(pass), h(1) {}
};
int Height(AVLTree* ver) {
  if (ver != nullptr) {
    return ver->h;
  }
  return cNeutral;
}
AVLTree* Spin(bool type, AVLTree* one) {
  AVLTree* two;
  AVLTree* three;
  if (type) {
    two = one->right;
    three = two->left;
    one->right = three;
    two->left = one;
  } else {
    two = one->left;
    three = two->right;
    one->left = three;
    two->right = one;
  }
  one->h = std::max(Height(one->left), Height(one->right));
  two->h = std::max(Height(two->left), Height(two->right));
  ++one->h;
  ++two->h;
  return two;
}

int Balance(AVLTree*& node) {
  if (node != nullptr) {
    return (Height(node->left)) - (Height(node->right));
  }
  return cNeutral;
}

AVLTree* Insert(std::string& f, std::string& s, AVLTree* ver) {
  if (ver == nullptr) {
    return new AVLTree(f, s);
  }
  if (f == ver->one) {
    return ver;
  }
  if (ver->one > f) {
    ver->left = Insert(f, s, ver->left);
  } else {
    ver->right = Insert(f, s, ver->right);
  }
  ver->h = std::max(Height(ver->left), Height(ver->right)) + 1;

  if (1 < Balance(ver)) {
    if (ver->left->one < f) {
      ver->left = Spin(true, ver->left);
      return Spin(false, ver);
    }
    if (ver->left->one > f) {
      return Spin(false, ver);
    }
  }
  if (-1 > Balance(ver)) {
    if (ver->right->one < f) {
      return Spin(true, ver);
    }
    if (ver->right->one > f) {
      ver->right = Spin(false, ver->right);
      return Spin(true, ver);
    }
  }
  return ver;
}
std::string Find(std::string& key, AVLTree* ver) {
  if (ver == nullptr) {
    return "";
  }
  if (key > ver->one) {
    return Find(key, ver->right);
  }
  if (key == ver->one) {
    return ver->two;
  }
  return Find(key, ver->left);
}

void Destroy(AVLTree* ver) {
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
  AVLTree* logpass = nullptr;
  AVLTree* passlog = nullptr;
  std::string one;
  std::string two;
  int n;
  std::cin >> n;
  for (int i = 0; i != n; i++) {
    std::cin >> one >> two;
    logpass = Insert(one, two, logpass);
    passlog = Insert(two, one, passlog);
  }
  int q;
  std::cin >> q;
  while (q != 0) {
    --q;
    std::cin >> one;
    std::cout << Find(one, logpass) + Find(one, passlog) << '\n';
  }
  Destroy(logpass);
  Destroy(passlog);
}