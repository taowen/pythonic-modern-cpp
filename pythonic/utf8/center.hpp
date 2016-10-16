#pragma once
#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng1, typename Rng2 = Utf8Encoded<std::string_view>, typename IsLazily=int,
          CONCEPT_REQUIRES_(utf8::Utf8EncodedSizedRange<Rng1>()),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedSizedRange<Rng2>())>
auto center(Rng1 &&rng, size_t width,
           Rng2 &&padding = utf8::utf8_cast(std::string_view(" ")), IsLazily* is_lazily=nullptr) {
    auto rng_size = ranges::size(rng.utf8_encoded);
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
    auto result = view::concat(left, rng.utf8_encoded, right, tail);
    return utf8::return_lazily(std::move(result), is_lazily);
}

}
}
