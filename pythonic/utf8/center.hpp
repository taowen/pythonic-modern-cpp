#pragma once
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct center_fn {
private:
  friend view::view_access;

  template <typename Rng = Utf8Encoded<std::string_view>>
  static auto bind(center_fn center, size_t width,
                   Rng &&padding = utf8::utf8_cast(std::string_view(" "))) {
    return ranges::make_pipeable(std::bind(center, std::placeholders::_1, width,
                                           std::forward<Rng>(padding)));
  }

public:
  template <typename Rng1, typename Rng2 = Utf8Encoded<std::string_view>,
            CONCEPT_REQUIRES_(ranges::SizedRange<Rng1>())>
  auto
  operator()(Rng1 &&rng, size_t width,
             Rng2 &&padding = utf8::utf8_cast(std::string_view(" "))) const {
    auto rng_size = ranges::size(rng);
    auto padding_size = ranges::size(padding.utf8_encoded);
    auto gap = width - rng_size;
    auto left_padding_count = gap / 2 / padding_size;
    auto right_pading_count =
        (gap - left_padding_count * padding_size) / padding_size;
    auto left = view::repeat(padding.utf8_encoded) | view::join |
                view::take(left_padding_count * padding_size);
    auto right = view::repeat(padding.utf8_encoded) | view::join |
                 view::take(right_pading_count * padding_size);
    auto tail_count = gap - left_padding_count * padding_size -
                      right_pading_count * padding_size;
    auto tail = view::repeat(' ') | view::take(tail_count);
    return view::concat(left, rng, right, tail);
  }
};
auto center = utf8::Utf8View<center_fn>();
}
}
