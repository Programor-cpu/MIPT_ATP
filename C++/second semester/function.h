#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace function_detail {
// CONCEPT
template <typename Returner, typename T, typename... Args>
concept function_suitable = requires {
  std::is_invocable_r_v<Returner, T, Args...>&&
      std::convertible_to<std::invoke_result_t<T, Args...>, Returner>;
};

// DECLARATIONS
template <bool IsMoveOnly, typename T>
class BaseFunction;

template <typename T>
struct Signature;

// BASE FUNCTION
template <bool IsMoveOnly, typename Returner, typename... Args>
class BaseFunction<IsMoveOnly, Returner(Args...)> {
 private:
  using invoke_ptr_t = Returner (*)(void*, Args...);
  using copy_or_move_ptr_t = void* (*)(void*, void*);
  using destroy_ptr_t = void (*)(void*);
  // FIELDS
  static constexpr size_t cCriticalSize = 16;
  void* func_ptr_ = nullptr;
  alignas(max_align_t) char buffer_[cCriticalSize];
  invoke_ptr_t invoker_ptr_;
  copy_or_move_ptr_t copier_ptr_;
  copy_or_move_ptr_t mover_ptr_;
  destroy_ptr_t destroyer_ptr_;
  const std::type_info* type_info_ptr_ = &typeid(void);

  // STATIC FUNCS
  template <typename Func>
  static Returner invoker(Func* f_ptr, Args... args) {
    return std::invoke(*f_ptr, std::forward<Args>(args)...);
  }

  template <typename Func>
  static Func* copier(Func* source, Func* buffer) {
    Func* to = nullptr;
    if constexpr (IsMoveOnly) {
      return to;
    } else {
      if (source != nullptr) {
        if constexpr (std::is_function_v<Func>) {
          return source;
        } else {
          if constexpr (sizeof(Func) <= cCriticalSize) {
            to = buffer;
            new (to) Func(*source);
          } else {
            to = new Func(*source);
          }
          return to;
        }
      }
    }
    return to;
  }

  template <typename Func>
  static Func* mover(Func* source, Func* buffer) {
    Func* to = nullptr;
    if constexpr (std::is_function_v<Func>) {
      return source;
    } else {
      if constexpr (sizeof(Func) <= cCriticalSize) {
        to = buffer;
        new (to) Func(std::move(*source));
      } else {
        to = source;
      }
      return to;
    }
  }

  template <typename Func>
  static void destroyer(Func* f_ptr) {
    if constexpr (!std::is_function_v<Func>) {
      if constexpr (sizeof(Func) <= cCriticalSize) {
        f_ptr->~Func();
      } else {
        delete f_ptr;
      }
    }
  }

 public:
  // CONS/DES
  BaseFunction()
      : func_ptr_(nullptr),
        invoker_ptr_(nullptr),
        copier_ptr_(nullptr),
        mover_ptr_(nullptr),
        destroyer_ptr_(nullptr),
        type_info_ptr_(&typeid(void)) {}

  template <typename Func>
  requires(function_suitable<Returner, Func, Args...>)
      BaseFunction(const Func& function)
      : invoker_ptr_(reinterpret_cast<invoke_ptr_t>(&invoker<Func>)),
        copier_ptr_(reinterpret_cast<copy_or_move_ptr_t>(&copier<Func>)),
        mover_ptr_(reinterpret_cast<copy_or_move_ptr_t>(&mover<Func>)),
        destroyer_ptr_(reinterpret_cast<destroy_ptr_t>(&destroyer<Func>)),
        type_info_ptr_(&typeid(Func)) {
    if constexpr (!std::is_function_v<Func>) {
      if constexpr (sizeof(Func) <= cCriticalSize) {
        new (buffer_) Func(function);
        func_ptr_ = buffer_;
      } else {
        func_ptr_ = new Func(function);
      }
    } else {
      func_ptr_ = reinterpret_cast<void*>(&function);
    }
  }

