#pragma once

#include "Utf8Encoded.hpp"

#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng, typename Result=folly::fbstring, CONCEPT_REQUIRES_(utf8::Utf8EncodedRange<Rng>())>
auto capitalize(Rng &&rng) {
    auto begin = rng.utf8_encoded.begin();
    auto size = rng.utf8_encoded.size();
    auto head_capitalized = std::toupper(*begin);
    auto result = Result();
    result.reserve(size);
    result.push_back(head_capitalized);
    result.append(begin+1, size-1);
    return utf8_cast(result);
}

}
}
