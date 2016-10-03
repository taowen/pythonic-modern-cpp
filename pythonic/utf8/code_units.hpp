#pragma once

#include "Utf8Encoded.hpp"

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct code_units_fn {
public:
  template <typename T>
  auto operator()(utf8::Utf8Encoded<T> &&input) const noexcept {
    return input.utf8_encoded;
  }
};

auto code_units = code_units_fn();

template <typename T> auto operator|(T &&left, code_units_fn code_units) {
  return code_units(std::forward<T>(left));
}
}
}
