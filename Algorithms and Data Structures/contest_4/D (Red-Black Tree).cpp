#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>


const int cLow = -pow(10, 9) - 1;

enum Color { Red, Black };

template <typename T>
class RBTree {
 private:
  class RBTreeVer {
   public:
    T key;
    int amount;
    Color color;
    RBTreeVer* left;
    RBTreeVer* right;
    RBTreeVer* parent;
    RBTreeVer(T value)
        : key(value),
          amount(0),
          color(Red),
          left(nullptr),
          right(nullptr),
          parent(nullptr) {}
  };
  RBTreeVer* supreme_ = nullptr;
  T Right(T value) {
    RBTreeVer* ver = supreme_;
    T find = cLow;
    while (ver != nullptr) {
      if (ver->key > value) {
        if (find != cLow) {
          find = std::min(ver->key, find);
        } else {
          find = ver->key;
        }
        ver = ver->left;
      } else {
        ver = ver->right;
      }
    }
    return find;
  }
  T Left(T value) {
    RBTreeVer* ver = supreme_;
    T find = cLow;
    while (ver != nullptr) {
      if (ver->key < value) {
        if (find != cLow) {
          find = std::max(ver->key, find);
        } else {
          find = ver->key;
        }
        ver = ver->right;
      } else {
        ver = ver->left;
      }
    }
    return find;
  }
  void Spin(bool type, RBTreeVer*& ver) {
    RBTreeVer* child;
    if (type) {
      child = ver->left;
      ver->left = child->right;
      if (ver->left != nullptr) {
        ver->left->parent = ver;
      }
      child->parent = ver->parent;
      if (child->parent == nullptr) {
        supreme_ = child;
      } else if (ver == ver->parent->right) {
        ver->parent->right = child;
      } else {
        ver->parent->left = child;
      }
      child->right = ver;
      ver->parent = child;
    } else {
      child = ver->right;
      ver->right = child->left;
      if (ver->right != nullptr) {
        ver->right->parent = ver;
      }
      child->parent = ver->parent;
      if (child->parent == nullptr) {
        supreme_ = child;
      } else if (ver == ver->parent->right) {
        ver->parent->right = child;
      } else {
        ver->parent->left = child;
      }
      child->left = ver;
      ver->parent = child;
    }
    FixAmount(ver);
  }
  void FixAmount(RBTreeVer*& ver) {
    if (ver != nullptr) {
      ver->amount = 0;
      if (ver->left != nullptr) {
        ver->amount += ver->left->amount;
        ++ver->amount;
      }
      if (ver->right != nullptr) {
        ver->amount += ver->right->amount;
        ++ver->amount;
      }
      if (ver->parent != nullptr) {
        FixAmount(ver->parent);
      }
    }
  }
  void FixInsertion(RBTreeVer*& ver) {
    RBTreeVer* parent = nullptr;
    RBTreeVer* grandpa = nullptr;
    RBTreeVer* uncle = nullptr;
    while (ver != supreme_ && ver->color == ver->parent->color &&
           ver->color == Red) {
      parent = ver->parent;
      grandpa = ver->parent->parent;
      if (grandpa->left != parent) {
        uncle = grandpa->left;
        if (uncle == nullptr || uncle->color == Black) {
          if (ver == parent->left) {
            Spin(true, parent);
            ver = parent;
            parent = ver->parent;
          }
          Spin(false, grandpa);
          std::swap(grandpa->color, parent->color);
          ver = parent;
        } else {
          grandpa->color = Red;
          parent->color = Black;
          uncle->color = Black;
          ver = grandpa;
        }
      } else {
        uncle = grandpa->right;
        if (uncle == nullptr || uncle->color == Black) {
          if (ver == parent->right) {
            Spin(false, parent);
            ver = parent;
            parent = ver->parent;
          }
          Spin(true, grandpa);
          std::swap(grandpa->color, parent->color);
          ver = parent;
        } else {
          grandpa->color = Red;
          parent->color = Black;
          uncle->color = Black;
          ver = grandpa;
        }
      }
    }
    supreme_->color = Black;
  }
  RBTreeVer* Brother(RBTreeVer* ver) {
    if (ver->parent == nullptr) {
      return nullptr;
    }
    if (ver == ver->parent->left) {
      return ver->parent->right;
    }
    return ver->parent->left;
  }
  RBTreeVer* Replace(RBTreeVer* ver) {
    if (ver->left == nullptr && ver->right == nullptr) {
      return nullptr;
    }
    if (ver->left != nullptr && ver->right != nullptr) {
      ver = ver->right;
      while (ver->left != nullptr) {
        ver = ver->left;
      }
      return ver;
    }
    if (ver->left != nullptr) {
      return ver->left;
    }
    return ver->right;
  }
  void FixDeletion(RBTreeVer* ver) {
    if (supreme_ == ver) {
      return;
    }
    RBTreeVer* par = ver->parent;
    RBTreeVer* bro = Brother(ver);
    if (bro == nullptr) {
      FixDeletion(par);
    } else {
      if (bro->color == Black) {
        if (((bro->left != nullptr) && (bro->left->color == Red)) ||
            ((bro->right != nullptr) && (bro->right->color == Red))) {
          if (bro->right != nullptr && bro->right->color == Red) {
            if (bro->parent->left == bro) {
              bro->right->color = par->color;
              Spin(false, bro);
              Spin(true, par);
            } else {
              bro->right->color = bro->color;
              bro->color = par->color;
              Spin(false, par);
            }
          } else {
            if (bro->parent->left == bro) {
              bro->left->color = bro->color;
              bro->color = par->color;
              Spin(true, par);
            } else {
              bro->left->color = par->color;
              Spin(true, bro);
              Spin(false, par);
            }
          }
        } else {
          bro->color = Red;
          if (par->color == Red) {
            par->color = Black;
          } else {
            FixDeletion(par);
          }
        }
      } else {
        bro->color = Black;
        par->color = Red;
        if (bro->parent->left == bro) {
          Spin(true, par);
        } else {
          Spin(false, par);
        }
        FixDeletion(ver);
      }
    }
  }
  void DeleteVer(RBTreeVer* ver) {
    RBTreeVer* rep = Replace(ver);
    bool flag = false;
    if ((rep == nullptr || rep->color == Black) && (ver->color == Black)) {
      flag = true;
    }
    RBTreeVer* par = ver->parent;
    if (rep == nullptr) {
      if (ver == supreme_) {
        supreme_ = nullptr;
        FixAmount(supreme_);
      } else {
        if (!flag) {
          if (Brother(ver) != nullptr) {
            Brother(ver)->color = Red;
          }
        } else {
          FixDeletion(ver);
        }
        if (par->right == ver) {
          par->right = nullptr;
        } else {
          par->left = nullptr;
        }
        FixAmount(par);
      }
      delete ver;
      return;
    }
    if (ver->right == nullptr || ver->left == nullptr) {
      if (supreme_ != ver) {
        if (par->left == ver) {
          par->left = rep;
        } else {
          par->right = rep;
        }
        rep->parent = par;
        FixAmount(par);
        delete ver;
        if (!flag) {
          rep->color = Black;
        } else {
          FixDeletion(rep);
        }
      } else {
        ver->key = rep->key;
        ver->left = nullptr;
        ver->right = nullptr;
        FixAmount(ver);
        delete rep;
      }
      return;
    }
    std::swap(ver->key, rep->key);
    DeleteVer(rep);
  }

