#include <concepts>
#include <memory>
#include <type_traits>

namespace shrd_detail {
// CONCEPTS
template <typename T, typename Y>
concept is_convertible_pointer = std::convertible_to<Y*, T*>;

template <typename T, typename Y, typename Deleter>
concept good_convertible_and_deleter =
    is_convertible_pointer<T, Y> && requires(Deleter d, Y* ptr) {
      { d(ptr) } -> std::same_as<void>;
    };

enum class Action { StaySafe, DestroyValue, DestroyBlock };

// CONTROL BLOCKS
struct ControlBlock {
 public:
  // FIELDS
  size_t count_of_shared;
  size_t count_of_weak;

  // CONS/DES
  ControlBlock() : ControlBlock(1, 0) {};

  virtual ~ControlBlock() = default;

  // METHODS
  bool is_alive() const { return (count_of_shared != 0); }

  virtual void destroy(Action action) = 0;

  void minus_pointer(bool is_shared) {
    if (is_shared) {
      if (count_of_shared == 1) {
        destroy(Action::DestroyValue);
      }
      --count_of_shared;
    } else {
      --count_of_weak;
    }
    if (count_of_shared > 0 || count_of_weak > 0) {
      destroy(Action::StaySafe);
      return;
    }
    destroy(Action::DestroyBlock);
  }

  virtual void* get_managed_object_ptr() const = 0;

 private:
  ControlBlock(size_t shared, size_t weaked)
      : count_of_shared(shared), count_of_weak(weaked) {}
};

// POINTER CASE (SharedPtr made from Y*)
template <typename Y, typename Allocator = std::allocator<Y>,
          typename Deleter = std::default_delete<Y>>
struct PointerControlBlock : ControlBlock {
 public:
  // FIELDS
  Y* pointer = nullptr;
  [[no_unique_address]] Allocator alloc;
  [[no_unique_address]] Deleter delet;

  // CONS/DES
  PointerControlBlock(Y* ptr = nullptr)
      : ControlBlock(), pointer(ptr), alloc(), delet() {}

  PointerControlBlock(Y* ptr, Allocator allocator)
      : ControlBlock(), pointer(ptr), alloc(allocator), delet() {};

  PointerControlBlock(Y* ptr, Deleter deleter)
      : ControlBlock(), pointer(ptr), alloc(), delet(deleter) {};

  PointerControlBlock(Y* ptr, Allocator allocator, Deleter deleter)
      : ControlBlock(), pointer(ptr), alloc(allocator), delet(deleter) {};

  ~PointerControlBlock() = default;

  // METHODS
  virtual void destroy(Action action) final {
    if (action == Action::StaySafe) {
      return;
    }
    if (action == Action::DestroyValue) {
      delet(pointer);
      pointer = nullptr;
      return;
    }
    typename std::allocator_traits<Allocator>::template rebind_alloc<
        PointerControlBlock<Y, Allocator, Deleter>>
        finish_alloc(alloc);
    std::allocator_traits<Allocator>::template rebind_traits<
        PointerControlBlock<Y, Allocator, Deleter>>::deallocate(finish_alloc,
                                                                this, 1);
  }

  void* get_managed_object_ptr() const override final {
    return static_cast<void*>(pointer);
  }
};

// VALUE CASE (for makeShared and allocateShared case)
template <typename Y, typename Allocator = std::allocator<Y>>
struct ValueControlBlock : ControlBlock {
 public:
  // FIELDS
  Y value;
  [[no_unique_address]] Allocator alloc;
  // CONS/DES
  ValueControlBlock() = default;

  template <typename... Args>
  ValueControlBlock(Args&&... args)
      : ControlBlock(), value(std::forward<Args>(args)...), alloc() {}

  template <typename... Args>
  ValueControlBlock(Allocator allocator, Args&&... args)
      : ControlBlock(), value(std::forward<Args>(args)...), alloc(allocator) {}

