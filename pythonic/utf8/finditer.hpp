#pragma once
#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng1, typename Rng2,
          CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng1>()),
          CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng2>())>
class FindView : public ranges::view_facade<FindView<Rng1, Rng2>> {

  friend ranges::range_access;

  decltype(Rng1().begin()) __haystack_begin;
  decltype(Rng1().begin()) __haystack_cur;
  decltype(Rng1().end()) __haystack_end;
  decltype(Rng2().begin()) __needle;
  size_t __found_at;
  size_t __nsize;
  size_t __skip;
  char __last_needle;

  struct cursor {
  private:
    FindView *__rng;

  public:
    cursor() = default;
    explicit cursor(FindView &rng) : __rng(&rng) {}
    void next() { __rng->next(); }
    size_t get() const { return __rng->__found_at; }
    bool done() const { return __rng->__found_at == std::string::npos; }
  };

  cursor begin_cursor() { return cursor{*this}; }

  void next() {
    if (__found_at == std::string::npos) {
      return;
    }
    // Boyer-Moore skip value for the last char in the needle. Zero is
    // not a valid value; skip will be computed the first time it's
    // needed.

    auto nsize_1 = __nsize - 1;
    auto i = __haystack_cur;
    while (i < __haystack_end) {
      // Boyer-Moore: match the last element in the needle
      while (i[nsize_1] != __last_needle) {
        if (++i == __haystack_end) {
          // not found
          __found_at = std::string::npos;
          return;
        }
      }
      // Here we know that the last char matches
      // Continue in pedestrian mode
      for (size_t j = 0;;) {
        if (i[j] != __needle[j]) {
          // Not found, we can skip
          i += get_skip();
          break;
        }
        // Check if done searching
        if (++j == __nsize) {
          // Yay
          __found_at = size_t(i - __haystack_begin);
          __haystack_cur = i + __nsize;
          return;
        }
      }
    }
    __found_at = std::string::npos;
    return;
  }

  size_t get_skip() {
    // Compute the skip value lazily
    if (__skip == 0) {
      auto nsize_1 = __nsize - 1;
      __skip = 1;
      while (__skip <= nsize_1 && __needle[nsize_1 - __skip] != __last_needle) {
        ++__skip;
      }
    }
    return __skip;
  }

public:
  FindView(Rng1 haystack, Rng2 needle)
      : __haystack_begin(haystack.begin()), __haystack_cur(haystack.begin()),
        __haystack_end(haystack.end()), __needle(needle.begin()), __skip(0),
        __found_at(0), __last_needle(ranges::back(needle)),
        __nsize(ranges::size(needle)) {
    auto const size = ranges::size(haystack);
    if (__nsize > size) {
      __found_at = std::string::npos;
      return;
    }
    if (__nsize == 0) {
      __found_at = std::string::npos;
      return;
    }
    // Don't use std::search, use a Boyer-Moore-like trick by comparing
    // the last characters first
    next();
  }
};

struct finditer_fn {
public:
  friend view::view_access;

  template <typename Rng, typename End = decltype(ranges::end),
            CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng>())>
  static auto bind(finditer_fn finditer, Rng &&sub, size_t start = 0,
                   End end = ranges::end) {
    return ranges::make_pipeable(std::bind(finditer, std::placeholders::_1,
                                           std::forward<Rng>(sub), start, end));
  }

public:
  template <typename Rng1, typename Rng2, typename End = decltype(ranges::end),
            CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng1>()),
            CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng2>())>
  auto operator()(Rng1 &&raw_haystack, Rng2 &&utf8_needle, size_t start = 0,
                  End end = ranges::end) const {
    auto needle = utf8_needle.utf8_encoded;
    auto haystack = raw_haystack | view::slice(start, end);
    return FindView<decltype(haystack), decltype(needle)>(haystack, needle);
  }
};
auto finditer = utf8::Utf8View<finditer_fn>();
}
}
