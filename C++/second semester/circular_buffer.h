#include <array>
#include <iostream>
#include <iterator>
#include <limits>
#include <type_traits>

static const size_t DYNAMIC_CAPACITY = std::numeric_limits<std::size_t>::max();

constexpr bool IsDynamic(size_t capacity) {
  return (capacity == DYNAMIC_CAPACITY);
}


template <typename T, size_t Capacity = DYNAMIC_CAPACITY>
class CircularBuffer {
 private:
  template <size_t Cap>
  struct optional_buffer_container {
   private:
    alignas(alignof(T)) std::array<std::byte[sizeof(T)], Cap + 1> arr_;

   public:
    optional_buffer_container() = default;

    optional_buffer_container(size_t size) {
      if (size != Cap) {
        throw std::invalid_argument("Not equal");
      }
    }

    size_t get_capacity() const { return Cap; }

    T* get_begin() {
      T* first = reinterpret_cast<T*>(arr_.data());
      return first;
    }

    const T* get_begin() const {
      const T* first = reinterpret_cast<const T*>(arr_.data());
      return first;
    }

    void swap(optional_buffer_container& another) { arr_.swap(another.arr_); }
    ~optional_buffer_container() = default;
  };
  template <>
  struct optional_buffer_container<DYNAMIC_CAPACITY> {
   private:
    T* arr_;
    size_t capacity_;

   public:
    optional_buffer_container() = delete;

    optional_buffer_container(size_t size)
        : arr_(reinterpret_cast<T*>(new std::byte[(size + 1) * sizeof(T)])),
          capacity_(size) {}
    size_t get_capacity() const { return capacity_; }

    T* get_begin() { return arr_; }

    const T* get_begin() const { return arr_; }

    void swap(optional_buffer_container& another) {
      std::swap(arr_, another.arr_);
      std::swap(capacity_, another.capacity_);
    }

    ~optional_buffer_container() {
      delete[] reinterpret_cast<std::byte*>(arr_);
    }
  };
  template <size_t PossibleCapacity>
  struct optional_capacity {
   public:
    optional_capacity(size_t capacity) {
      if (capacity == 0) {
        throw std::logic_error("Pointless");
      }
    }

    size_t get_capacity() const { return PossibleCapacity; }
  };
  template <>
  struct optional_capacity<DYNAMIC_CAPACITY> {
   private:
    size_t capacity_;

   public:
    optional_capacity(size_t capacity) : capacity_(capacity) {}

    size_t get_capacity() const { return capacity_; }
  };

  optional_buffer_container<Capacity> buffer_;
  size_t head_ = 0;
  size_t tail_ = 0;
  size_t size_ = 0;

 public:
  explicit CircularBuffer(size_t size) : buffer_(size) {}

  CircularBuffer() : buffer_() {}

  size_t capacity() const { return buffer_.get_capacity(); }

  size_t size() const { return size_; }

  bool empty() const { return (size_ == 0); }

  bool full() const { return (size_ == capacity()); }

  CircularBuffer(const CircularBuffer& another) : buffer_(another.capacity()) {
    try {
      for (size_t i = 0; i != another.size_; ++i) {
        push_back(another[i]);
      }
    } catch (...) {
      clear();
      throw "error";
    }
  }

  CircularBuffer& operator=(const CircularBuffer& another) {
    if (this == &another) {
      return *this;
    }
    if (Capacity == DYNAMIC_CAPACITY) {
        CircularBuffer copy(another);
        swap(copy);
    } else {
      clear();
      for (size_t i = 0; i != another.size_; ++i) {
        push_back(another[i]);
      }
    }
    return *this;
  }

  ~CircularBuffer() { clear(); };
  void clear() {
    while (!empty()) {
      pop_back();
    }
  }
  template <bool ISCONST>
  class base_iterator : private optional_capacity<Capacity> {
   private:
    typedef typename std::conditional<ISCONST, const T*, T*>::type Pointer;
    typedef typename std::conditional<ISCONST, const T&, T&>::type Reference;
    friend class base_iterator<true>;
    size_t head_;
    Pointer pointer_;
    Pointer begin_;

    size_t capacity() const {
      return static_cast<optional_capacity<Capacity>>(*this).get_capacity();
    }

   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional<ISCONST, const T, T>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = Pointer;
    using reference = Reference;
    explicit base_iterator(Pointer pointer, Pointer begin, size_t capacity,
                           size_t head)
        : optional_capacity<Capacity>(capacity),
          head_(head),
          pointer_(pointer),
          begin_(begin) {}

    base_iterator(const base_iterator<ISCONST>& other) = default;

    template <bool ANOTHERCONST>
    requires(!ANOTHERCONST || ISCONST)
        base_iterator(const base_iterator<ANOTHERCONST>& other)
        : optional_capacity<Capacity>(other.capacity()),
          head_(other.head_),
          pointer_(other.pointer_),
          begin_(other.begin_) {}

    template <bool ANOTHERCONST>
    requires(!ANOTHERCONST || ISCONST) base_iterator& operator=(
        const base_iterator<ANOTHERCONST>& other);

