#pragma once
#include "Utf8Encoded.hpp"
#include "finditer.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct find_fn {
public:
  friend view::view_access;

  template <typename Rng, typename End = decltype(ranges::end),
            CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng>())>
  static auto bind(find_fn find, Rng &&sub, size_t start = 0,
                   End end = ranges::end) {
    return ranges::make_pipeable(std::bind(find, std::placeholders::_1,
                                           std::forward<Rng>(sub), start, end));
  }

public:
  template <typename Rng1, typename Rng2, typename End = decltype(ranges::end),
            CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng1>()),
            CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng2>())>
  auto operator()(Rng1 &&raw_haystack, Rng2 &&utf8_needle, size_t start = 0,
                  End end = ranges::end) const {
    return ranges::front(
        finditer(Utf8Encoded<Rng1>{raw_haystack}, utf8_needle, start, end));
  }
};
auto find = utf8::Utf8View<find_fn>();
}
}
