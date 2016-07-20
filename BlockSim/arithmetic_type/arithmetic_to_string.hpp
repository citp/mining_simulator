#ifndef ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_TO_STRING_
#define ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_TO_STRING_
////////////////////////////////////////////////////////////////////////////////
#include <string>
////////////////////////////////////////////////////////////////////////////////

namespace arithmetic {

/// \brief to_string
template <class T, class U>
inline std::string to_string(const Arithmetic<T, U> a) {
  return std::to_string(a());
}

}  // namespace arithmetic

////////////////////////////////////////////////////////////////////////////////
#endif  // ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_TO_STRING_
