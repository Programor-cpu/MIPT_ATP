#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>

template <size_t N>
class StackStorage {
 private:
  std::array<std::byte, N> storage;
  size_t current = 0;

 public:
  StackStorage(const StackStorage& another) = delete;
  StackStorage() = default;

  void* allocate(size_t size, size_t align) noexcept {
    void* ptr = storage.data() + current;
    size_t space = N - current;
    void* aligned = std::align(align, size, ptr, space);
    if (!aligned) {
      return nullptr;
    }
    current = N - space + size;
    return aligned;
  }
};

template <typename T, size_t N>
class StackAllocator {
 private:
  StackStorage<N>* current = nullptr;

 public:
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef void* void_pointer;
  typedef const void* const_void_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  template <typename U>

  struct rebind {
    using other = StackAllocator<U, N>;
  };

  StackStorage<N>* take_pointer() const { return current; }

  StackAllocator() { current = nullptr; };

  StackAllocator(StackStorage<N>& storage) : current(&storage) {}

  template <class U>
  StackAllocator(const StackAllocator<U, N>& another)
      : current(another.take_pointer()) {}

  ~StackAllocator() = default;
  StackAllocator& operator=(const StackAllocator<T, N>& another) {
    current = another.current;
    return *this;
  }

  T* allocate(size_t count) const {
    T* pointer =
        reinterpret_cast<T*>(current->allocate(count * sizeof(T), alignof(T)));
    return pointer;
  }

  void deallocate(T* to_deallocate, size_t size) const {
    to_deallocate += size;
    ++size;
    return;
  }

  template <typename... Args>
  void construct(T* const p, const Args&... args) const {
    void* const pointer = static_cast<void*>(p);
    new (pointer) T(args...);
  }

  template <typename U>
  void destroy(U* const to_delete) const {
    to_delete->~U();
  };

  template <typename U, size_t A>
  bool operator==(const StackAllocator<U, A>& another) const {
    return (current == another.current && std::is_same_v<U, T>);
  }

  template <typename U, size_t A>
  bool operator!=(const StackAllocator<U, A>& another) const {
    return !(*this == another);
  }

  StackAllocator& select_on_container_copy_construction() { return *this; }

  const StackAllocator& select_on_container_copy_construction() const {
    return *this;
  }
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;
  
  struct BaseNode {
    BaseNode* left = nullptr;
    BaseNode* right = nullptr;

    BaseNode(BaseNode* const left, BaseNode* const right)
        : left(left), right(right) {}
    BaseNode() : left(this), right(this) {}
    BaseNode(BaseNode&& moved)
        : left(std::move(moved.left)), right(std::move(moved.right)) {}

    BaseNode& operator=(const BaseNode& another) {
      if (this == &another) {
        return *this;
      }
      left = another.left;
      right = another.right;
      return *this;
    }
  };

  struct Node : BaseNode {
    T value;

    Node(const T& new_value, BaseNode* const left, BaseNode* const right)
        : BaseNode(left, right), value(new_value) {}
    template <typename... Args>
    Node(T&& moved, BaseNode* const left, BaseNode* const right)
        : BaseNode(left, right), value(std::move(moved)) {}
    Node() = default;
  };


  typedef typename std::allocator_traits<Allocator>::template rebind_alloc<Node>
      NodeAlloc;
  typedef
      typename std::allocator_traits<Allocator>::template rebind_traits<Node>
          AllocTraits;

  BaseNode fake_;
  size_t size_ = 0;
  [[no_unique_address]] NodeAlloc allocator_;

  Node* make_node(const T& to_push, BaseNode* const left,
                  BaseNode* const right) {
    Node* node = AllocTraits::allocate(allocator_, 1);
    try {
      AllocTraits::construct(allocator_, node, to_push, left, right);
    } catch (...) {
      AllocTraits::deallocate(allocator_, node, 1);
      throw "error";
    }
    return node;
  }
  Node* make_default_node() {
    Node* node = AllocTraits::allocate(allocator_, 1);
    try {
      AllocTraits::construct(allocator_, node);
    } catch (...) {
      AllocTraits::deallocate(allocator_, node, 1);
      throw "error";
    }
    return node;
  }
  void destroy_node(Node* const node) {
    AllocTraits::destroy(allocator_, node);
    AllocTraits::deallocate(allocator_, node, 1);
  }

 public:
  List(const Allocator& alloc = Allocator())
      : fake_(&fake_, &fake_), size_(0), allocator_(alloc){};
  List(size_t number, const Allocator& alloc = Allocator())
      : fake_(&fake_, &fake_), size_(0), allocator_(alloc) {
    try {
      for (size_t i = 0; i != number; ++i) {
        Node* current = make_default_node();
        current->left = &fake_;
        current->right = fake_.right;
        fake_.right->left = current;
        fake_.right = current;
        ++size_;
      }
    } catch (...) {
      clear();
      throw "error";
    }
  };
  List(size_t number, const T& init, const Allocator& alloc = Allocator())
      : size_(0), allocator_(alloc) {
    try {
      for (size_t i = 0; i != number; ++i) {
        push_back(init);
      }
    } catch (...) {
      clear();
      throw "error";
    }
  };
  List<T, Allocator>(const List<T, Allocator>& another) {
    try {
      allocator_ = AllocTraits::select_on_container_copy_construction(
          another.allocator_);
      for (auto i = another.cbegin(); i != another.cend(); ++i) {
        push_back(*i);
      }
    } catch (...) {
      clear();
      throw "error";
    }
  }
  ~List() { clear(); }

