#pragma once

#include "finditer.hpp"
#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng, typename Rng2, typename End = decltype(ranges::end),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRange<Rng>()),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRange<Rng2>())>
auto count(Rng &&raw_haystack, Rng2 &&utf8_needle, size_t start = 0,
           End end = ranges::end) {
    auto haystack = raw_haystack.utf8_encoded | view::slice(start, end);
    return ranges::distance(finditer(utf8::utf8_cast(haystack), utf8_needle));
}

}
}
