#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

// DECLARATIONS

template <class... Types>
class Tuple;

template <typename Head, typename... Tail>
class Tuple<Head, Tail...>;

// TRIVIAL CASE

template <>
class Tuple<> {
 private:
  friend bool operator==(const Tuple<>&, const Tuple<>&) { return true; }
  friend bool operator<(const Tuple<>&, const Tuple<>&) { return false; }
  template <typename... UTypes>
  friend class Tuple;

 public:
  constexpr static size_t size() { return 0; }
};

// TUPLE SERVICE NAMESPACE

namespace tuple_detail {

// CONSTEXPR CHECKS

template <typename TupleOne, typename UHead, typename... UTail>
constexpr bool is_tuple_constructible_from_args =
    TupleOne::size() == 1 + sizeof...(UTail) && TupleOne::size() >= 1 &&
    std::is_constructible_v<typename decltype(TupleOne::head_type())::type,
                            UHead>&& []() {
      if constexpr (sizeof...(UTail) == 0) {
        return true;
      } else {
        return is_tuple_constructible_from_args<
            typename decltype(TupleOne::tail_type())::type, UTail...>;
      }
    }();

template <typename Tuple, typename UHead, typename... UTail>
constexpr bool is_tuple_implicitly_convertible_from_args =
    Tuple::size() == 1 + sizeof...(UTail) &&
    std::is_convertible_v<typename decltype(Tuple::head_type())::type,
                          UHead>&& []() {
      if constexpr (sizeof...(UTail) == 0) {
        return true;
      } else {
        return is_tuple_implicitly_convertible_from_args<
            typename decltype(Tuple::tail_type())::type, UTail...>;
      }
    }();

template <typename TupleOne, typename TupleTwo>
constexpr bool is_tuple_constructible_from_tuple =
    TupleOne::size() == TupleTwo::size() &&
    std::is_constructible_v<
        typename decltype(TupleOne::head_type())::type,
        typename decltype(TupleTwo::head_type())::type>&& []() {
      if constexpr (TupleOne::size() == 1) {
        return true;
      } else {
        return is_tuple_constructible_from_tuple<
            typename decltype(TupleOne::tail_type())::type,
            typename decltype(TupleTwo::tail_type())::type>;
      }
    }();

template <typename TupleOne, typename TupleTwo>
constexpr bool is_tuple_convertible_from_tuple = std::is_convertible_v<
    typename decltype(TupleTwo::head_type())::type,
    typename decltype(TupleOne::head_type())::type>&& []() {
  if constexpr (TupleOne::size() == 1) {
    return true;
  } else {
    return is_tuple_convertible_from_tuple<
        typename decltype(TupleOne::tail_type())::type,
        typename decltype(TupleTwo::tail_type())::type>;
  }
}();

template <typename TupleOne, typename TupleTwo>
constexpr bool is_tuple_assigable_from_tuple_copy =
    TupleOne::size() == TupleTwo::size() &&
    std::is_assignable_v<
        typename decltype(TupleOne::head_type())::type&,
        const typename decltype(TupleTwo::head_type())::type&>&& []() {
      if constexpr (TupleOne::size() == 1) {
        return true;
      } else {
        return is_tuple_assigable_from_tuple_copy<
            typename decltype(TupleOne::tail_type())::type,
            typename decltype(TupleTwo::tail_type())::type>;
      }
    }();

template <typename TupleOne, typename TupleTwo>
constexpr bool is_tuple_assigable_from_tuple_move =
    TupleOne::size() == TupleTwo::size() &&
    std::is_assignable_v<
        typename decltype(TupleOne::head_type())::type&,
        typename decltype(TupleTwo::head_type())::type>&& []() {
      if constexpr (TupleOne::size() == 1) {
        return true;
      } else {
        return is_tuple_assigable_from_tuple_move<
            typename decltype(TupleOne::tail_type())::type,
            typename decltype(TupleTwo::tail_type())::type>;
      }
    }();

// CONCEPTS

template <typename... Types>
concept is_default_constructible = (std::is_default_constructible_v<Types> &&
                                    ...);

template <typename... Types>
concept is_convertible = (std::is_convertible_v<const Types&, Types> && ...);

template <typename... Types>
concept is_copy_list_initializable = (requires {
  {
    Types {}
  }
  ->std::same_as<Types>;
} && ...);

template <typename... Types>
concept is_copy_constructible = (std::is_copy_constructible_v<Types> && ...);

template <typename... Types>
concept is_move_constructible = (std::is_move_constructible_v<Types> && ...);

template <typename... Types>
concept is_copy_assignable = (std::is_copy_assignable_v<Types> && ...);

template <typename... Types>
concept is_move_assignable = (std::is_move_assignable_v<Types> && ...);

template <typename Tuple, typename UHead, typename... UTail>
concept tuple_constructible_from_args =
    is_tuple_constructible_from_args<Tuple, UHead, UTail...>;

template <typename Tuple, typename UHead, typename... UTail>
concept tuple_implicitly_convertible_from_args =
    is_tuple_implicitly_convertible_from_args<Tuple, UHead, UTail...>;

template <typename TupleOne, typename TupleTwo>
concept tuple_convertible_from_tuple =
    is_tuple_convertible_from_tuple<TupleOne, TupleTwo>;

template <typename TupleOne, typename TupleTwo>
concept tuple_assigable_from_tuple_copy =
    is_tuple_assigable_from_tuple_copy<TupleOne, TupleTwo>;
template <typename TupleOne, typename TupleTwo>
concept tuple_assigable_from_tuple_move =
    is_tuple_assigable_from_tuple_move<TupleOne, TupleTwo>;

template <typename TupleOne, typename TupleTwo>
concept tuple_constructible_from_tuple_main =
    is_tuple_constructible_from_tuple<TupleOne, TupleTwo>;

template <typename TupleOne, typename TupleTwo>
concept tuple_trivial_cases_check = requires {
  requires(
      TupleOne::size() == 1 &&
      !(std::is_convertible_v<TupleTwo,
                              typename decltype(TupleOne::head_type())::type> ||
        std::is_constructible_v<typename decltype(TupleOne::head_type())::type,
                                TupleTwo> ||
        std::is_same_v<typename decltype(TupleOne::head_type())::type,
                       typename decltype(TupleTwo::head_type())::type>)) ||
      (TupleOne::size() != 1);
};

template <typename TupleOne, typename TupleTwo>
concept tuple_constructible_from_tuple = requires {
  requires !std::is_same_v<TupleOne, TupleTwo>;
  requires tuple_trivial_cases_check<TupleOne, TupleTwo>;
  requires tuple_constructible_from_tuple_main<TupleOne, TupleTwo>;
};

// GET HELPERS

template <size_t Index, typename Head, typename... Tail>
auto& get_helper(Tuple<Head, Tail...>& tuple) noexcept {
  if constexpr (Index == 0) {
    return tuple.head_;
  } else {
    return get_helper<Index - 1>(tuple.tail_);
  }
}

template <size_t Index, typename Head, typename... Tail>
const auto& get_helper(const Tuple<Head, Tail...>& tuple) noexcept {
  if constexpr (Index == 0) {
    return tuple.head_;
  } else {
    return get_helper<Index - 1>(tuple.tail_);
  }
}

template <std::size_t Index, class Head, class... Tail>
auto&& get_helper(Tuple<Head, Tail...>&& tuple) noexcept {
  if constexpr (Index == 0) {
    return std::move(tuple.head_);
  } else {
    return get_helper<Index - 1>(std::move(tuple).tail_);
  }
}

template <std::size_t Index, class Head, class... Tail>
const auto&& get_helper(const Tuple<Head, Tail...>&& tuple) noexcept {
  if constexpr (Index == 0) {
    return std::move(tuple.head_);
  } else {
    return get_helper<Index - 1>(std::move(tuple).tail_);
  }
}

template <size_t Index>
auto get_helper(Tuple<>&) noexcept {
  static_assert(Index != Index);
}

template <typename T, typename... Types>
constexpr std::size_t count_type = (0 + ... +
                                    (std::is_same_v<T, Types> ? 1 : 0));

template <typename T, typename Head, typename... Tail>
T& get_by_type(Tuple<Head, Tail...>& tuple) noexcept {
  if constexpr (std::is_same_v<T, Head>) {
    return tuple.head_;
  } else {
    return get_by_type<T>(tuple.tail_);
  }
}

template <typename T, typename Head, typename... Tail>
const T& get_by_type(const Tuple<Head, Tail...>& tuple) noexcept {
  if constexpr (std::is_same_v<T, Head>) {
    return tuple.head_;
  } else {
    return get_by_type<T>(tuple.tail_);
  }
}

template <typename T, typename Head, typename... Tail>
T&& get_by_type(Tuple<Head, Tail...>&& tuple) noexcept {
  if constexpr (std::is_same_v<T, Head>) {
    return std::move(tuple.head_);
  } else {
    return get_by_type<T>(std::move(tuple.tail_));
  }
}

template <typename T, typename Head, typename... Tail>
const T&& get_by_type(const Tuple<Head, Tail...>&& tuple) noexcept {
  if constexpr (std::is_same_v<T, Head>) {
    return std::move(tuple.head_);
  } else {
    return get_by_type<T>(std::move(tuple.tail_));
  }
}

// TUPLE_CAT HELPER

template <typename... Tuples>
struct TupleCater;

template <>
struct TupleCater<> {
  using Type = Tuple<>;
};

template <typename... Types>
struct TupleCater<Tuple<Types...>> {
  using Type = Tuple<Types...>;
};

template <typename... Tuples>
using TupleCatType = typename TupleCater<Tuples...>::Type;

template <typename... First, typename... Second, typename... Tuples>
struct TupleCater<Tuple<First...>, Tuple<Second...>, Tuples...> {
  using Type = TupleCatType<Tuple<First..., Second...>, Tuples...>;
};

template <typename TupleT>
struct TupleSize;
template <typename... Tuples>
struct TupleSize<Tuple<Tuples...>> {
  static constexpr size_t value = sizeof...(Tuples);
};

template <size_t I, typename First, typename... Rest>
decltype(auto) get_args(First&& first, Rest&&... rest) {
  if constexpr (I < TupleSize<std::unwrap_ref_decay_t<First>>::value) {
    return get<I>(std::forward<First>(first));
  } else {
    return get_args<I - TupleSize<std::unwrap_ref_decay_t<First>>::value>(
        std::forward<Rest>(rest)...);
  }
}

template <typename... Tuples>
struct TotalSize {
  static constexpr size_t value =
      (0 + ... + TupleSize<std::unwrap_ref_decay_t<Tuples>>::value);
};

template <size_t I, typename... Tuples>
using ElemTypes = decltype(get_args<I>(std::declval<Tuples>()...));

template <typename... Tuples, size_t... Is>
auto tupleCat_impl(std::index_sequence<Is...>, Tuples&&... tuples) {
  return TupleCatType<std::remove_cvref_t<Tuples>...>(
      std::forward<ElemTypes<Is, Tuples...>>(
          get_args<Is>(std::forward<Tuples>(tuples)...))...);
}
}  // namespace tuple_detail

