#ifndef ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_ISTREAM_
#define ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_ISTREAM_
////////////////////////////////////////////////////////////////////////////////
#include <istream>
////////////////////////////////////////////////////////////////////////////////

namespace arithmetic {

/// \brief istream operator
template <class C, class CT, class T, class B>
inline auto operator>>(std::basic_istream<C, CT>& i, Arithmetic<T, B>& v)
    -> decltype(i >> v()) {
  return i >> v();
}

}  // namespace arithmetic

////////////////////////////////////////////////////////////////////////////////
#endif  // ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_ISTREAM_