  ~ValueControlBlock() = default;
  // METHODS
  virtual void destroy(Action action) final {
    if (action == Action::StaySafe) {
      return;
    }
    if (action == Action::DestroyValue) {
      typename std::allocator_traits<Allocator>::template rebind_alloc<Y>
          finish_alloc(alloc);
      std::allocator_traits<Allocator>::template rebind_traits<Y>::destroy(
          finish_alloc, &value);
      return;
    }
    typename std::allocator_traits<Allocator>::template rebind_alloc<
        ValueControlBlock<Y, Allocator>>
        finish_alloc(alloc);
    std::allocator_traits<Allocator>::template rebind_traits<
        ValueControlBlock<Y, Allocator>>::deallocate(finish_alloc, this, 1);
  }

  void* get_managed_object_ptr() const override final {
    return static_cast<void*>(const_cast<Y*>(&value));
  }
};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

// ENABLE SHARED FROM THIS
template <typename T>
class EnableSharedFromThis {
 private:
  friend SharedPtr<T>;

  template <typename Y, typename... Args>
  friend SharedPtr<Y> makeShared(Args&&... args);

  template <typename Y, typename Allocator, typename... Args>
  friend SharedPtr<Y> allocateShared(const Allocator& allocator,
                                     Args&&... args);

  // FIELDS
  WeakPtr<T> weak_;

 public:
  // METHODS
  EnableSharedFromThis& operator=(const EnableSharedFromThis&) { return *this; }

  SharedPtr<T> shared_from_this() {
    if (weak_.controller_ != nullptr) {
      return weak_.lock();
    }

    throw std::bad_weak_ptr();
  }

  SharedPtr<const T> shared_from_this() const {
    if (weak_.controller_ != nullptr) {
      return weak_.lock();
    }
    throw std::bad_weak_ptr();
  }
};

// SHARED PTR
template <typename T>
class SharedPtr {
 private:
  // FRIENDS
  template <typename Y>
  friend class SharedPtr;

  template <typename Y>
  friend class WeakPtr;

  template <typename Y, typename Allocator = std::allocator<Y>,
            typename Deleter = std::default_delete<Y>>
  using BlockAlloc =
      typename std::allocator_traits<Allocator>::template rebind_alloc<
          PointerControlBlock<Y, Allocator, Deleter>>;

  template <typename Y, typename Allocator = std::allocator<Y>,
            typename Deleter = std::default_delete<Y>>
  using BlockTraits =
      typename std::allocator_traits<Allocator>::template rebind_traits<
          PointerControlBlock<Y, Allocator, Deleter>>;

  // FIELDS
  T* pointer_;
  ControlBlock* controller_;

 public:
  // CONS/DES
  SharedPtr(T* pointer, ControlBlock* p) : pointer_(pointer), controller_(p) {}

  constexpr SharedPtr(std::nullptr_t null = nullptr) : SharedPtr(null, null) {}

  template <class Y, typename Allocator, typename Deleter>
    requires good_convertible_and_deleter<T, Y, Deleter>
  SharedPtr(Y* pointer, Deleter delet, Allocator alloc)
      : pointer_(static_cast<T*>(pointer)), controller_(nullptr) {
    BlockAlloc<Y, Allocator, Deleter> block_allocator(alloc);
    PointerControlBlock<Y, Allocator, Deleter>* controller_extra =
        BlockTraits<Y, Allocator, Deleter>::allocate(block_allocator, 1);
    new (controller_extra)
        PointerControlBlock<Y, Allocator, Deleter>(pointer, alloc, delet);
    controller_ = static_cast<ControlBlock*>(controller_extra);
    if constexpr (std::is_base_of<EnableSharedFromThis<T>, T>::value) {
      pointer_->EnableSharedFromThis<T>::weak_ = *this;
    }
  }

  template <class Y, typename Deleter>
    requires good_convertible_and_deleter<T, Y, Deleter>
  SharedPtr(Y* pointer, Deleter delet)
      : pointer_(static_cast<T*>(pointer)), controller_(nullptr) {
    BlockAlloc<Y, std::allocator<Y>, Deleter> block_allocator;
    PointerControlBlock<Y, std::allocator<Y>, Deleter>* controller_extra =
        BlockTraits<Y, std::allocator<Y>, Deleter>::allocate(block_allocator,
                                                             1);
    new (controller_extra)
        PointerControlBlock<Y, std::allocator<Y>, Deleter>(pointer, delet);
    controller_ = static_cast<ControlBlock*>(controller_extra);
    if constexpr (std::is_base_of<EnableSharedFromThis<T>, T>::value) {
      pointer_->EnableSharedFromThis<T>::weak_ = *this;
    }
  }