// GET BY INDEX

template <size_t Index, typename... Types>
decltype(auto) get(Tuple<Types...>& tuple) noexcept {
  static_assert(Index < sizeof...(Types));
  return tuple_detail::get_helper<Index>(tuple);
}

template <size_t Index, typename... Types>
decltype(auto) get(const Tuple<Types...>& tuple) noexcept {
  static_assert(Index < sizeof...(Types));
  return tuple_detail::get_helper<Index>(tuple);
}

template <size_t Index, typename... Types>
decltype(auto) get(Tuple<Types...>&& tuple) noexcept {
  static_assert(Index < sizeof...(Types));
  return std::move(tuple_detail::get_helper<Index>(tuple));
}

template <size_t Index, typename... Types>
decltype(auto) get(const Tuple<Types...>&& tuple) noexcept {
  static_assert(Index < sizeof...(Types));
  return std::move(tuple_detail::get_helper<Index>(tuple));
}

// GET BY TYPE

template <typename T, typename... Types>
T& get(Tuple<Types...>& tuple) noexcept {
  static_assert(tuple_detail::count_type<T, Types...> == 1,
                "Only one T-type in tuple for this get");
  return tuple_detail::get_by_type<T>(tuple);
}

template <typename T, typename... Types>
const T& get(const Tuple<Types...>& tuple) noexcept {
  static_assert(tuple_detail::count_type<T, Types...> == 1,
                "Only one T-type in tuple for this get");
  return tuple_detail::get_by_type<T>(tuple);
}

