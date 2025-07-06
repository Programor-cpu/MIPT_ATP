#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <list>
#include <set>
#include <stack>
#include <string>
#include <vector>

// const int cBig = 2147483647;

int mem = 0;

struct Tree {
  int value;
  int rg;
  int index;
  int number_of_heap;
  Tree* parent;
  Tree* child;
  Tree* brother;
};
using namespace std;

Tree* Merge(Tree* one, Tree* two) {
  if (two->value > one->value ||
      ((two->value == one->value) && (two->index > one->index))) {
    two->parent = one;
    two->brother = one->child;
    one->child = two;
    one->rg += 1;
    return one;
  }
  one->parent = two;
  one->brother = two->child;
  two->child = one;
  two->rg += 1;
  return two;
}

std::list<Tree*> Ordering(std::list<Tree*>& one) {
  if (one.size() < 2) {
    return one;
  }
  std::list<Tree*>::iterator iter_first = one.begin();
  std::list<Tree*>::iterator iter_second = one.begin();
  std::list<Tree*>::iterator iter_third = one.begin();
  if (one.size() == 2) {
    iter_second = iter_first;
    iter_second++;
    iter_third = one.end();
  } else {
    iter_second++;
    iter_third = iter_second;
    iter_third++;
  }
  // cout << *iter_first << ' ' << *iter_second << endl;
  while (iter_first != one.end()) {
    if (iter_second == one.end()) {
      iter_first++;
    } else if ((*iter_first)->rg < (*iter_second)->rg) {
      iter_first++;
      iter_second++;
      if (iter_third != one.end()) {
        iter_third++;
      }
    } else if (iter_third != one.end() &&
               (*iter_first)->rg == (*iter_second)->rg &&
               (*iter_second)->rg == (*iter_third)->rg) {
      iter_first++;
      iter_second++;
      iter_third++;
    } else if ((*iter_first)->rg == (*iter_second)->rg) {
      *iter_first = Merge(*iter_first, *iter_second);
      iter_second = one.erase(iter_second);
      if (iter_third != one.end()) {
        iter_third++;
      }
    }
  }
  return one;
}

std::list<Tree*> Union(std::list<Tree*>& one, std::list<Tree*>& two,
                       int number) {
  std::list<Tree*>::iterator first = one.begin();
  std::list<Tree*>::iterator second = two.begin();
  std::list<Tree*> converged;
  while (first != one.end() && second != two.end()) {
    if ((*first)->rg > (*second)->rg) {
      (*second)->number_of_heap = number;
      converged.push_back(*second);
      second++;
    } else {
      (*first)->number_of_heap = number;
      converged.push_back(*first);
      first++;
    }
  }
  while (first != one.end()) {
    (*first)->number_of_heap = number;
    converged.push_back(*first);
    first++;
  }
  while (second != two.end()) {
    // std::cout << "!" << '\n';
    (*second)->number_of_heap = number;
    converged.push_back(*second);
    second++;
  }

  converged = Ordering(converged);

  return converged;
}

Tree* Create(int v, int i) {
  Tree* new_tree = new Tree;
  mem += 1;
  // std::cout << "New " << new_tree << "\n";
  new_tree->rg = 0;
  new_tree->value = v;
  new_tree->index = i;
  new_tree->child = NULL;
  new_tree->parent = NULL;
  new_tree->brother = NULL;
  return new_tree;
}

Tree* Insert(int v, int i, std::list<Tree*>& one, const int& number) {
  Tree* new_tree = Create(v, i);
  new_tree->number_of_heap = number;
  one.push_front(new_tree);
  return new_tree;
}

void SiftUp(Tree* one, std::vector<Tree*>& inserted) {
  // cout << one << ' ' << one->parent << endl;
  while (((one->parent) != NULL) && (((one->value) < ((one->parent)->value)) ||
                                     ((one->value == (one->parent)->value) &&
                                      (one->index < (one->parent)->index)))) {
    std::swap(inserted[one->index], inserted[(one->parent)->index]);
    std::swap(one->value, (one->parent)->value);
    std::swap(one->index, (one->parent)->index);
    // std::cout << one->number_of_heap << " " << one->parent->number_of_heap <<
    // '\n';
    one = one->parent;
  }
}

Tree* UpdateMin(std::list<Tree*> roots) {
  if (roots.empty()) {
    return NULL;
  }
  Tree* m = roots.front();
  for (Tree* n : roots) {
    if (n->value < m->value ||
        ((n->value == m->value) && (n->index < m->index))) {
      m = n;
    }
  }
  return m;
}

std::list<Tree*> DeformedOfMin(Tree* root) {
  std::list<Tree*> deformed_heap;
  Tree* one = root->child;
  Tree* two;
  while (one != NULL) {
    two = one;
    // std::cout << two << '\n';
    one = one->brother;
    two->brother = NULL;
    two->parent = NULL;
    deformed_heap.push_front(two);
  }

  // std::cout << "Del " << root << '\n';
  delete root;
  mem -= 1;
  return deformed_heap;
}

