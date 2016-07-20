## Arithmetic types

### Provides

`Arithmetic<T>` where `T` models the `Arithmetic` concept is a `T` wrapper that:
- disables _any_ implicit conversions between `T` and other types, and 
- can be made opaque by passing it a unique tag as second template parameter
`Arithmetic<T, unique_tag>`.

### Why is this useful?

It let's you easily specify strongly typed interfaces when Arithmetic types are
involved!

### Example 1: disabling implicit conversions

```c++
int a{2};
long b{3};
b = a; // works: the implicit conversion is safe

Arithmetic<int> a{2};
Arithmetic<long> b{3};
b = a; // fails: implicit assignment requires implicit conversion
b = Arithmetic<long>{a};               // works: explicit construction
b = static_cast<Arithmetic<long>>(a);  // works: explicit conversion
```

### Example 2: opaque type-defs

```c++
struct Tag1 {};
struct Tag2 {};

using Type1 = Arithmetic<int, Tag1>;
using Type2 = Arithmetic<int, Tag2>;
Type1 a{2};
Type2 b{3};
a = b;  // fails: Type1 != Type2 even tho both wrap an int
b = Type2{a};               // works: explicit construction
b = static_cast<Type2>(a);  // works: explicit conversion
```

See the [tests](https://github.com/gnzlbg/arithmetic_type/blob/master/test/all_test.cpp)
for more examples.

### Other facilities
- `to_string` function is provided in
`arithmetic_type/arithmetic_to_string.hpp`
- i/o stream operators are provided in
`arithmetic_type/arithmetic_istream.hpp` and
`arithmetic_type/arithmetic_ostream.hpp`
- when writing generic code one sometimes need to deal with both `Arithmetic<T>`
  and `T`. The function `primitive_cast(T t)` does the right thing.

### Dependencies:
- C++11/14 compiler (currently tested with clang 3.5 only)

### License
- Boost Software License Version 1.0

### Comparison with BOOST_STRONG_TYPEDEF

`Arithmetic<T>` supports C++1y `constexpr`, move semantics, and disables
implicit conversions.
