// (C) Copyright Martinho Fernandes (see below)
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt).

/// \brief EnableIf / DisableIf as proposed by Martinho Fernandes in
/// http://flamingdangerzone.com/cxx11/2012/06/01/almost-static-if.html

#if !defined(ARITHMETIC_TYPE_DETAIL_ENABLE_IF_HPP_)
#define ARITHMETIC_TYPE_DETAIL_ENABLE_IF_HPP_

#include <type_traits>

namespace detail_ {
enum class enabler {};
constexpr enabler dummy = {};

template <class C, class T = enabler>
using EnableIf = std::enable_if_t<C::value, T>;

template <class C, class T = enabler>
using DisableIf = std::enable_if_t<!(C::value), T>;

}  // namespace detail_

#endif  // ARITHMETIC_TYPE_ENABLE_IF_HPP_