void ExtractMin(std::list<Tree*>& roots, Tree* looking_for, int number,
                std::vector<Tree*>& inserted) {

  Tree* to_erase_root = nullptr;
  for (Tree* x : roots) {
    if (x->value == looking_for->value && x->index == looking_for->index) {
      to_erase_root = x;
      break;
    }
  }
  if (to_erase_root == nullptr) {

    return;
  }


  std::list<Tree*> changed;
  for (Tree* x : roots) {
    if (x != to_erase_root) {
      changed.push_back(x);
    }
  }


  inserted[to_erase_root->index] = nullptr;
  std::list<Tree*> destroyed = DeformedOfMin(to_erase_root);


  roots = Union(changed, destroyed, number);
}

void Purification(Tree* to_be_killed) {
  if (to_be_killed == NULL) {
    return;
  }
  mem -= 1;
  Purification(to_be_killed->brother);
  Purification(to_be_killed->child);
  // std::cout << "Del " << to_be_killed << '\n';
  delete to_be_killed;
}

/*void printTree(Tree* h)
{
    while (h)
    {
        std::cout << "(" << h->value << ", " << h->index << ") ";
        printTree(h->child);
        h = h->brother;
    }
}*/

/*void printHeap(std::list<Tree*> _heap)
{
    for (std::list<Tree*> ::iterator it = _heap.begin(); it != _heap.end();
it++) { printTree(*it); cout << '\n';
    }
    std::cout << '\n';
}*/

int main() {
  int amount;
  int requests;
  int opertion_type;
  int heap_number;
  int another_heap;
  int last;
  int index;
  int insert_counter = 0;
  Tree* done;
  std::cin >> amount >> requests;
  std::vector<std::list<Tree*>> arr_heaps(amount);
  std::vector<Tree*> minimums(amount);
  std::vector<Tree*> inserted;
  for (int i = 0; i != amount; i++) {
    minimums[i] = NULL;
  }
  for (int i = 0; i != requests; i++) {
    std::cin >> opertion_type;
    switch (opertion_type) {
      case 0:
        std::cin >> heap_number;
        std::cin >> last;
        done = Insert(last, insert_counter, arr_heaps[heap_number - 1],
                      heap_number);
        insert_counter++;
        inserted.push_back(done);
        minimums[heap_number - 1] = UpdateMin(arr_heaps[heap_number - 1]);
        arr_heaps[heap_number - 1] = Ordering(arr_heaps[heap_number - 1]);
        break;
      case 1:
        std::cin >> heap_number;
        std::cin >> another_heap;
        if (!arr_heaps[heap_number - 1].empty() &&
            heap_number != another_heap) {
          arr_heaps[another_heap - 1] =
              Union(arr_heaps[heap_number - 1], arr_heaps[another_heap - 1],
                    another_heap);
          arr_heaps[heap_number - 1].clear();
          minimums[another_heap - 1] = UpdateMin(arr_heaps[another_heap - 1]);
          minimums[heap_number - 1] = NULL;
        }
        break;
      case 2:
        std::cin >> index;
        if (index <= (int)inserted.size() && inserted[index - 1] != NULL) {
          inserted[index - 1]->value = -1;
          SiftUp(inserted[index - 1], inserted);
          heap_number = inserted[index - 1]->number_of_heap;
          ExtractMin(arr_heaps[heap_number - 1], inserted[index - 1],
                     heap_number, inserted);
          arr_heaps[heap_number - 1] = Ordering(arr_heaps[heap_number - 1]);
          minimums[heap_number - 1] = UpdateMin(arr_heaps[heap_number - 1]);
        }
        break;
      case 3:
        std::cin >> index;
        std::cin >> last;
        if (index <= (int)inserted.size() && inserted[index - 1] != NULL) {
          inserted[index - 1]->value = -1;
          SiftUp(inserted[index - 1], inserted);
          heap_number = inserted[index - 1]->number_of_heap;
          // std::cout << heap_number << '\n';
          ExtractMin(arr_heaps[heap_number - 1], inserted[index - 1],
                     heap_number, inserted);
          inserted[index - 1] =
              Insert(last, index - 1, arr_heaps[heap_number - 1], heap_number);
          arr_heaps[heap_number - 1] = Ordering(arr_heaps[heap_number - 1]);
          minimums[heap_number - 1] = UpdateMin(arr_heaps[heap_number - 1]);
        }
        break;
      case 4:
        std::cin >> heap_number;
        if (!arr_heaps[heap_number - 1].empty()) {
          std::cout << minimums[heap_number - 1]->value << '\n';
        }
        break;
      case 5:
        std::cin >> heap_number;
        if (!arr_heaps[heap_number - 1].empty()) {
          ExtractMin(arr_heaps[heap_number - 1], minimums[heap_number - 1],
                     heap_number, inserted);
          arr_heaps[heap_number - 1] = Ordering(arr_heaps[heap_number - 1]);
          minimums[heap_number - 1] = UpdateMin(arr_heaps[heap_number - 1]);
        }

        break;
      default:
        break;
    }
    // cout << -1 << endl;
    // printHeap(arr_heaps[heap_number - 1]);
    // std::cout << mem << '\n';
  }
  for (int i = 0; i != (int)arr_heaps.size(); i++) {
    while (!arr_heaps[i].empty()) {
      Purification(arr_heaps[i].front());
      arr_heaps[i].pop_front();
    }
  }
}