  List& operator=(const List<T, Allocator>& another) {
    if (this == &another) {
      return *this;
    }
    List<T, Allocator> copy(allocator_);
    try {
      for (auto i = another.cbegin(); i != another.cend(); ++i) {
        copy.push_back(*i);
      }
      if (AllocTraits::propagate_on_container_copy_assignment::value &&
          copy.allocator_ != another.allocator_) {
        copy.allocator_ = another.allocator_;
      }
    } catch (...) {
      copy.clear();
      throw "error";
    }
    swap(copy);
    return *this;
  }

  template <bool ISCONST>
  class base_iterator {
   private:
    friend class List<T, Allocator>;
    typedef typename std::conditional<ISCONST, const T*, T*>::type Pointer;
    typedef typename std::conditional<ISCONST, const T&, T&>::type Reference;
    BaseNode* current_ = nullptr;

   public:
    using difference_type = std::ptrdiff_t;
    using pointer = Pointer;
    using reference = Reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = BaseNode;

    explicit base_iterator(const BaseNode* need)
        : current_(const_cast<BaseNode*>(need)) {}

    template <bool ANOTHERCONST>
    requires(!ANOTHERCONST || ISCONST)
        base_iterator(const base_iterator<ANOTHERCONST>& other)
        : current_(other.current_) {}

    template <bool ANOTHERCONST>
    requires(!ANOTHERCONST || ISCONST) base_iterator& operator=(
        const base_iterator<ANOTHERCONST>& other);

    Reference operator*() const { return static_cast<Node*>(current_)->value; }
    Pointer operator->() const {
      return &(static_cast<Node*>(current_)->value);
    }

    base_iterator& operator++() {
      current_ = current_->right;
      return *this;
    }
    base_iterator operator++(int) {
      base_iterator temper = *this;
      current_ = current_->right;
      return temper;
    }
    base_iterator& operator--() {
      current_ = current_->left;
      return *this;
    }
    base_iterator operator--(int) {
      base_iterator temper = *this;
      current_ = current_->left;
      return temper;
    }

    bool operator==(const base_iterator& another) const {
      return current_ == another.current_;
    }

    bool operator!=(const base_iterator& another) const {
      return !(*this == another);
    }
  };
  typedef base_iterator<false> iterator;
  typedef base_iterator<true> const_iterator;

  iterator begin() { return iterator(fake_.right); }

  const_iterator begin() const { return const_iterator(fake_.right); }

  const_iterator cbegin() const { return const_iterator(fake_.right); }

  iterator end() { return iterator(&fake_); }

  const_iterator end() const { return const_iterator(&fake_); }

  const_iterator cend() const { return const_iterator(&fake_); }
  NodeAlloc get_allocator() const { return allocator_; }

  typedef typename std::reverse_iterator<base_iterator<false>> reverse_iterator;
  typedef typename std::reverse_iterator<base_iterator<true>>
      const_reverse_iterator;

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const { return rbegin(); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator crend() const { return rend(); }

  size_t size() const { return size_; }
  bool empty() const { return (size_ == 0); }

  void push_back(const T& to_push) { insert(end(), to_push); }
  void push_front(const T& to_push) { insert(begin(), to_push); }
  void pop_back() { erase(--end()); }
  void pop_front() { erase(begin()); }
  void clear() {
    while (!empty()) {
      pop_back();
    }
  }
  void erase(const_iterator to_erase) {
    if (to_erase.current_ == &fake_) {
      return;
    }
    Node* erased = static_cast<Node*>(const_cast<BaseNode*>(to_erase.current_));
    erased->left->right = erased->right;
    erased->right->left = erased->left;
    destroy_node(erased);
    --size_;
  }
  void insert(const_iterator to_insert, const T& element) {
    Node* moved = static_cast<Node*>(const_cast<BaseNode*>(to_insert.current_));
    Node* inserted = nullptr;
    try {
      inserted = make_node(element, moved->left, moved);
    } catch (...) {
      throw "error";
    }
    moved->left->right = inserted;
    moved->left = inserted;
    ++size_;
  }

  void swap(List& another) {
    std::swap(fake_, another.fake_);
    if (!another.empty()) {
        fake_.left->right = &fake_;
        fake_.right->left = &fake_;
    }
    else {
        fake_.left = &fake_;
        fake_.right = &fake_;
    }
    if (!empty()) {
        another.fake_.left->right = &another.fake_;
        another.fake_.right->left = &another.fake_;
    }
    else {
        another.fake_.left = &another.fake_;
        another.fake_.right = &another.fake_;
    }
    std::swap(size_, another.size_);
    std::swap(allocator_, another.allocator_);
}
};
