#pragma once

#include "utf8/Utf8Encoded.hpp"

namespace pythonic {

namespace view = ranges::view;

struct len_fn {
public:
  template <typename T>
  size_t operator()(utf8::Utf8Encoded<T> input) const noexcept {
    auto length = size_t(0);
    auto n = input.utf8_encoded.size();
    for (auto cur = input.utf8_encoded.begin(); n > 0; n--, cur++) {
      if ((*cur & 0xc0) != 0x80) {
        length++;
      }
    }
    return length;
  }
  template <typename T> size_t operator()(T input) const noexcept {
    return input.size();
  }
  template <typename T>
  size_t operator()(std::initializer_list<T> input) const noexcept {
    return input.size();
  }
};

auto len = len_fn();

template <typename T> auto operator|(T const &left, len_fn len) {
  return len(left);
}
}