  template <typename Func>
  requires(function_suitable<Returner, std::remove_reference_t<Func>, Args...>)
      BaseFunction(Func&& function)
      : invoker_ptr_(reinterpret_cast<invoke_ptr_t>(
            &invoker<std::remove_reference_t<Func>>)),
        copier_ptr_(reinterpret_cast<copy_or_move_ptr_t>(
            &copier<std::remove_reference_t<Func>>)),
        mover_ptr_(reinterpret_cast<copy_or_move_ptr_t>(
            &mover<std::remove_reference_t<Func>>)),
        destroyer_ptr_(reinterpret_cast<destroy_ptr_t>(
            &destroyer<std::remove_reference_t<Func>>)),
        type_info_ptr_(&typeid(std::remove_reference_t<Func>)) {
    if constexpr (!std::is_function_v<Func>) {
      if constexpr (sizeof(std::remove_reference_t<Func>) <= cCriticalSize) {
        new (buffer_) std::remove_reference_t<Func>(
            std::forward<std::remove_reference_t<Func>>(function));
        func_ptr_ = buffer_;
      } else {
        func_ptr_ = new std::remove_reference_t<Func>(
            std::forward<std::remove_reference_t<Func>>(function));
      }
    } else {
      func_ptr_ = reinterpret_cast<void*>(&function);
    }
  }

  BaseFunction(const BaseFunction& another)
      : invoker_ptr_(another.invoker_ptr_),
        copier_ptr_(another.copier_ptr_),
        mover_ptr_(another.mover_ptr_),
        destroyer_ptr_(another.destroyer_ptr_),
        type_info_ptr_(another.type_info_ptr_) {
    func_ptr_ = copier_ptr_(another.func_ptr_, buffer_);
  }

  BaseFunction(BaseFunction&& another)
      : invoker_ptr_(another.invoker_ptr_),
        copier_ptr_(another.copier_ptr_),
        mover_ptr_(another.mover_ptr_),
        destroyer_ptr_(another.destroyer_ptr_),
        type_info_ptr_(another.type_info_ptr_) {
    func_ptr_ = mover_ptr_(another.func_ptr_, buffer_);
    another.invoker_ptr_ = nullptr;
    another.destroyer_ptr_ = nullptr;
    another.copier_ptr_ = nullptr;
    another.mover_ptr_ = nullptr;
    another.func_ptr_ = nullptr;
    another.type_info_ptr_ = &typeid(void);
  }

  ~BaseFunction() {
    if (destroyer_ptr_ != nullptr) {
      destroyer_ptr_(func_ptr_);
    }
  }

  // OPERATORS
  BaseFunction& operator=(const BaseFunction& another) {
    if (this == &another) {
      return *this;
    }
    if (destroyer_ptr_ != nullptr) {
      destroyer_ptr_(func_ptr_);
    }
    invoker_ptr_ = another.invoker_ptr_;
    copier_ptr_ = another.copier_ptr_;
    mover_ptr_ = another.mover_ptr_;
    destroyer_ptr_ = another.destroyer_ptr_;
    type_info_ptr_ = another.type_info_ptr_;
    func_ptr_ = copier_ptr_(another.func_ptr_, buffer_);
    return *this;
  }

  BaseFunction& operator=(BaseFunction&& another) {
    if (this == &another) {
      return *this;
    }
    if (destroyer_ptr_ != nullptr) {
      destroyer_ptr_(func_ptr_);
    }
    invoker_ptr_ = another.invoker_ptr_;
    copier_ptr_ = another.copier_ptr_;
    mover_ptr_ = another.mover_ptr_;
    destroyer_ptr_ = another.destroyer_ptr_;
    type_info_ptr_ = another.type_info_ptr_;
    func_ptr_ = mover_ptr_(another.func_ptr_, buffer_);
    another.invoker_ptr_ = nullptr;
    another.copier_ptr_ = nullptr;
    another.mover_ptr_ = nullptr;
    another.destroyer_ptr_ = nullptr;
    another.func_ptr_ = nullptr;
    another.type_info_ptr_ = &typeid(void);
    return *this;
  }

