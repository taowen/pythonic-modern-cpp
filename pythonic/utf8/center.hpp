#pragma once
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct center_fn {
private:
  friend view::view_access;

  template <typename Rng = std::string_view>
  static auto bind(center_fn center, size_t width,
                   Rng padding = std::string_view(" ")) {
    return ranges::make_pipeable(
        std::bind(center, std::placeholders::_1, width, padding));
  }

public:
  template <typename Rng1, typename Rng2>
  auto operator()(Rng1 rng, size_t width, Rng2 padding) const {
    auto rng_size = ranges::size(rng);
    auto padding_size = ranges::size(padding);
    auto gap = width - rng_size;
    auto left_padding_count = gap / 2 / padding_size;
    auto right_pading_count =
        (gap - left_padding_count * padding_size) / padding_size;
    auto left = view::repeat(padding) | view::join |
                view::take(left_padding_count * padding_size);
    auto right = view::repeat(padding) | view::join |
                 view::take(right_pading_count * padding_size);
    auto tail_count = gap - left_padding_count * padding_size -
                      right_pading_count * padding_size;
    auto tail = view::repeat(' ') | view::take(tail_count);
    return view::concat(left, rng, right, tail);
  }
};
auto center = view::view<center_fn>();
}
}
