#pragma once

#include "Utf8Encoded.hpp"

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename T>
auto code_units(utf8::Utf8Encoded<T> &&input) {
    return input.utf8_encoded;
}

}
}
