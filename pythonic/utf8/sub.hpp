

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
auto sub(Rng1 &&haystack_, Rng2 &&needle_, Repl repl) {
    auto haystack = haystack_.utf8_encoded;
    auto needle = needle_.utf8_encoded;
    auto needle_len = ranges::size(needle);
    auto result = Result();
    result.reserve(ranges::size(haystack));
    auto last_pos = 0;
    for (auto pos : finditer(haystack_, needle_)) {
        result.append(haystack.begin() + last_pos, pos - last_pos);
        auto replaced = repl(pos).utf8_encoded;
        result.append(replaced.begin(), replaced.size());
        last_pos = pos + needle_len;
    }
    result.append(haystack.begin() + last_pos, haystack.size() - last_pos);
    return utf8::utf8_cast(std::move(result));
}

}
}