  template <typename Func>
  requires(function_suitable<Returner, Func, Args...>) BaseFunction& operator=(
      const Func& function) {
    if (destroyer_ptr_ != nullptr) {
      destroyer_ptr_(func_ptr_);
    }
    invoker_ptr_ = reinterpret_cast<invoke_ptr_t>(&invoker<Func>);
    copier_ptr_ = reinterpret_cast<copy_or_move_ptr_t>(&copier<Func>);
    mover_ptr_ = reinterpret_cast<copy_or_move_ptr_t>(&mover<Func>);
    destroyer_ptr_ = reinterpret_cast<destroy_ptr_t>(&destroyer<Func>);
    type_info_ptr_ = &typeid(Func);
    if constexpr (!std::is_function_v<Func>) {
      if constexpr (sizeof(Func) <= cCriticalSize) {
        new (buffer_) Func(function);
        func_ptr_ = buffer_;
      } else {
        func_ptr_ = new Func(function);
      }
    } else {
      func_ptr_ = reinterpret_cast<void*>(&function);
    }
    return *this;
  }

  template <typename Func>
  requires(function_suitable<Returner, Func, Args...>) BaseFunction& operator=(
      Func&& function) {
    if (destroyer_ptr_ != nullptr) {
      destroyer_ptr_(func_ptr_);
    }
    invoker_ptr_ =
        reinterpret_cast<invoke_ptr_t>(&invoker<std::remove_reference_t<Func>>);
    copier_ptr_ = reinterpret_cast<copy_or_move_ptr_t>(
        &copier<std::remove_reference_t<Func>>);
    mover_ptr_ = reinterpret_cast<copy_or_move_ptr_t>(
        &mover<std::remove_reference_t<Func>>);
    destroyer_ptr_ = reinterpret_cast<destroy_ptr_t>(
        &destroyer<std::remove_reference_t<Func>>);
    type_info_ptr_ = &typeid(std::remove_reference_t<Func>);
    if constexpr (!std::is_function_v<Func>) {
      if constexpr (sizeof(Func) <= cCriticalSize) {
        new (buffer_) std::remove_reference_t<Func>(
            std::forward<std::remove_reference_t<Func>>(function));
        func_ptr_ = buffer_;
      } else {
        func_ptr_ = new std::remove_reference_t<Func>(
            std::forward<std::remove_reference_t<Func>>(function));
      }
    } else {
      func_ptr_ = reinterpret_cast<void*>(&function);
    }
    return *this;
  }

  Returner operator()(Args... args) const {
    if (func_ptr_ != nullptr) {
      return invoker_ptr_(func_ptr_, std::forward<Args>(args)...);
    }
    throw std::bad_function_call();
  }

  // BOOL TIPS
  operator bool() const { return (func_ptr_ != nullptr); }

  bool operator==(std::nullptr_t) const { return (func_ptr_ == nullptr); }

  bool operator!=(std::nullptr_t) const { return (func_ptr_ != nullptr); }

  // TARGET AND INFO TIPS
  template <typename T>
  T* target() {
    if (type_info_ptr_ == &typeid(T)) {
      if constexpr (sizeof(T) <= cCriticalSize) {
        return reinterpret_cast<T*>(buffer_);
      } else {
        return reinterpret_cast<T*>(func_ptr_);
      }
    }
    return nullptr;
  }

  template <typename T>
  const T* target() const {
    return const_cast<BaseFunction*>(this)->target<T>();
  }
  const std::type_info& target_type() const noexcept { return *type_info_ptr_; }
};

template <typename Returner, typename Class, typename... Args>
struct Signature<Returner (Class::*)(Args...) const> {
  using type = Returner(Args...);
};
}  // namespace function_detail

// DECLARATIONS

template <typename T>
class Function;

template <typename T>
class MoveOnlyFunction;

// DEDUCTION

