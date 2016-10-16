
#pragma once

#include "finditer.hpp"
#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct endswith_fn {
private:
    friend view::view_access;

    template <typename Rng, typename End = decltype(ranges::end),
              CONCEPT_REQUIRES_(utf8::Utf8EncodedRange<Rng>())>
    static auto bind(count_fn count, Rng &&utf8_needle, size_t start = 0,
                     End end = ranges::end) {
        return ranges::make_pipeable(std::bind(count, std::placeholders::_1, utf8_needle, start, end));
    }

public:
    template <typename Rng, typename Rng2, typename End = decltype(ranges::end),
              CONCEPT_REQUIRES_(ranges::Range<Rng>()),
              CONCEPT_REQUIRES_(utf8::Utf8EncodedSizedRange<Rng2>())>
    auto operator()(Rng &&raw_haystack, Rng2 &&utf8_needle, size_t start = 0,
                    End end = ranges::end) const {
        auto needle_len = ranges::size(utf8_needle.utf8_encoded);
        auto haystack = raw_haystack | view::slice(start, end) | view::slice(ranges::end - needle_len, ranges::end);
        return ranges::equal(haystack, utf8_needle.utf8_encoded);
    }
};
auto endswith = utf8::Utf8View<endswith_fn>();
}
}
