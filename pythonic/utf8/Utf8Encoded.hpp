#pragma once

#include <folly/FBString.h>
#include <range/v3/all.hpp>

namespace pythonic {
namespace utf8 {

namespace view = ranges::view;

template <typename T> class Utf8Encoded {
public:
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

template <typename T> constexpr TextView utf8_cast(T data) {
  return TextView{data};
}

#define U8(X) pythonic::utf8::utf8_cast(u8##X)

template <typename View> struct Utf8View : ranges::pipeable<Utf8View<View>> {
private:
  View view_;
  friend ranges::pipeable_access;

  template <typename Rng, typename... Rest>
  using ViewConcept = meta::and_<view::ViewableRange<Rng>,
                                 ranges::Function<View, Rng, Rest...>>;

  // Pipeing requires range arguments or lvalue containers.
  template <typename Rng, typename Vw, CONCEPT_REQUIRES_(ViewConcept<Rng>())>
  static auto pipe(Rng &&rng, Vw &&v) {
    return v.view_(std::forward<Rng>(rng));
  }

  template <typename Rng, typename Vw>
  static auto pipe(Utf8Encoded<Rng> &&rng, Vw &&v) {
    auto result = v.view_(std::forward<Rng>(rng.utf8_encoded));
    return Utf8Encoded<decltype(result)>{result};
  }

#ifndef RANGES_DOXYGEN_INVOKED
  // For better error messages:
  template <typename Rng, typename Vw, CONCEPT_REQUIRES_(!ViewConcept<Rng>())>
  static void pipe(Rng &&, Vw &&) {
    CONCEPT_ASSERT_MSG(ranges::Range<Rng>(),
                       "The type Rng must be a model of the Range concept.");
    // BUGBUG This isn't a very helpful message. This is probably the wrong
    // place
    // to put this check:
    CONCEPT_ASSERT_MSG(ranges::Function<View, Rng>(),
                       "This view is not callable with this range type.");
    static_assert(
        ranges::View<Rng>() || std::is_lvalue_reference<Rng>(),
        "You can't pipe an rvalue container into a view. First, save"
        "the container into a named variable, and then pipe it to the view.");
  }
#endif
public:
  Utf8View() = default;
  Utf8View(View a) : view_(std::move(a)) {}
  // Calling directly requires View arguments or lvalue containers.
  template <typename Rng, typename... Rest,
            CONCEPT_REQUIRES_(ViewConcept<Rng, Rest...>())>
  auto operator()(Rng &&rng, Rest &&... rest) const {
    return view_(std::forward<Rng>(rng), std::forward<Rest>(rest)...);
  }
  // Currying overload.
  template <typename... Ts, typename V = View>
  auto operator()(Ts &&... ts) const {
    auto func =
        view::view_access::impl<V>::bind(view_, std::forward<Ts>(ts)...);
    return Utf8View<decltype(func)>{func};
  }

  template <typename Rng, typename... Rest,
            CONCEPT_REQUIRES_(ranges::Range<Rng>())>
  auto operator()(Utf8Encoded<Rng> &&rng, Rest &&... rest) const {
    return view_(std::forward<Rng>(rng.utf8_encoded),
                 std::forward<Rest>(rest)...);
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