    base_iterator& operator++() {
      ++pointer_;
      if (begin_ + capacity() + 1 == pointer_) {
        pointer_ = begin_;
      }
      return *this;
    }

    base_iterator operator++(int) {
      base_iterator temper = *this;
      ++pointer_;
      ++*this;
      return temper;
    }

    base_iterator& operator--() {
      if (begin_ == pointer_) {
        pointer_ = begin_ + capacity();
        return *this;
      }
      --pointer_;
      return *this;
    }

    base_iterator operator--(int) {
      base_iterator temper = *this;
      --*this;
      return temper;
    }

    base_iterator& operator+=(size_t number) {
      number %= static_cast<int>(capacity() + 1);
      if (begin_ + capacity() + 1 <= pointer_ + number) {
        pointer_ += (number - capacity() - 1);
        return *this;
      }
      pointer_ += number;
      return *this;
    }

    base_iterator operator+(size_t number) const {
      base_iterator copy = *this;
      copy += number;
      return copy;
    }

    friend base_iterator operator+(size_t n, const base_iterator& it) {
      return (it + n);
    }

    base_iterator& operator-=(size_t number) {
      *this += static_cast<int>(capacity() + 1) - number;
      return *this;
    }

    base_iterator operator-(size_t number) const {
      base_iterator copy = *this;
      copy -= number;
      return copy;
    }

    difference_type operator-(const base_iterator& other) const {
      bool flag = false;
      difference_type dif = 0;
      T* first = pointer_;
      T* second = other.pointer_;
      if (first > second) {
        std::swap(first, second);
        flag = true;
      }
      T* end;
      if (begin_ + capacity() == begin_ + head_) {
        end = begin_;
      } else {
        end = begin_ + head_ + 1;
      }
      if (second > end && first <= end) {
        dif = capacity() + 1 - static_cast<difference_type>(second - first);
        flag = !flag;
      } else {
        dif = static_cast<difference_type>(second - first);
      }
      if (!flag) {
        dif *= -1;
      }
      return dif;
    }

    bool operator==(const base_iterator& other) const {
      return (pointer_ == other.pointer_);
    }

    bool operator!=(const base_iterator& other) const {
      return !(*this == other);
    }

    bool operator<(const base_iterator& other) const {
      return (*this - other)<0;
    }

    bool operator>(const base_iterator& other) const {
      return (*this - other) > 0;
    }

    bool operator<=(const base_iterator& other) const {
      return !(*this > other);
    }

    bool operator>=(const base_iterator& other) const {
      return !(*this < other);
    }

    Reference operator*() { return *pointer_; }
    Pointer operator->() { return pointer_; }
    Reference operator[](size_t pos) { return *(*this + pos); }
  };

  typedef base_iterator<false> iterator;
  typedef base_iterator<true> const_iterator;

  typedef std::reverse_iterator<base_iterator<false>> reverse_iterator;
  typedef std::reverse_iterator<base_iterator<true>> const_reverse_iterator;

 private:
  void shift_elements(iterator it) {
    iterator help = end() - 1;
    T save(*help);
    while (help != it) {
      (buffer_.get_begin() + ((tail_ + (help - begin()))) % (capacity() + 1))
          ->~T();
      new (buffer_.get_begin() +
           ((tail_ + (help - begin()))) % (capacity() + 1))
          T(*reinterpret_cast<const T*>(buffer_.get_begin() +
                                        ((tail_ + (help - begin())) - 1) %
                                            (capacity() + 1)));
      --help;
    }
    push_back(save);
  }

 public:
  iterator begin() {
    return iterator(buffer_.get_begin() + tail_, buffer_.get_begin(),
                    capacity(), head_);
  }

  const_iterator begin() const { return cbegin(); }

  const_iterator cbegin() const {
    return const_iterator(buffer_.get_begin() + tail_, buffer_.get_begin(),
                          capacity(), head_);
  }

  iterator end() {
    return iterator(buffer_.get_begin() + (head_ + 1) % (capacity() + 1),
                    buffer_.get_begin(), capacity(), head_);
  }

  const_iterator end() const { return cend(); }

  const_iterator cend() const {
    return const_iterator(buffer_.get_begin() + (head_ + 1) % (capacity() + 1),
                          buffer_.get_begin(), capacity(), head_);
  }

  reverse_iterator rbegin() {
    return (reverse_iterator(iterator(buffer_.get_begin() + tail_,
                                      buffer_.get_begin(), capacity(), head_)) +
            1);
  }

  const_reverse_iterator rbegin() const { return crbegin(); }

  const_reverse_iterator crbegin() const {
    return (const_reverse_iterator(const_iterator(buffer_.get_begin() + tail_,
                                                  buffer_.get_begin(),
                                                  capacity(), head_)) +
            1);
  }

  reverse_iterator rend() {
    return (reverse_iterator(
                iterator(buffer_.get_begin() + (head_ + 1) % (capacity() + 1),
                         buffer_.get_begin(), capacity(), head_)) -
            1);
  }

