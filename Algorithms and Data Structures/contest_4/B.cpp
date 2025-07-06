#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int cBig = pow(10, 9);
const int cNeutral = 0;

class AVLTree {
 private:
  class AVLTreeVer {
   public:
    AVLTreeVer* right;
    AVLTreeVer* left;
    int value;
    int h;
    AVLTreeVer(int k) : right(nullptr), left(nullptr), value(k), h(1) {}
  };
  void Destroy(AVLTreeVer* ver) {
    if (ver == nullptr) {
      return;
    }
    Destroy(ver->left);
    Destroy(ver->right);
    delete ver;
  }
  static AVLTreeVer* Spin(bool type, AVLTreeVer* one) {
    AVLTreeVer* two;
    AVLTreeVer* three;
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
  static int Height(AVLTreeVer* ver) {
    if (ver != nullptr) {
      return ver->h;
    }
    return cNeutral;
  }

  static int Balance(AVLTreeVer* node) {
    if (node != nullptr) {
      return (Height(node->left)) - (Height(node->right));
    }
    return cNeutral;
  }
  static AVLTreeVer* Insertion(int val, AVLTreeVer* ver) {
    if (ver == nullptr) {
      return new AVLTreeVer(val);
    }
    if (val == ver->value) {
      return ver;
    }
    if (ver->value > val) {
      ver->left = Insertion(val, ver->left);
    } else {
      ver->right = Insertion(val, ver->right);
    }
    ver->h = std::max(Height(ver->left), Height(ver->right)) + 1;

    if (1 < Balance(ver)) {
      if (ver->left->value < val) {
        ver->left = Spin(true, ver->left);
        return Spin(false, ver);
      }
      if (ver->left->value > val) {
        return Spin(false, ver);
      }
    }
    if (-1 > Balance(ver)) {
      if (ver->right->value < val) {
        return Spin(true, ver);
      }
      if (ver->right->value > val) {
        ver->right = Spin(false, ver->right);
        return Spin(true, ver);
      }
    }
    return ver;
  }
  AVLTreeVer* supreme_;

 public:
  AVLTree() : supreme_(nullptr) {}
  ~AVLTree() { Destroy(supreme_); }
  void Insert(int val) { supreme_ = Insertion(val, supreme_); }
  void LowerBound(int val, int& find) {
    AVLTreeVer* ver = supreme_;
    while (true) {
      if (ver == nullptr) {
        return;
      }
      if (val == ver->value) {
        find = ver->value;
        return;
      }
      if (val > ver->value) {
        ver = ver->right;
      } else {
        if (find > -1) {
          find = std::min(find, ver->value);
        } else {
          find = ver->value;
        }
        ver = ver->left;
      }
    }
  }
};

int main() {
  int q;
  std::cin >> q;
  int r;
  char req;
  AVLTree toolofchoise;
  bool flag = false;
  int prev = 0;
  while (q != 0) {
    --q;
    std::cin >> req;
    if (req == '?') {
      std::cin >> r;
      flag = true;
      prev = -1;
      toolofchoise.LowerBound(r, prev);
      std::cout << prev << '\n';
    } else {
      std::cin >> r;
      if (flag) {
        r += prev;
        r %= cBig;
        flag = false;
      }
      toolofchoise.Insert(r);
    }
  }
}