

#pragma once

#include "finditer.hpp"
#include "Utf8Encoded.hpp"
#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng1, typename Rng2, typename Repl, typename Result=folly::fbstring,
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng1>()),
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng2>())>
auto sub(Rng1 &&haystack, Rng2 &&needle, Repl repl) {
    auto needle_len = ranges::size(needle.utf8_encoded);
    auto result = Result();
    result.reserve(ranges::size(haystack.utf8_encoded));
    auto last_pos = 0;
    for (auto pos : finditer(haystack, needle)) {
        result.append(haystack.utf8_encoded.begin() + last_pos, pos - last_pos);
        auto replaced = repl(pos);
        result.append(replaced.utf8_encoded.begin(), replaced.utf8_encoded.size());
        last_pos = pos + needle_len;
    }
    result.append(haystack.utf8_encoded.begin() + last_pos, haystack.utf8_encoded.size() - last_pos);
    return utf8::utf8_cast(std::move(result));
}

}
}