// FOR FUNCTION
template <typename Returner, typename... Args>
Function(Returner(Args...))->Function<Returner(Args...)>;

template <typename Func>
Function(Func)
    ->Function<
        typename function_detail::Signature<decltype(&Func::operator())>::type>;

// FOR MOVE_ONLY_FUNCTION
template <typename Returner, typename... Args>
MoveOnlyFunction(Returner(Args...))->MoveOnlyFunction<Returner(Args...)>;

template <typename Func>
MoveOnlyFunction(Func)
    ->MoveOnlyFunction<
        typename function_detail::Signature<decltype(&Func::operator())>::type>;

// FUNCTION
template <typename Returner, typename... Args>
class Function<Returner(Args...)>
    : public function_detail::BaseFunction<false, Returner(Args...)> {
 public:
  // CONS
  Function() : function_detail::BaseFunction<false, Returner(Args...)>(){};

  template <typename Func>
  requires(function_detail::function_suitable<Returner, Func, Args...>)
      Function(const Func& function)
      : function_detail::BaseFunction<false, Returner(Args...)>(function){};

  template <typename Func>
  requires(function_detail::function_suitable<
               Returner, std::remove_reference_t<Func>, Args...> &&
           !std::is_same_v<Function, std::remove_reference_t<Func>>)
      Function(Func&& function)
      : function_detail::BaseFunction<false, Returner(Args...)>(
            std::forward<std::remove_reference_t<Func>>(function)){};

  Function(const Function& another)
      : function_detail::BaseFunction<false, Returner(Args...)>(
            static_cast<
                const function_detail::BaseFunction<false, Returner(Args...)>&>(
                another)) {}

  Function(Function&& another)
      : function_detail::BaseFunction<false, Returner(Args...)>(
            static_cast<
                function_detail::BaseFunction<false, Returner(Args...)>&&>(
                another)){};

  // OPERATORS
  Function& operator=(const Function& another) {
    if (this == &another) {
      return *this;
    }
    function_detail::BaseFunction<false, Returner(Args...)>::operator=(
        static_cast<
            const function_detail::BaseFunction<false, Returner(Args...)>&>(
            another));
    return *this;
  }

  Function& operator=(Function&& another) {
    if (this == &another) {
      return *this;
    }
    function_detail::BaseFunction<false, Returner(Args...)>::operator=(
        static_cast<function_detail::BaseFunction<false, Returner(Args...)>&&>(
            another));
    return *this;
  }
};

template <typename Returner, typename... Args>
class MoveOnlyFunction<Returner(Args...)>
    : public function_detail::BaseFunction<true, Returner(Args...)> {
 public:
  // CONS
  MoveOnlyFunction()
      : function_detail::BaseFunction<true, Returner(Args...)>(){};

  template <typename Func>
  requires(function_detail::function_suitable<Returner, Func, Args...>)
      MoveOnlyFunction(const Func& function)
      : function_detail::BaseFunction<true, Returner(Args...)>(
            std::forward<Func>(function)){};

  template <typename Func>
  requires(function_detail::function_suitable<
           Returner, std::remove_reference_t<Func>, Args...>)
      MoveOnlyFunction(Func&& function)
      : function_detail::BaseFunction<true, Returner(Args...)>(
            std::forward<Func>(function)){};

  MoveOnlyFunction(MoveOnlyFunction&& another)
      : function_detail::BaseFunction<true, Returner(Args...)>(
            static_cast<
                function_detail::BaseFunction<true, Returner(Args...)>&&>(
                another)){};

  // OPERATORS
  MoveOnlyFunction& operator=(MoveOnlyFunction&& another) {
    if (this == &another) {
      return *this;
    }
    function_detail::BaseFunction<true, Returner(Args...)>::operator=(
        static_cast<function_detail::BaseFunction<true, Returner(Args...)>&&>(
            another));
    return *this;
  }
};

// IT WAS A GOOD RUN FOR C++ COURSE
// SPECIAL THANKS TO ILYA MESHERIN, REVIEWERS AND ETC.