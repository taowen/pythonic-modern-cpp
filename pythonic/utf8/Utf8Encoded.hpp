#pragma once

#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace concepts {

struct CharRange : ranges::concepts::refines<ranges::concepts::Range> {
    template <typename T>
    using value_t = ranges::concepts::Readable::value_t<iterator_t<T>>;

    template <typename T>
    auto requires_(T &&t) -> decltype(ranges::concepts::valid_expr(
                                          ranges::concepts::same_type('0', value_t<T>())));
};
}
template <typename T>
using CharRange = ranges::concepts::models<concepts::CharRange, T>;

namespace view = ranges::view;

template <typename T, CONCEPT_REQUIRES_(CharRange<T>())> class Utf8Encoded {
public:
    using utf8_range_type = T;
    T utf8_encoded;

    operator Utf8Encoded<folly::fbstring>() {
        return Utf8Encoded<folly::fbstring> {
            folly::fbstring(utf8_encoded.begin(), utf8_encoded.size())
        };
    }
};

using Text = Utf8Encoded<folly::fbstring>;
using TextView = Utf8Encoded<std::string_view>;

bool operator==(std::string_view left, folly::fbstring const &right) {
    return left == std::string_view(right.c_str(), right.size());
}

template <typename T1, typename T2>
bool operator==(Utf8Encoded<T1> const &left, Utf8Encoded<T2> const &right) {
    return left.utf8_encoded == right.utf8_encoded;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, Utf8Encoded<T> const &str) {
    os << str.utf8_encoded;
    return os;
}

constexpr TextView utf8_cast(char const *data) {
    return TextView {data};
}

template <typename T> constexpr auto utf8_cast(T &&data) {
    return Utf8Encoded<T> {std::forward<T>(data)};
}

#define U8(X) pythonic::utf8::utf8_cast(u8##X)

namespace concepts {
struct Utf8EncodedRange {
    template <typename T>
    auto requires_(T &&t) -> decltype(ranges::concepts::valid_expr(
                                          ranges::concepts::model_of<ranges::concepts::Range>(t.utf8_encoded)));
};
struct Utf8EncodedSizedRange {
    template <typename T>
    auto requires_(T &&t) -> decltype(ranges::concepts::valid_expr(
                                          ranges::concepts::model_of<ranges::concepts::SizedRange>(
                                                  t.utf8_encoded)));
};
struct Utf8EncodedRandomAccessRange {
    template <typename T>
    auto requires_(T &&t) -> decltype(ranges::concepts::valid_expr(
                                          ranges::concepts::model_of<ranges::concepts::RandomAccessRange>(
                                                  t.utf8_encoded)));
};
}

template <typename T>
using Utf8EncodedRange =
    ranges::concepts::models<concepts::Utf8EncodedRange, T>;
template <typename T>
using Utf8EncodedSizedRange =
    ranges::concepts::models<concepts::Utf8EncodedSizedRange, T>;
template <typename T>
using Utf8EncodedRandomAccessRange =
    ranges::concepts::models<concepts::Utf8EncodedRandomAccessRange, T>;

template <typename Rng, CONCEPT_REQUIRES_(ranges::Range<Rng>())> auto to_text(Rng &&rng) {
    folly::fbstring text;
    text.reserve(static_cast<folly::fbstring::size_type>(ranges::size(rng)));
    using I = ranges::range_common_iterator_t<Rng>;
    text.assign(I {ranges::begin(rng)}, I {ranges::end(rng)});
    return std::move(text);
}


template <typename Rng, CONCEPT_REQUIRES_(Utf8EncodedRange<Rng>())> auto to_text(Rng &&rng) {
    return utf8_cast(to_text(rng.utf8_encoded));
}

namespace literals {
constexpr auto operator"" _u ( char const* c, size_t l )
{
    return utf8_cast(std::string_view(c, l));
}
}

}
}
