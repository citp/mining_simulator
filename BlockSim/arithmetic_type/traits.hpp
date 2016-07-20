#ifndef ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_TRAITS_
#define ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_TRAITS_
////////////////////////////////////////////////////////////////////////////////
#include "arithmetic_type.hpp"
////////////////////////////////////////////////////////////////////////////////

namespace arithmetic {

template <class T> struct is_arithmetic_ {
  static const bool value = false;
};
template <class T, class U> struct is_arithmetic_<Arithmetic<T, U>> {
  static const bool value = true;
};

template <class T> struct is_arithmetic {
  static const bool value
      = is_arithmetic_<std::remove_reference_t<std::remove_cv_t<T>>>::value;
};

}  // namespace arithmetic

////////////////////////////////////////////////////////////////////////////////
#endif  // ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_TRAITS_
