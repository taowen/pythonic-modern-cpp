#pragma once
#include "Utf8Encoded.hpp"
#include <folly/FBString.h>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

using namespace literals;

template <typename Rng1, typename Rng2 = TextView, typename Result=folly::fbstring,
          CONCEPT_REQUIRES_(utf8::Utf8EncodedSizedRange<Rng1>()),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedSizedRange<Rng2>())>
auto center(Rng1 &&rng, size_t width, Rng2 &&padding = " "_u, char tail = ' ') {
    auto rng_size = rng.utf8_encoded.size();
    auto padding_size = padding.utf8_encoded.size();
    auto gap = width - rng_size;
    auto left_padding_count = gap / 2 / padding_size;
    auto right_padding_count = (gap - left_padding_count * padding_size) / padding_size;
    auto tail_count = gap - left_padding_count * padding_size - right_padding_count * padding_size;
    auto result = Result();
    result.reserve(width);
    for (int i = 0; i < left_padding_count; i++) {
        result.append(padding.utf8_encoded.begin(), padding_size);
    }
    result.append(rng.utf8_encoded.begin(), rng_size);
    for (int i = 0; i < right_padding_count; i++) {
        result.append(padding.utf8_encoded.begin(), padding_size);
    }
    for (int i = 0; i < tail_count; i++) {
        result.push_back(tail);
    }
    return utf8_cast(result);
}

}
}