template <typename T, typename... Types>
T&& get(Tuple<Types...>&& tuple) noexcept {
  static_assert(tuple_detail::count_type<T, Types...> == 1,
                "Only one T-type in tuple for this get");
  return tuple_detail::get_by_type<T>(std::move(tuple));
}

template <typename T, typename... Types>
const T&& get(const Tuple<Types...>&& tuple) noexcept {
  static_assert(tuple_detail::count_type<T, Types...> == 1,
                "Only one T-type in tuple for this get");
  return tuple_detail::get_by_type<T>(std::move(tuple));
}

// LEXICOGRAPHICAL COMPARISON OPERATORS

template <typename... TypesOne, typename... TypesTwo>
bool operator==(const Tuple<TypesOne...>& left,
                const Tuple<TypesTwo...>& right) {
  if constexpr (sizeof...(TypesOne) != sizeof...(TypesTwo)) {
    return false;
  } else if constexpr (sizeof...(TypesOne) == 0) {
    return true;
  } else {
    return left.head_ == right.head_ && left.tail_ == right.tail_;
  }
}

template <typename... TypesOne, typename... TypesTwo>
bool operator!=(const Tuple<TypesOne...>& left,
                const Tuple<TypesTwo...>& right) {
  return !(left == right);
}

template <typename... TypesOne, typename... TypesTwo>
bool operator<(const Tuple<TypesOne...>& left,
               const Tuple<TypesTwo...>& right) {
  if constexpr (sizeof...(TypesOne) == sizeof...(TypesTwo) &&
                sizeof...(TypesOne) == 0) {
    return false;
  } else if constexpr (sizeof...(TypesOne) == 0) {
    return true;
  } else if constexpr (sizeof...(TypesTwo) == 0) {
    return false;
  } else {
    if (left.head_ < right.head_) {
      return true;
    }
    if (right.head_ < left.head_) {
      return false;
    }
    return left.tail_ < right.tail_;
  }
}

