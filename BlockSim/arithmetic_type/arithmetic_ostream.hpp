#ifndef ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_OSTREAM_
#define ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_OSTREAM_
////////////////////////////////////////////////////////////////////////////////
#include <ostream>
////////////////////////////////////////////////////////////////////////////////

namespace arithmetic {

/// \brief ostream operator
template <class C, class CT, class T, class B>
inline auto operator<<(std::basic_ostream<C, CT>& o, const Arithmetic<T, B>& v)
    -> decltype(o << v()) {
  return o << v();
}

}  // namespace arithmetic

////////////////////////////////////////////////////////////////////////////////
#endif  // ARITHMETIC_UTILITIES_ARITHMETIC_TYPE_ARITHMETIC_OSTREAM_
