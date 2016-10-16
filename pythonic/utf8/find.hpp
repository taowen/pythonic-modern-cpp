#pragma once
#include "Utf8Encoded.hpp"
#include "finditer.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;


template <typename Rng1, typename Rng2, typename End = decltype(ranges::end),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng1>()),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng2>())>
auto find(Rng1 &&haystack, Rng2 &&needle, size_t start = 0, End end = ranges::end) {
    return ranges::front(finditer(haystack, needle, start, end));
}

}
}