template <typename... TypesOne, typename... TypesTwo>
bool operator<=(const Tuple<TypesOne...>& left,
                const Tuple<TypesTwo...>& right) {
  return !(right < left);
}

template <typename... TypesOne, typename... TypesTwo>
bool operator>(const Tuple<TypesOne...>& left,
               const Tuple<TypesTwo...>& right) {
  return right < left;
}

template <typename... TypesOne, typename... TypesTwo>
bool operator>=(const Tuple<TypesOne...>& left,
                const Tuple<TypesTwo...>& right) {
  return !(left < right);
}

// TUPLE ITSELF

template <typename Head, typename... Tail>
class Tuple<Head, Tail...> {
 private:
  // FRIENDS
  template <typename... UTypes>
  friend class Tuple;

  template <size_t Index, typename Header, typename... Tailer>
  friend auto& tuple_detail::get_helper(
      Tuple<Header, Tailer...>& tuple) noexcept;

  template <size_t Index, typename Header, typename... Tailer>
  friend const auto& tuple_detail::get_helper(
      const Tuple<Header, Tailer...>& tuple) noexcept;

  template <size_t Index, typename Header, typename... Tailer>
  friend auto&& tuple_detail::get_helper(
      Tuple<Header, Tailer...>&& tuple) noexcept;

  template <size_t Index, typename Header, typename... Tailer>
  friend const auto&& tuple_detail::get_helper(
      Tuple<Header, Tailer...>&& tuple) noexcept;

  template <typename T, typename Header, typename... Tailer>
  friend T& tuple_detail::get_by_type(Tuple<Header, Tailer...>& tuple) noexcept;

  template <typename T, typename Header, typename... Tailer>
  friend const T& tuple_detail::get_by_type(
      const Tuple<Header, Tailer...>& tuple) noexcept;

  template <typename T, typename Header, typename... Tailer>
  friend T&& tuple_detail::get_by_type(
      Tuple<Header, Tailer...>&& tuple) noexcept;

  template <typename T, typename Header, typename... Tailer>
  friend const T&& tuple_detail::get_by_type(
      const Tuple<Header, Tailer...>&& tuple) noexcept;

  template <typename... TypesOne, typename... TypesTwo>
  friend bool operator==(const Tuple<TypesOne...>& left,
                         const Tuple<TypesTwo...>& right);

  template <typename... TypesOne, typename... TypesTwo>
  friend bool operator<(const Tuple<TypesOne...>& left,
                        const Tuple<TypesTwo...>& right);

  // FIELDS

  Head head_;
  [[no_unique_address]] Tuple<Tail...> tail_;

 public:
  // CONSTRUCTORS

  explicit(!tuple_detail::is_copy_list_initializable<Head, Tail...>)
      Tuple() requires(tuple_detail::is_default_constructible<Head, Tail...>)
      : head_(), tail_(){};

  explicit(!tuple_detail::is_convertible<Head, Tail...>)
      Tuple(const Head& head, const Tail&... tail) requires(
          tuple_detail::is_copy_constructible<Head, Tail...>)
      : head_(head), tail_(tail...) {}

  Tuple(const Tuple&
            another) requires tuple_detail::is_copy_constructible<Head, Tail...>
      : head_(another.head_), tail_(another.tail_) {}

  Tuple(Tuple&&
            another) requires tuple_detail::is_move_constructible<Head, Tail...>
      : head_(std::forward<Head>(another.head_)),
        tail_(std::forward<Tuple<Tail...>>(another.tail_)) {}

