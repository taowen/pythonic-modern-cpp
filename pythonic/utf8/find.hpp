#pragma once
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

struct find_fn {
private:
  friend view::view_access;

  template <typename Rng>
  static auto bind(find_fn find, Rng sub, size_t start = 0, size_t end = -1) {
    return ranges::make_pipeable(
        std::bind(find, std::placeholders::_1, sub, start, end));
  }

public:
  template <typename Rng1, typename Rng2,
            CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng1>()),
            CONCEPT_REQUIRES_(ranges::RandomAccessRange<Rng2>())>
  auto operator()(Rng1 rng, Rng2 sub, size_t start = 0, size_t end = -1) const {
    rng |= view::slice(start, end);
    return 0;
  }
};
auto find = view::view<find_fn>();
}
}
