#pragma once

#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename T> class Utf8Encoded {
  using my_type = Utf8Encoded<T>;

public:
  using traits_type = typename T::traits_type;
  using value_type = typename T::value_type;
  using pointer = typename T::pointer;
  using const_pointer = typename T::const_pointer;
  using reference = typename T::reference;
  using const_reference = typename T::const_reference;
  using const_iterator = typename T::const_iterator;
  using iterator = typename T::iterator;
  using reverse_iterator = typename T::reverse_iterator;
  using const_reverse_iterator = typename T::const_reverse_iterator;
  using size_type = typename T::size_type;
  using difference_type = typename T::difference_type;

  T data;
  Utf8Encoded(T data) : data(data) {}
  Utf8Encoded() {}
  Utf8Encoded(my_type const &that) : data(that.data) {}
  Utf8Encoded(my_type &&that) : data(std::move(that.data)) {}
  Utf8Encoded(const_iterator begin, const_iterator end) : data(T(begin, end)) {}
  my_type &operator=(my_type const &that) {
    data = that.data;
    return *this;
  }
  my_type &operator=(my_type &&that) {
    data = std::move(that.data);
    return *this;
  }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }
  auto size() const { return data.size(); }
  void reserve(size_type new_cap) { data.reserve(new_cap); }
  template <typename I1, typename I2> auto assign(I1 begin, I2 end) {
    data.assign(begin, end);
  }
};

using Text = Utf8Encoded<folly::fbstring>;
using TextView = Utf8Encoded<std::string_view>;

bool operator==(std::string_view left, folly::fbstring const &right) {
  return left == std::string_view(right.c_str(), right.size());
}

template <typename T1, typename T2>
bool operator==(Utf8Encoded<T1> const &left, Utf8Encoded<T2> const &right) {
  return left.data == right.data;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, Utf8Encoded<T> const &str) {
  os << str.data;
  return os;
}

TextView utf8_cast(char const *data) { return TextView{data}; }

template <typename T> TextView utf8_cast(T data) { return TextView{data}; }

#define U8(X) pythonic::utf8::utf8_cast(u8##X)

struct to_text_fn {
public:
  template <typename Rng> Text operator()(Rng rng) const {
    Text text;
    text.reserve(static_cast<Text::size_type>(ranges::size(rng)));
    using I = ranges::range_common_iterator_t<Rng>;
    text.assign(I{ranges::begin(rng)}, I{ranges::end(rng)});
    return text;
  }
};
auto to_text = view::view<to_text_fn>();
}
}
