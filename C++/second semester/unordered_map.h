#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;
  using NodeTypeNonConst = std::pair<Key, Value>;

 private:
  // NODES
  struct BaseNode {
    BaseNode* left = nullptr;
    BaseNode* right = nullptr;

    BaseNode(BaseNode* const left, BaseNode* const right)
        : left(left), right(right) {}

    BaseNode() : left(this), right(this) {}

    BaseNode(const BaseNode& moved) : left(moved.left), right(moved.right) {
      left->right = this;
      right->left = this;
    }

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
    NodeType value;
    size_t hash = 0;

    Node(const NodeType& new_value)
        : BaseNode(nullptr, nullptr), value(new_value) {}

    Node(NodeTypeNonConst&& moved)
        : BaseNode(nullptr, nullptr),
          value(std::forward<NodeTypeNonConst>(moved)) {}

    Node(NodeType&& moved)
        : BaseNode(nullptr, nullptr), value(std::move(moved)) {}

    template <typename... Args>
    Node(Args&&... args)
        : BaseNode(nullptr, nullptr), value(std::forward<Args>(args)...) {}
  };
  // NODES END
  // LIST START
  template <typename T, typename list_allocator = std::allocator<T>>
  class List {
   private:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using node_alloc =
        std::allocator_traits<list_allocator>::template rebind_alloc<Node>;
    using alloc_traits =
        std::allocator_traits<list_allocator>::template rebind_traits<Node>;
    // LIST FIELDS
    BaseNode fake_;
    size_t size_ = 0;
    [[no_unique_address]] node_alloc allocator_;
    using pair_allocator =
        std::allocator_traits<Allocator>::template rebind_alloc<NodeType>;
    using pair_traits =
        std::allocator_traits<Allocator>::template rebind_traits<NodeType>;
    using key_allocator =
        std::allocator_traits<Allocator>::template rebind_alloc<Key>;
    using key_traits =
        std::allocator_traits<Allocator>::template rebind_traits<Key>;
    using value_allocator =
        std::allocator_traits<Allocator>::template rebind_alloc<Value>;
    using value_traits =
        std::allocator_traits<Allocator>::template rebind_traits<Value>;

    [[no_unique_address]] pair_allocator pair_alloc{allocator_};
    [[no_unique_address]] key_allocator key_alloc{allocator_};
    [[no_unique_address]] value_allocator value_alloc{allocator_};

   public:
    // NODE MAKERS AND DESTROYER
    Node* make_node(const T& to_push) {
      Node* node = alloc_traits::allocate(allocator_, 1);
      try {
        alloc_traits::construct(allocator_, node, to_push);
      } catch (...) {
        alloc_traits::deallocate(allocator_, node, 1);
        throw;
      }
      return node;
    }

    Node* make_node(NodeType&& another) {
      Node* node = alloc_traits::allocate(allocator_, 1);
      try {
        alloc_traits::construct(allocator_, node,
                                std::forward<NodeType>(another));
      } catch (...) {
        alloc_traits::deallocate(allocator_, node, 1);
        throw;
      }
      return node;
    }

    template <typename... Args>
    Node* make_node_via_args(Args&&... args) {
      Node* node = alloc_traits::allocate(allocator_, 1);
      NodeType* pair_ptr = &(node->value);
      try {
        pair_traits::construct(pair_alloc, pair_ptr,
                               std::forward<Args>(args)...);
      } catch (...) {
        alloc_traits::deallocate(allocator_, node, 1);
        throw;
      }
      node->left = nullptr;
      node->right = nullptr;
      return node;
    }

    Node* make_node_via_key(const Key& key) {
      Node* node = alloc_traits::allocate(allocator_, 1);
      NodeType* pair_ptr = &(node->value);
      try {
        pair_traits::construct(pair_alloc, pair_ptr, key, Value{});
      } catch (...) {
        alloc_traits::deallocate(allocator_, node, 1);
        throw;
      }
      node->left = nullptr;
      node->right = nullptr;
      return node;
    }

    Node* make_node_via_key(Key&& key) {
      Node* node = alloc_traits::allocate(allocator_, 1);
      NodeType* pair_ptr = &(node->value);
      try {
        pair_traits::construct(pair_alloc, pair_ptr, std::move(key), Value{});
      } catch (...) {
        alloc_traits::deallocate(allocator_, node, 1);
        throw;
      }
      node->left = nullptr;
      node->right = nullptr;
      return node;
    }

    void destroy_node(Node* const node) {
      alloc_traits::destroy(allocator_, node);
      alloc_traits::deallocate(allocator_, node, 1);
    }

   public:
    // LIST CONSTRUCTORS
    List(const list_allocator& alloc = list_allocator())
        : fake_(&fake_, &fake_), size_(0), allocator_(alloc) {};

    ~List() {
      while (!empty()) {
        erase(begin());
      }
    }
    // LIST CON END
    // LIST ITERATOR AND METHODS
    template <bool ISCONST>
    class base_iterator {
     private:
      friend class List<T, list_allocator>;
      using Pointer = std::conditional<ISCONST, const T*, T*>::type;
      using Reference = std::conditional<ISCONST, const T&, T&>::type;
      BaseNode* current_ = nullptr;

     public:
      using difference_type = std::ptrdiff_t;
      using pointer = Pointer;
      using reference = Reference;
      using iterator_category = std::bidirectional_iterator_tag;
      using value_type = BaseNode;

      // CONSTRUCTORS
      base_iterator() : current_(nullptr) {}

      explicit base_iterator(const BaseNode* need)
          : current_(const_cast<BaseNode*>(need)) {}

      template <bool ANOTHERCONST>
        requires(!ANOTHERCONST || ISCONST)
      base_iterator(const base_iterator<ANOTHERCONST>& other)
          : current_(other.current_) {}

      // OPERATORS AND METHOD
      template <bool ANOTHERCONST>
        requires(!ANOTHERCONST || ISCONST)
      base_iterator& operator=(const base_iterator<ANOTHERCONST>& other);

      Reference operator*() const {
        return static_cast<Node*>(current_)->value;
      }

      Pointer operator->() const {
        return &(static_cast<Node*>(current_)->value);
      }

      Node* get_pointer() { return static_cast<Node*>(current_); }

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

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;

    // ITERATOTS METHODS
    iterator begin() { return iterator(fake_.right); }
    const_iterator begin() const { return const_iterator(fake_.right); }
    const_iterator cbegin() const { return const_iterator(fake_.right); }

    iterator end() { return iterator(&fake_); }
    iterator end_iter() const { return iterator(&fake_); }  // SPECIAL FOR FIND
    const_iterator end() const { return const_iterator(&fake_); }
    const_iterator cend() const { return const_iterator(&fake_); }

    node_alloc get_allocator() const { return allocator_; }
    // ITERATOR TIPS END
    //
    // SIZE TIPS

    size_t size() const { return size_; }
    bool empty() const { return (size_ == 0); }

    // SIZE TIPS END
    // CONTAINMENT TIPS
    // JUST A SPECIAL METHOD FOR EFFECTIVE REHASH: ACTS LIKE CUTTING DOWN NODE
    // WITHOUT DESTROYING
    Node* pop_front_extra() noexcept {
      Node* to_delete = static_cast<Node*>(fake_.right);
      if (!empty()) {
        --size_;
        fake_.right->right->left = &fake_;
        fake_.right = fake_.right->right;
      }
      return to_delete;
    }
    // ERASERS
    void erase(iterator to_erase) noexcept {
      if (to_erase.current_ == &fake_) {
        return;
      }
      Node* erased = static_cast<Node*>(to_erase.current_);
      erased->left->right = erased->right;
      erased->right->left = erased->left;
      destroy_node(erased);
      --size_;
    }
    // INSERTERS
    void insert(iterator to_emplace, Node* inserted) noexcept {
      Node* moved = nullptr;
      moved = static_cast<Node*>(to_emplace.current_);
      inserted->left = moved->left;
      inserted->right = moved;
      moved->left->right = inserted;
      moved->left = inserted;
      ++size_;
    }
  };
  // LIST END
  // MAP FIELDS
  // CONSTEXPR PARAMS
  constexpr static const size_t cDefaultBucketAmount = 5;
  constexpr static const float cDefaultMaxLoadFactor = 1;
  // USINGS
  using list_type = List<NodeType, Allocator>;
  using list_iterator = list_type::iterator;
  using list_const_iterator = list_type::const_iterator;
  using alloc_traits = std::allocator_traits<Allocator>;
  using alloc_list_iterator =
      alloc_traits::template rebind_alloc<list_iterator>;
  using list_allocator = std::allocator_traits<
      Allocator>::template rebind_alloc<List<NodeType, Allocator>>;
  using list_traits = std::allocator_traits<Allocator>::template rebind_traits<
      List<NodeType, Allocator>>;
  // FIELDS
  size_t buckets_amount_;
  float max_load_factor_;
  [[no_unique_address]] Hash hasher_;
  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]] list_allocator list_alloc_{allocator_};
  [[no_unique_address]] Equal equal_;
  List<NodeType, Allocator>* containment_;
  std::vector<list_iterator, alloc_list_iterator> buckets_begins_;
  // PRIVATE METHODS
  List<NodeType, Allocator>* make_list() {
    List<NodeType, Allocator>* list = list_traits::allocate(list_alloc_, 1);
    list_traits::construct(list_alloc_, list);
    return list;
  }

  void destroy_list(List<NodeType, Allocator>* const list) {
    list_traits::destroy(list_alloc_, list);
    list_traits::deallocate(list_alloc_, list, 1);
  }

  bool check_rehash() {
    if (max_load_factor_ >= load_factor()) {
      return false;
    }
    rehash(2 * buckets_amount_);
    return true;
  }

  void rehash(size_t space) {
    UnorderedMap<Key, Value, Hash, Equal, Allocator> rehashed(space,
                                                              allocator_);
    while (!empty()) {
      Node* node = containment_->pop_front_extra();
      rehashed.insert(node);
    }
    swap(rehashed);
  }
  // ITERATOR DECLARE
 public:
  template <bool ISCONST>
  class base_iterator;

 private:
  // INSERT READY NODES
  void insert(Node* push) noexcept {
    size_t bucket = (push->hash) % buckets_amount_;
    iterator finded(find(push->value.first));
    if (finded != end()) {
      return;
    }
    containment_->insert(buckets_begins_[bucket], push);
    --buckets_begins_[bucket];
    check_rehash();
  }
  // GET INDEX OF BUCKET
  size_t get_bucket(const Key& key) const noexcept {
    size_t bucket = hasher_(key) % buckets_amount_;
    return bucket;
  }
  // SPECIAL CONS
  UnorderedMap(size_t n, const Allocator& alloc = Allocator())
      : buckets_amount_(n),
        max_load_factor_(cDefaultMaxLoadFactor),
        hasher_(),
        allocator_(alloc),
        equal_(),
        containment_(make_list()),
        buckets_begins_(buckets_amount_, list_iterator(containment_->end()),
                        alloc) {}

 public:
  Allocator get_allocator() const { return allocator_; }
  // CONSRTUCTORS
  UnorderedMap(const Allocator& alloc = Allocator())
      : UnorderedMap(cDefaultBucketAmount, alloc) {}

  UnorderedMap(const UnorderedMap& another, const Allocator& alloc)
      : buckets_amount_(another.buckets_amount_),
        max_load_factor_(another.max_load_factor_),
        hasher_(another.hasher_),
        allocator_(alloc),
        equal_(another.equal_),
        containment_(make_list()),
        buckets_begins_(another.buckets_begins_.size(),
                        list_iterator(containment_->end()), alloc) {
    if (another.empty()) {
      return;
    }
    list_const_iterator another_iter = another.containment_->cbegin();
    while (another_iter != another.containment_->cend()) {
      insert(*another_iter);
      ++another_iter;
    }
  }

  UnorderedMap(const UnorderedMap& another)
      : UnorderedMap(another,
                     alloc_traits::select_on_container_copy_construction(
                         another.allocator_)) {}

  UnorderedMap(UnorderedMap&& another)
      : buckets_amount_(std::move(another.buckets_amount_)),
        max_load_factor_(std::move(another.max_load_factor_)),
        hasher_(std::move(another.hasher_)),
        allocator_(std::move(another.allocator_)),
        equal_(std::move(another.equal_)),
        containment_(std::move(another.containment_)),
        buckets_begins_(std::move(another.buckets_begins_)) {
    another.containment_ = nullptr;
  };

  ~UnorderedMap() {
    if (containment_ != nullptr) {
      destroy_list(containment_);
    }
  }
  // CONS END
  // OPERATOR =
  UnorderedMap& operator=(const UnorderedMap& another) {
    if (this == &another) {
      return *this;
    }
    if (alloc_traits::propagate_on_container_copy_assignment::value) {
      UnorderedMap copied(another);
      swap(copied);
      return *this;
    }
    UnorderedMap copied(another, allocator_);
    swap(copied);
    return *this;
  }

  UnorderedMap& operator=(UnorderedMap&& another) noexcept {
    if (this == &another) {
      return *this;
    }
    if constexpr (!alloc_traits::propagate_on_container_move_assignment::
                      value &&
                  allocator_ != another.allocator_) {
      UnorderedMap copied(allocator_);
      for (iterator i = another.begin(); i != another.end(); ++i) {
        copied.insert(std::move(*i));
      }
      swap(copied);
      return *this;
    }
    swap(another);
    return *this;
  }
  // OPERATOR = END
  // MAP ITERATOR
  template <bool ISCONST>
  class base_iterator {
   private:
    using Pointer = std::conditional<ISCONST, const NodeType*, NodeType*>::type;
    using Reference =
        std::conditional<ISCONST, const NodeType&, NodeType&>::type;
    using PointerNode =
        std::conditional<ISCONST, const NodeType*, NodeType*>::type;
    using list_iter =
        std::conditional<ISCONST, list_const_iterator, list_iterator>::type;

    friend class base_iterator<true>;
    friend class UnorderedMap<Key, Value, Hash, Equal, Allocator>;
    list_iter iter_;
    list_iterator get_list_iterator() const { return iter_; }

   public:
    using value_type = std::conditional_t<ISCONST, const NodeType, NodeType>;
    using iterator_category = std::forward_iterator_tag;
    using reference = Reference;
    using pointer = Pointer;
    using difference_type = std::ptrdiff_t;
    // CONSTRUCTORS
    explicit base_iterator(const list_iter& iter)
        : iter_(const_cast<list_iter&>(iter)) {}

    explicit base_iterator(list_iter& iter) : iter_(iter) {}

    template <bool ANOTHERCONST>
      requires(!ANOTHERCONST || ISCONST)
    base_iterator(const base_iterator<ANOTHERCONST>& other)
        : iter_(other.iter_) {}

    // OPERATORS
    template <bool ANOTHERCONST>
      requires(!ANOTHERCONST || ISCONST)
    base_iterator& operator=(const base_iterator<ANOTHERCONST>& other);

    base_iterator operator++(int) {
      base_iterator temper = *this;
      ++iter_;
      return temper;
    }
    base_iterator& operator++() {
      ++iter_;
      return *this;
    }

    bool operator==(const base_iterator& another) const {
      return (iter_ == another.iter_);
    }
    bool operator!=(const base_iterator& another) const {
      return (iter_ != another.iter_);
    }

    Reference operator*() const { return *(iter_); }
    Pointer operator->() const { return &(*(iter_)); }
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

  // ITERATOR METHODS
  iterator begin() { return iterator(containment_->begin()); }
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const {
    return const_iterator((containment_->cbegin()));
  }

  iterator end() { return iterator(containment_->end()); }
  const_iterator end() const { return cend(); }
  const_iterator cend() const { return const_iterator((containment_->cend())); }

  // ITERATOR END
  // SIZE ASPECTS
  size_t size() const { return containment_->size(); }
  bool empty() const { return (containment_->size() == 0); }

  size_t bucket_count() const { return buckets_amount_; }

  float load_factor() const {
    return static_cast<float>(size()) / static_cast<float>(buckets_amount_);
  }
  float max_load_factor() const { return max_load_factor_; }
  void max_load_factor(float change) { max_load_factor_ = change; }
  // SIZE ASPECTS END
  // METHODS
  // FIND
  iterator find(const Key& key) const noexcept {
    size_t bucket = get_bucket(key);
    list_iterator i = buckets_begins_[bucket];
    if (i == containment_->end_iter()) {
      return iterator(i);
    }
    size_t actual_hash = (i.get_pointer())->hash;
    while (actual_hash % buckets_amount_ == bucket) {
      if (equal_(key, i->first)) {
        return iterator(i);
      }
      if (i == containment_->end_iter()) {
        break;
      }
      ++i;
      actual_hash = (i.get_pointer())->hash;
    }
    auto j = containment_->end_iter();
    return iterator(j);
  }
  // CONTAINS (like modern map)
  bool contains(const Key& key) const noexcept { return (find(key) != cend()); }
  // ERASERS
  void erase(iterator to_erase) {
    size_t bucket = get_bucket((*to_erase).first);
    list_iterator eraser = to_erase.get_list_iterator();
    if (buckets_begins_[bucket] == eraser) {
      ++buckets_begins_[bucket];
      if (buckets_begins_[bucket] == containment_->end()) {
        containment_->erase(eraser);
        return;
      }
      if ((buckets_begins_[bucket].get_pointer()->hash) % buckets_amount_ !=
          bucket) {
        buckets_begins_[bucket] = containment_->end();
      }
      containment_->erase(eraser);
      return;
    }
    containment_->erase(eraser);
    return;
  }

  void erase(iterator begin, iterator end) {
    for (iterator it = begin; it != end;) {
      iterator eraser = it;
      ++it;
      erase(eraser);
    }
  }
  // INSERTERS
  std::pair<iterator, bool> insert(const NodeType& push) {
    size_t hash = hasher_(push.first);
    size_t bucket = hash % buckets_amount_;
    iterator finded(find(push.first));
    if (finded != end()) {
      return {finded, false};
    }
    Node* node;
    node = containment_->make_node(push);
    node->hash = hash;
    insert(node);
    return {iterator(buckets_begins_[bucket]), true};
  }
  std::pair<iterator, bool> insert(NodeType&& push) {
    size_t hash = hasher_(push.first);
    size_t bucket = hash % buckets_amount_;
    iterator finded(find(push.first));
    if (finded != end()) {
      return {finded, false};
    }
    Node* node;
    node = containment_->make_node(std::forward<NodeType>(push));
    node->hash = hash;
    insert(node);
    return {iterator(buckets_begins_[bucket]), true};
  }

  template <typename P>
    requires(!std::is_same_v<P, NodeType>)
  std::pair<iterator, bool> insert(P&& push) {
    return emplace(std::forward<P>(push));
  }

  template <typename IteratorType>
  void insert(IteratorType begin, IteratorType end) {
    for (auto iter = begin; iter != end; ++iter) {
      insert(containment_->make_node(std::forward<decltype(*iter)>(*iter)));
    }
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    Node* node = containment_->make_node_via_args(std::forward<Args>(args)...);
    node->hash = hasher_((node->value).first);
    size_t bucket = (node->hash) % buckets_amount_;
    iterator finded = find((node->value).first);
    if (finded != end()) {
      containment_->destroy_node(node);
      return {finded, false};
    }
    containment_->insert(buckets_begins_[bucket], node);
    --buckets_begins_[bucket];
    if (check_rehash()) {
      return {find((node->value).first), true};
    }
    return {iterator(buckets_begins_[bucket]), true};
  }
  //[] OPERATORS
  Value& operator[](const Key& key) {
    iterator iter = find(key);
    if (iter != end()) {
      return (*iter).second;
    }
    Node* result = containment_->make_node_via_key(key);
    result->hash = hasher_((result->value).first);
    insert(result);
    return (result->value).second;
  }

  Value& operator[](Key&& key) {
    iterator iter = find(key);
    if (iter != end()) {
      return (*iter).second;
    }
    Node* result = containment_->make_node_via_key(std::move(key));
    result->hash = hasher_((result->value).first);
    insert(result);
    return (result->value).second;
  }
  // AT
  Value& at(const Key& key) {
    iterator iter = find(key);
    if (iter == end()) {
      throw std::logic_error("Key doesn't exist");
    }
    return (*iter).second;
  }

  const Value& at(const Key& key) const {
    const_iterator iter = find(key);
    if (iter == end()) {
      throw std::logic_error("Key doesn't exist");
    }
    return (*iter).second;
  }
  // RESERVE AND SWAP
  void reserve(size_t size) {
    if (size > buckets_amount_ * max_load_factor_) {
      rehash(std::ceil(size / max_load_factor_));
    }
  }

  void swap(UnorderedMap& another) {
    if (alloc_traits::propagate_on_container_swap::value) {
      std::swap(allocator_, another.allocator_);
    }
    std::swap(equal_, another.equal_);
    std::swap(hasher_, another.hasher_);
    std::swap(buckets_amount_, another.buckets_amount_);
    std::swap(max_load_factor_, another.max_load_factor_);
    std::swap(buckets_begins_, another.buckets_begins_);
    std::swap(containment_, another.containment_);
  }
};
