#pragma once

#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng, CONCEPT_REQUIRES_(utf8::CharRange<Rng>())>
class CodePointsView : public ranges::view_facade<CodePointsView<Rng>> {

  friend ranges::range_access;
  using iter_type = ranges::range_iterator_t<Rng>;

  iter_type __cur;
  ranges::range_sentinel_t<Rng> __end;
  utf8::TextView __code_point;

  struct cursor {
  private:
    CodePointsView *__rng;

  public:
    cursor() = default;
    explicit cursor(CodePointsView &rng) : __rng(&rng) {}
    void next() { __rng->next(); }
    auto get() const { return __rng->__code_point; }
    bool done() const {
      return ranges::size(__rng->__code_point.utf8_encoded) == 0;
    }
  };

  cursor begin_cursor() { return cursor{*this}; }

  void next() {
    if (__cur == __end) {
      __code_point = utf8::utf8_cast(std::string_view{});
      return;
    }
    auto code = static_cast<unsigned char>(*__cur);
    auto step = 4;
    if (code <= 0xc1) {
      step = 1;
    } else if (code <= 0xdf) {
      step = 2;
    } else if (code <= 0xef) {
      step = 3;
    }
    auto code_point_end = ranges::next(__cur, step, __end);
    __code_point =
        utf8::utf8_cast(std::string_view(__cur, code_point_end - __cur));
    __cur = code_point_end;
  }

public:
  CodePointsView(Rng const &rng)
      : __cur(ranges::begin(rng)), __end(ranges::end(rng)) {
    next();
  }
};

struct code_points_fn {
  template <typename Rng, CONCEPT_REQUIRES_(ranges::Range<Rng>())>
  auto operator()(Rng &&rng) const {
    return CodePointsView<Rng>(rng);
  }
};
auto code_points = utf8::Utf8View<code_points_fn>();
}
}