  const_reverse_iterator rend() const { return crend(); }

  const_reverse_iterator crend() const {
    return (const_reverse_iterator(const_iterator(
                buffer_.get_begin() + (head_ + 1) % (capacity() + 1),
                buffer_.get_begin(), capacity(), head_)) -
            1);
  }

  T& operator[](int index) {
    iterator need = begin();
    need += index;
    return *need;
  }

  const T& operator[](int index) const {
    const_iterator need = cbegin();
    need += index;
    return *need;
  }
  T& at(int index) {
    if (index < 0 || index >= static_cast<int>(size_)) {
      throw std::out_of_range("");
    }
    return (*this)[index];
  }

  const T& at(int index) const {
    if (index < 0 || index >= static_cast<int>(size_)) {
      throw std::out_of_range("");
    }
    return (*this)[index];
  }

  void push_back(const T& pushed) {
    size_t tail_old = tail_;
    size_t head_old = head_;
    T* save = nullptr;
    try {
      if (empty()) {
        new (buffer_.get_begin()) T(pushed);
        head_ = 0;
        tail_ = 0;
        ++size_;
        return;
      }
      if (full()) {
        save = reinterpret_cast<T*>(new std::byte[sizeof(T)]);
        new (save) T(*reinterpret_cast<const T*>(buffer_.get_begin() + tail_));
        pop_front();
        ++size_;
      }
      if (head_ == capacity()) {
        head_ = 0;
      } else {
        ++head_;
      }

      new (buffer_.get_begin() + head_) T(pushed);
      if (!full()) {
        ++size_;
      }
      if (save != nullptr) {
        save->~T();
        delete[] reinterpret_cast<std::byte*>(save);
      }
    } catch (...) {
      head_ = head_old;
      tail_ = tail_old;
      if (full()) {
        reinterpret_cast<T*>(buffer_.get_begin() + tail_)->~T();
        new (buffer_.get_begin() + tail_) T(*reinterpret_cast<const T*>(save));
      }
      if (save != nullptr) {
        save->~T();
        delete[] reinterpret_cast<std::byte*>(save);
      }
      throw "error";
    }
  }

  void pop_back() {
    if (size_ == 0) {
      throw std::logic_error("Empty already");
    }
    reinterpret_cast<T*>(buffer_.get_begin() + head_)->~T();
    if (head_ == 0) {
      head_ = capacity();
      --size_;
      return;
    }
    --head_;
    --size_;
  }

  void push_front(const T& pushed) {
    size_t tail_old = tail_;
    size_t head_old = head_;
    T* save = nullptr;
    try {
      if (empty()) {
        new (buffer_.get_begin()) T(pushed);
        head_ = 0;
        tail_ = 0;
        ++size_;
        return;
      }
      if (full()) {
        save = reinterpret_cast<T*>(new std::byte[sizeof(T)]);
        new (save) T(*reinterpret_cast<const T*>(buffer_.get_begin() + head_));
        pop_back();
        ++size_;
      }
      if (tail_ == 0) {
        tail_ = capacity();
      } else {
        --tail_;
      }
      new (buffer_.get_begin() + tail_) T(pushed);
      if (!full()) {
        ++size_;
      }
      if (save != nullptr) {
        save->~T();
        delete[] reinterpret_cast<std::byte*>(save);
      }
    } catch (...) {
      head_ = head_old;
      tail_ = tail_old;
      if (full()) {
        reinterpret_cast<T*>(buffer_.get_begin() + head_)->~T();
        new (buffer_.get_begin() + head_) T(*reinterpret_cast<const T*>(save));
      }
      if (save != nullptr) {
        save->~T();
        delete[] reinterpret_cast<std::byte*>(save);
      }
      throw "error";
    }
  }

  void pop_front() {
    if (size_ == 0) {
      throw std::logic_error("Empty already");
    }
    reinterpret_cast<T*>(buffer_.get_begin() + tail_)->~T();
    if (tail_ == capacity()) {
      tail_ = 0;
      --size_;
      return;
    }
    ++tail_;
    --size_;
  }

  void swap(CircularBuffer<T, Capacity>& another) {
    buffer_.swap(another.buffer_);
    std::swap(head_, another.head_);
    std::swap(tail_, another.tail_);
    std::swap(size_, another.size_);
  }

  void insert(iterator it, const T& inserted) {
    if (it == begin() && full()) {
      return;
    }
    if (size_ == 0 || it == end()) {
      push_back(inserted);
      return;
    }
    shift_elements(it);
    (buffer_.get_begin() + ((tail_ + (it - begin()))) % (capacity() + 1))->~T();
    new (buffer_.get_begin() + ((tail_ + (it - begin()))) % (capacity() + 1))
        T(*reinterpret_cast<const T*>(&inserted));
  }

  void erase(iterator it) {
    while (it != end() - 1) {
      std::swap(*it, *(it + 1));
      ++it;
    }
    pop_back();
  }
};
