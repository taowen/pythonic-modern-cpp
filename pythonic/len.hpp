#pragma once

#include "utf8/Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {

template <typename Rng, CONCEPT_REQUIRES_(utf8::Utf8EncodedRange<Rng>())>
size_t len(Rng&& rng) {
    auto length = size_t(0);
    auto n = rng.utf8_encoded.size();
    for (auto cur = rng.utf8_encoded.begin(); n > 0; n--, cur++) {
        if ((*cur & 0xc0) != 0x80) {
            length++;
        }
    }
    return length;
}

template <typename Rng, CONCEPT_REQUIRES_(ranges::Range<Rng>())>
size_t len(Rng&& rng) {
    return ranges::size(rng);
}
}
