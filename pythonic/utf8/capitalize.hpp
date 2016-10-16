#pragma once

#include "Utf8Encoded.hpp"
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng, typename IsLazily=int, CONCEPT_REQUIRES_(utf8::Utf8EncodedRange<Rng>())>
auto capitalize(Rng &&rng, IsLazily* is_lazily=nullptr) {
    auto head =
    rng.utf8_encoded | view::slice(0, 1) | view::transform([](auto c) -> decltype(c) {
        return std::toupper(c);
    });
    auto tail = rng.utf8_encoded | view::slice(1, ranges::end);
    auto result = view::concat(head, std::move(tail));
    return utf8::return_lazily(std::move(result), is_lazily);
}

}
}