  template <class Y>
    requires is_convertible_pointer<T, Y>
  SharedPtr(Y* pointer)
      : pointer_(static_cast<T*>(pointer)), controller_(nullptr) {
    BlockAlloc<Y> block_allocator;
    PointerControlBlock<Y>* controller_extra =
        BlockTraits<Y>::allocate(block_allocator, 1);
    new (controller_extra) PointerControlBlock<Y>(pointer);
    controller_ = static_cast<ControlBlock*>(controller_extra);
    if constexpr (std::is_base_of<EnableSharedFromThis<T>, T>::value) {
      pointer_->EnableSharedFromThis<T>::weak_ = *this;
    }
  }

  SharedPtr(const SharedPtr& another)
      : SharedPtr(another.pointer_, another.controller_) {
    if (controller_ != nullptr) {
      ++controller_->count_of_shared;
    }
  }

  SharedPtr(SharedPtr&& another)
      : SharedPtr(another.pointer_, another.controller_) {
    another.controller_ = nullptr;
    another.pointer_ = nullptr;
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  SharedPtr(const SharedPtr<Y>& another)
      : SharedPtr(another.pointer_, another.controller_) {
    if (pointer_ != nullptr) {
      ++controller_->count_of_shared;
    }
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  SharedPtr(SharedPtr<Y>&& another)
      : SharedPtr(std::move(another.pointer_), std::move(another.controller_)) {
    another.controller_ = nullptr;
    another.pointer_ = nullptr;
  }

  template <typename Y>
  SharedPtr(const SharedPtr<Y>& another, T* alias_pointer)
      : pointer_(alias_pointer), controller_(another.controller_) {
    if (controller_ != nullptr) {
      ++controller_->count_of_shared;
    }
  }

  ~SharedPtr() {
    if (controller_ != nullptr) {
      controller_->minus_pointer(true);
    }
  }

  // METHODS AND OPS
  SharedPtr& operator=(const SharedPtr& another) {
    SharedPtr(another).swap(*this);
    return *this;
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  SharedPtr& operator=(const SharedPtr<Y>& another) {
    SharedPtr(another).swap(*this);
    return *this;
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  SharedPtr& operator=(SharedPtr<Y>&& another) {
    SharedPtr(std::move(another)).swap(*this);
    return *this;
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  void reset(Y* pointer) {
    SharedPtr<Y>(pointer).swap(*this);
  }

  template <class Y, class Deleter>
    requires is_convertible_pointer<T, Y>
  void reset(Y* pointer, Deleter del) {
    SharedPtr<T>(pointer, del).swap(*this);
  }

  template <class Y, class Allocator, class Deleter>
    requires is_convertible_pointer<T, Y>
  void reset(Y* pointer, Deleter del, Allocator alloc) {
    SharedPtr<T>(pointer, del, alloc).swap(*this);
  }

  void swap(SharedPtr& another) {
    std::swap(pointer_, another.pointer_);
    std::swap(controller_, another.controller_);
  }

  size_t use_count() const {
    if (pointer_ != nullptr) {
      return controller_->count_of_shared;
    }
    return 0;
  }

  template <typename U = T>
    requires(!std::is_void_v<U> && std::is_same_v<U, T>)
  U& operator*() {
    return *pointer_;
  }

  template <typename U = T>
    requires(!std::is_void_v<U> && std::is_same_v<U, T>)
  const U& operator*() const {
    return *pointer_;
  }

  template <typename U = T>
    requires(!std::is_void_v<U> && std::is_same_v<U, T>)
  U* operator->() {
    return pointer_;
  }

  template <typename U = T>
    requires(!std::is_void_v<U> && std::is_same_v<U, T>)
  const U* operator->() const {
    return *pointer_;
  }

  T* get() { return pointer_; }

  const T* get() const { return pointer_; }

  void reset() { SharedPtr().swap(*this); }
};

// WEAK PTR
template <typename T>
class WeakPtr {
 private:
  // FRIENDS
  template <typename Y>
  friend class SharedPtr;

  template <typename Y>
  friend class WeakPtr;

  template <typename Y>
  friend class EnableSharedFromThis;

  // ONLY FIELD
  ControlBlock* controller_;

 public:
  // CONS/DES
  WeakPtr() : controller_(nullptr) {}

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  WeakPtr(const SharedPtr<Y>& another) : controller_(another.controller_) {
    if (controller_ != nullptr) {
      ++controller_->count_of_weak;
    }
  }

  WeakPtr(const WeakPtr& another) : controller_(another.controller_) {
    if (controller_ != nullptr) {
      ++controller_->count_of_weak;
    }
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  WeakPtr(const WeakPtr<Y>& another) : controller_(another.controller_) {
    if (controller_ != nullptr) {
      ++controller_->count_of_weak;
    }
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  WeakPtr(WeakPtr<Y>&& another) : controller_(another.controller_) {
    another.controller_ = nullptr;
  }

  ~WeakPtr() {
    if (controller_ != nullptr) {
      controller_->minus_pointer(false);
    }
  }

  // METHODS AND OPS
  template <typename Y>
    requires is_convertible_pointer<T, Y>
  WeakPtr& operator=(const WeakPtr<Y>& another) {
    WeakPtr(another).swap(*this);
    return *this;
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  WeakPtr& operator=(const SharedPtr<Y>& another) {
    WeakPtr(another).swap(*this);
    return *this;
  }

  template <typename Y>
    requires is_convertible_pointer<T, Y>
  WeakPtr& operator=(WeakPtr<Y>&& another) {
    WeakPtr(std::move(another)).swap(*this);
    return *this;
  }

  SharedPtr<T> lock() const {
    if (controller_ == nullptr || !controller_->is_alive()) {
      return SharedPtr<T>();
    }
    ++controller_->count_of_shared;
    return SharedPtr<T>(static_cast<T*>(controller_->get_managed_object_ptr()),
                        controller_);
  }

  bool expired() const {
    if (controller_ != nullptr) {
      return !controller_->is_alive();
    }
    return true;
  }

  size_t use_count() const {
    if (controller_ != nullptr) {
      return controller_->count_of_shared;
    }
    return 0;
  }

  void swap(WeakPtr& another) { std::swap(controller_, another.controller_); }
};

// MAKE SHARED and ALLOCATE SHARED
template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  shrd_detail::ValueControlBlock<T>* control =
      new shrd_detail::ValueControlBlock<T, std::allocator<T>>(
          std::forward<Args>(args)...);
  SharedPtr<T> pointer(&control->value,
                       static_cast<shrd_detail::ControlBlock*>(control));
  if constexpr (std::is_base_of<EnableSharedFromThis<T>, T>::value) {
    pointer->EnableSharedFromThis<T>::weak_ = pointer;
  }
  return pointer;
}

template <typename T, typename Allocator, typename... Args>
SharedPtr<T> allocateShared(const Allocator& allocator, Args&&... args) {
  using ShareAlloc = std::allocator_traits<Allocator>::template rebind_alloc<
      shrd_detail::ValueControlBlock<T, Allocator>>;
  using ShareTraits = std::allocator_traits<Allocator>::template rebind_traits<
      shrd_detail::ValueControlBlock<T, Allocator>>;
  ShareAlloc share_alloc(allocator);
  shrd_detail::ValueControlBlock<T, Allocator>* control =
      ShareTraits::allocate(share_alloc, 1);
  ShareTraits::construct(share_alloc, control, allocator,
                         std::forward<Args>(args)...);
  SharedPtr<T> pointer(&control->value,
                       static_cast<shrd_detail::ControlBlock*>(control));
  if constexpr (std::is_base_of<EnableSharedFromThis<T>, T>::value) {
    pointer->EnableSharedFromThis<T>::weak_ = pointer;
  }
  return pointer;
}
}  // namespace shrd_detail

template <typename T>
using SharedPtr = shrd_detail::SharedPtr<T>;

template <typename T>
using WeakPtr = shrd_detail::WeakPtr<T>;

template <typename T>
using EnableSharedFromThis = shrd_detail::EnableSharedFromThis<T>;

using shrd_detail::makeShared;

using shrd_detail::allocateShared;
