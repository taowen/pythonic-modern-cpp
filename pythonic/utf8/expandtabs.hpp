#pragma once

#include "finditer.hpp"
#include "Utf8Encoded.hpp"
#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename Rng,typename Result=folly::fbstring,
          CONCEPT_REQUIRES_(utf8::Utf8EncodedRandomAccessRange<Rng>())>
auto expandtabs(Rng &&haystack, size_t tab_size) {
    auto spaces = Result();
    spaces.reserve(tab_size);
    for (auto i = 0; i < tab_size; i++) {
        spaces.push_back(' ');
    }
    return utf8::sub(haystack, "\t"_u, [&spaces, tab_size](auto i){
        auto col = static_cast<int>(i / tab_size);
        auto spaces_count = (col + 1) * tab_size - i;
        return utf8::utf8_cast(std::string_view(spaces.begin(), spaces_count)); 
    });
}

}
}
