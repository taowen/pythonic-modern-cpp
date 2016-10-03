#pragma once
#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct find_fn {
public:
  friend view::view_access;

  template <typename Rng, typename End = decltype(ranges::end),
            CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng>())>
  static auto bind(find_fn find, Rng sub, size_t start = 0,
                   End end = ranges::end) {
    return ranges::make_pipeable(
        std::bind(find, std::placeholders::_1, sub, start, end));
  }

public:
  template <typename Rng1, typename Rng2, typename End = decltype(ranges::end),
            CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng1>()),
            CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng2>())>
  auto operator()(Rng1 raw_haystack, Rng2 utf8_needle, size_t start = 0,
                  End end = ranges::end) const {
    auto needle = utf8_needle.utf8_encoded;
    auto haystack = raw_haystack | view::slice(start, end);
    auto const size = ranges::size(haystack);
    auto nsize = ranges::size(needle);
    if (nsize > size) {
      return std::string::npos;
    }
    if (nsize == 0) {
      return std::string::npos;
    }
    // Don't use std::search, use a Boyer-Moore-like trick by comparing
    // the last characters first
    auto const nsize_1 = nsize - 1;
    auto const last_needle = ranges::back(needle);

    // Boyer-Moore skip value for the last char in the needle. Zero is
    // not a valid value; skip will be computed the first time it's
    // needed.
    size_t skip = 0;

    auto i = ranges::begin(haystack);
    auto i_end = ranges::end(haystack);
    auto needle_iter = ranges::begin(needle);

    while (i < i_end) {
      // Boyer-Moore: match the last element in the needle
      while (i[nsize_1] != last_needle) {
        if (++i == i_end) {
          // not found
          return std::string::npos;
        }
      }
      // Here we know that the last char matches
      // Continue in pedestrian mode
      for (size_t j = 0;;) {
        if (i[j] != needle_iter[j]) {
          // Not found, we can skip
          // Compute the skip value lazily
          if (skip == 0) {
            skip = 1;
            while (skip <= nsize_1 &&
                   needle_iter[nsize_1 - skip] != last_needle) {
              ++skip;
            }
          }
          i += skip;
          break;
        }
        // Check if done searching
        if (++j == nsize) {
          // Yay
          return size_t(i - ranges::begin(haystack));
        }
      }
    }
    return std::string::npos;
  }
};
auto find = utf8::Utf8View<find_fn>();
}
}
