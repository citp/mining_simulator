#ifndef ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_PRIMITIVE_CAST_
#define ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_PRIMITIVE_CAST_
////////////////////////////////////////////////////////////////////////////////
#include "traits.hpp"
#include "enable_if.hpp"
////////////////////////////////////////////////////////////////////////////////

namespace arithmetic {

template <class T, detail_::EnableIf<is_arithmetic<T>> = detail_::dummy>
constexpr inline auto&& primitive_cast(T&& t) {
  static_assert(is_arithmetic<T>::value, "T must be an integer!");
  return t();
}

template <class T, detail_::EnableIf<is_arithmetic<T>> = detail_::dummy>
constexpr inline auto primitive_cast(const T& t) -> decltype(t()) {
  static_assert(is_arithmetic<T>::value, "T must be an integer!");
  return t();
}

template <class T, detail_::EnableIf<is_arithmetic<T>> = detail_::dummy>
constexpr inline auto& primitive_cast(T& t) {
  static_assert(is_arithmetic<T>::value, "T must be an integer!");
  return t();
}

template <class T, detail_::DisableIf<is_arithmetic<T>> = detail_::dummy>
constexpr inline auto&& primitive_cast(T&& t) {
  static_assert(!is_arithmetic<T>::value, "T can't be an integer!");
  return std::forward<T>(t);
}

template <class T, detail_::DisableIf<is_arithmetic<T>> = detail_::dummy>
constexpr inline auto primitive_cast(const T& t) -> decltype(t) {
  static_assert(!is_arithmetic<T>::value, "T can't be an integer!");
  return t;
}

template <class T, detail_::DisableIf<is_arithmetic<T>> = detail_::dummy>
constexpr inline auto& primitive_cast(T& t) {
  static_assert(!is_arithmetic<T>::value, "T can't be an integer!");
  return t;
}

}  // namespace arithmetic

////////////////////////////////////////////////////////////////////////////////
#endif  // ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_PRIMITIVE_CAST_