  template <typename UHead, typename... UTail>
  requires((tuple_detail::tuple_constructible_from_tuple<
            Tuple,
            Tuple<UHead, UTail...>>)) explicit(!tuple_detail::
                                                   tuple_convertible_from_tuple<
                                                       Tuple,
                                                       Tuple<UHead, UTail...>>)
      Tuple(const Tuple<UHead, UTail...>& another)
      : head_(another.head_), tail_(another.tail_) {}

  template <typename UHead, typename... UTail>
  requires((tuple_detail::tuple_constructible_from_tuple<
            Tuple,
            Tuple<UHead, UTail...>>)) explicit(!tuple_detail::
                                                   tuple_convertible_from_tuple<
                                                       Tuple,
                                                       Tuple<UHead, UTail...>>)
      Tuple(Tuple<UHead, UTail...>&& another)
      : head_(std::forward<UHead>(another.head_)),
        tail_(std::forward<decltype(another.tail_)>(another.tail_)) {}

  template <typename UHead, typename... UTail>
  requires(tuple_detail::tuple_constructible_from_args<
           Tuple, UHead,
           UTail...>) explicit(!tuple_detail::
                                   tuple_implicitly_convertible_from_args<
                                       Tuple, UHead, UTail...>)
      Tuple(UHead&& arg, UTail&&... args)
      : head_(std::forward<UHead>(arg)), tail_(std::forward<UTail>(args)...) {}

  template <typename T_One, typename T_Two>
  Tuple(const std::pair<T_One, T_Two>& pair)
      : head_(pair.first), tail_(pair.second) {}

  template <typename T_One, typename T_Two>
  Tuple(std::pair<T_One, T_Two>&& pair)
      : head_(std::move(pair.first)), tail_(std::move(pair.second)) {}

  // = OPERATOR

  Tuple& operator=(
      const Tuple&
          another) requires tuple_detail::is_copy_assignable<Head, Tail...> {
    if (this == &another) {
      return *this;
    }
    head_ = another.head_;
    tail_ = another.tail_;
    return *this;
  }

  Tuple& operator=(Tuple&& another) requires tuple_detail::
      is_move_assignable<Head, Tail...> {
    if (this == &another) {
      return *this;
    }
    head_ = std::move(another.head_);
    tail_ = std::move(another.tail_);
    return *this;
  }

  template <class UHead, class... UTail>
  requires(tuple_detail::tuple_assigable_from_tuple_copy<
           Tuple, Tuple<UHead, UTail...>>) Tuple&
  operator=(const Tuple<UHead, UTail...>& another) {
    head_ = another.head_;
    if constexpr (sizeof...(UTail) > 0) {
      tail_ = another.tail_;
    }
    return *this;
  }
  template <class UHead, class... UTail>
  requires(tuple_detail::tuple_assigable_from_tuple_move<
           Tuple, Tuple<UHead, UTail...>>) Tuple&
  operator=(Tuple<UHead, UTail...>&& another) noexcept {
    head_ = std::forward<UHead>(another.head_);
    if constexpr (sizeof...(UTail) > 0) {
      tail_ = std::forward<Tuple<UTail...>>(another.tail_);
    }
    return *this;
  }

  // SIZE AND TYPE TIPS

  constexpr static size_t size() { return Tuple<Tail...>::size() + 1; }
  static constexpr auto head_type() -> std::type_identity<Head> { return {}; }
  static constexpr auto tail_type() -> std::type_identity<Tuple<Tail...>> {
    return {};
  }
};

// TEMPLATE DEDUCTION

template <typename Head, typename... Tail>
Tuple(Head, Tail...)->Tuple<Head, Tail...>;
template <class... Types>
Tuple(Tuple<Types...>)->Tuple<Types...>;
template <class One, class Two>
Tuple(std::pair<One, Two>)->Tuple<One, Two>;

// MAKE_TUPLE, TIE AND FORWARD_AS_TUPLE

template <typename... Args>
auto makeTuple(Args&&... args) {
  return Tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...);
}

template <typename... Args>
auto tie(Args&... args) {
  return Tuple<Args&...>(args...);
}

template <typename... Args>
auto forwardAsTuple(Args&&... args) {
  return Tuple<Args&&...>(std::forward<Args>(args)...);
}

// TUPLE_CAT

template <typename... Tuples>
auto tupleCat(Tuples&&... tuples) {
  constexpr size_t N = tuple_detail::TotalSize<Tuples...>::value;
  return tuple_detail::tupleCat_impl<Tuples...>(
      std::make_index_sequence<N>{}, std::forward<Tuples>(tuples)...);
}