  void Destroy(RBTreeVer*& ver) {
    if (ver != nullptr) {
      if (ver->left != nullptr) {
        Destroy(ver->left);
      }
      if (ver->right != nullptr) {
        Destroy(ver->right);
      }
      delete ver;
    }
  }
  RBTreeVer* Min(RBTreeVer*& ver) {
    RBTreeVer* min = ver;
    while (min->left != nullptr) {
      min = min->left;
    }
    return min;
  }
  RBTreeVer* Max(RBTreeVer*& ver) {
    RBTreeVer* max = ver;
    while (max->right != nullptr) {
      max = max->right;
    }
    return max;
  }

 public:
  RBTree() : supreme_(nullptr) {}
  ~RBTree() { Destroy(supreme_); }
  void Insert(T value) {
    RBTreeVer* insert = new RBTreeVer(value);
    RBTreeVer* now = supreme_;
    RBTreeVer* parent = nullptr;
    while (now != nullptr) {
      parent = now;
      if (value < now->key) {
        now = now->left;
      } else if (value > now->key) {
        now = now->right;
      } else {
        delete insert;
        return;
      }
    }
    insert->parent = parent;
    if (parent != nullptr) {
      if (value < parent->key) {
        parent->left = insert;
      } else {
        parent->right = insert;
      }
      FixAmount(insert->parent);
    } else {
      supreme_ = insert;
    }
    FixInsertion(insert);
  }

  void Erase(T value) {
    RBTreeVer* look = supreme_;
    while (true) {
      if (look == nullptr) {
        return;
      }
      if (value == look->key) {
        break;
      }
      if (value > look->key) {
        look = look->right;
      } else {
        look = look->left;
      }
    }
    DeleteVer(look);
  }
  bool Exist(T value) {
    RBTreeVer* now = supreme_;
    while (true) {
      if (now == nullptr) {
        return false;
      }
      if (value == now->key) {
        return true;
      }
      if (value < now->key) {
        now = now->left;
      } else {
        now = now->right;
      }
    }
  }
  T Next(T value) { return Right(value); }
  T Prev(T value) { return Left(value); }
  T Kth(int num) {
    RBTreeVer* ver = supreme_;
    if (num > supreme_->amount + 1 || num <= 0) {
      return cLow;
    }
    while (true) {
      int sum_left = 1;
      if (ver->left != nullptr) {
        sum_left += ver->left->amount;
        sum_left += 1;
      }
      if (sum_left == num) {
        return ver->key;
      }
      if (sum_left > num) {
        ver = ver->left;
      } else {
        ver = ver->right;
        num -= sum_left;
      }
    }
  }
  T Max() { return Max(supreme_)->key; }
  T Min() { return Min(supreme_)->key; }
};
int main() {
  RBTree<int> tree;
  std::string req;
  int num = 0;
  while (std::cin >> req) {
    if (req == "insert") {
      std::cin >> num;
      tree.Insert(num);
    }
    if (req == "delete") {
      std::cin >> num;
      tree.Erase(num);
    }
    if (req == "exists") {
      std::cin >> num;
      if (tree.Exist(num)) {
        std::cout << "true" << '\n';
      } else {
        std::cout << "false" << '\n';
      }
    }
    if (req == "next") {
      std::cin >> num;
      if (tree.Next(num) == cLow) {
        std::cout << "none" << '\n';
      } else {
        std::cout << tree.Next(num) << '\n';
      }
    }
    if (req == "prev") {
      std::cin >> num;
      if (tree.Prev(num) == cLow) {
        std::cout << "none" << '\n';
      } else {
        std::cout << tree.Prev(num) << '\n';
      }
    }
    if (req == "kth") {
      std::cin >> num;
      ++num;
      if (tree.Kth(num) == cLow) {
        std::cout << "none" << '\n';
      } else {
        std::cout << tree.Kth(num) << '\n';
      }
    }
  }
}