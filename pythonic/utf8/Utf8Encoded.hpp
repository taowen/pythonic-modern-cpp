#pragma once

#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename T> class Utf8Encoded {
public:
  using utf8_range_type = T;
  T utf8_encoded;
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

constexpr TextView utf8_cast(char const *data) { return TextView{data}; }

template <typename T> constexpr auto utf8_cast(T &&data) {
  return Utf8Encoded<T>{std::forward<T>(data)};
}

#define U8(X) pythonic::utf8::utf8_cast(u8##X)

namespace concepts {
struct CharRange : ranges::concepts::refines<ranges::concepts::Range> {
  template <typename T>
  using value_t = ranges::concepts::Readable::value_t<iterator_t<T>>;

  template <typename T>
  auto requires_(T &&t) -> decltype(ranges::concepts::valid_expr(
      ranges::concepts::same_type('0', value_t<T>())));
};
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
template <typename T>
using CharRange = ranges::concepts::models<concepts::CharRange, T>;

#define IF_CONSTEXPR                                                           \
  if                                                                           \
  constexpr

template <typename View> struct Utf8View : ranges::pipeable<Utf8View<View>> {
private:
  View view_;
  friend ranges::pipeable_access;

  template <typename Rng, typename... Rest>
  using ViewConcept = meta::and_<
      Utf8EncodedRange<Rng>,
      ranges::Function<View, typename Rng::utf8_range_type, Rest...>>;

  // Pipeing requires range arguments or lvalue containers.
  template <typename Rng, typename Vw,
            CONCEPT_REQUIRES_(Utf8EncodedRange<Rng>())>
  static auto pipe(Rng &&rng, Vw &&v) {
    auto result =
        v.view_(std::forward<decltype(rng.utf8_encoded)>(rng.utf8_encoded));
    IF_CONSTEXPR(CharRange<decltype(result)>()) {
      return Utf8Encoded<decltype(result)>{result};
    }
    else {
      return result;
    }
  }

public:
  Utf8View() = default;
  Utf8View(View a) : view_(std::move(a)) {}

  template <typename Rng, typename... Rest,
            CONCEPT_REQUIRES_(ViewConcept<Rng, Rest...>())>
  auto operator()(Rng &&rng, Rest &&... rest) const {
    auto result =
        view_(std::forward<decltype(rng.utf8_encoded)>(rng.utf8_encoded),
              std::forward<Rest>(rest)...);
    IF_CONSTEXPR(CharRange<decltype(result)>()) {
      return Utf8Encoded<decltype(result)>{result};
    }
    else {
      return result;
    }
  }

  // Currying overload.
  template <typename... Ts, typename V = View>
  auto operator()(Ts &&... ts) const {
    auto func =
        view::view_access::impl<V>::bind(view_, std::forward<Ts>(ts)...);
    return Utf8View<decltype(func)>{func};
  }
};

struct to_text_fn {
  template <typename Rng> auto operator()(Rng &&rng) const {
    folly::fbstring text;
    text.reserve(static_cast<folly::fbstring::size_type>(ranges::size(rng)));
    using I = ranges::range_common_iterator_t<Rng>;
    text.assign(I{ranges::begin(rng)}, I{ranges::end(rng)});
    return std::move(text);
  }
};
auto to_text = Utf8View<to_text_fn>();
}
}
