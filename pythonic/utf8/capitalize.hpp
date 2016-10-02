#pragma once
#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct capitalize_fn {
public:
  template <typename Rng> auto operator()(Rng rng) const {
    auto head =
        rng | view::slice(0, 1) | view::transform([](auto c) -> decltype(c) {
          if ('a' <= c <= 'Z') {
            return std::toupper(c);
          } else {
            return c;
          }
        });
    auto tail = rng | view::slice(1, ranges::end);
    return view::concat(head, tail);
  }
};
auto capitalize = view::view<capitalize_fn>();
}
}
