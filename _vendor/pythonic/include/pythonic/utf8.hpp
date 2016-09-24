#pragma once

#include "range/v3/all.hpp"
#include <iostream>

using namespace std;
namespace rng_detail = ranges::detail;

namespace pythonic {
namespace utf8 {

const auto npos = size_t(-1);

inline size_t decode_utf8(const char *src, size_t n, char32_t &dst) noexcept {
  auto code = reinterpret_cast<const uint8_t *>(src);
  if (code[0] <= 0xc1 || n < 2) {
    dst = code[0];
    return 1;
  } else if (code[0] <= 0xdf || n < 3) {
    dst = (char32_t(code[0] & 0x1f) << 6) | char32_t(code[1] & 0x3f);
    return 2;
  } else if (code[0] <= 0xef || n < 4) {
    dst = (char32_t(code[0] & 0x0f) << 12) | (char32_t(code[1] & 0x3f) << 6) |
          char32_t(code[2] & 0x3f);
    return 3;
  } else {
    dst = (char32_t(code[0] & 0x07) << 18) | (char32_t(code[1] & 0x3f) << 12) |
          (char32_t(code[2] & 0x3f) << 6) | char32_t(code[3] & 0x3f);
    return 4;
  }
}

inline size_t decode_utf8(const char *src, size_t n) noexcept {
  auto code = reinterpret_cast<const uint8_t *>(src);
  if (code[0] <= 0xc1 || n < 2) {
    return 1;
  } else if (code[0] <= 0xdf || n < 3) {
    return 2;
  } else if (code[0] <= 0xef || n < 4) {
    return 3;
  } else {
    return 4;
  }
}

class text_view;
bool operator==(text_view const &left, text_view const &right);

class text_view {
public:
  constexpr text_view() noexcept : __data(nullptr), __size(0) {}
  text_view(char const *data) noexcept : __data(data), __size(strlen(data)) {}
  constexpr text_view(string const &str) noexcept
      : __data(str.c_str()), __size(str.size()) {}
  constexpr text_view(char const *data, size_t size) noexcept
      : __data(data), __size(size) {}
  constexpr size_t code_units_count() const noexcept { return __size; }
  constexpr char const *begin() const noexcept { return __data; }
  constexpr char const *end() const noexcept { return __data + __size; }
  size_t code_points_count() const noexcept {
    auto end_iter = end();
    auto length = size_t(0);
    for (auto cur_iter = begin(); cur_iter != end_iter;) {
      auto adv = decode_utf8(cur_iter, size_t(end_iter - cur_iter));
      cur_iter += adv;
      length++;
    }
    return length;
  }

  text_view
  operator[](rng_detail::slice_bounds<size_t, ranges::end_fn> offs) const
      noexcept {
    return (*this)[{offs.from, code_points_count()}];
  }

  text_view operator[](
      rng_detail::slice_bounds<size_t, rng_detail::from_end_<long>> offs) const
      noexcept {
    return (*this)[{offs.from, long(code_points_count()) + offs.to.dist_}];
  }

  text_view operator[](
      rng_detail::slice_bounds<rng_detail::from_end_<long>, ranges::end_fn>
          offs) const noexcept {
    return (*this)[{long(code_points_count()) + offs.from.dist_,
                    code_points_count()}];
  }

  text_view
  operator[](rng_detail::slice_bounds<rng_detail::from_end_<long>> offs) const
      noexcept {
    return (*this)[{long(code_points_count()) + offs.from.dist_,
                    long(code_points_count()) + offs.to.dist_}];
  }

  text_view operator[](rng_detail::slice_bounds<long> offs) const noexcept {
    auto cur = this->begin();
    auto end = this->end();
    decltype(this->begin()) sub_begin = nullptr;
    auto sub_end = end;
    auto visited = 0;
    if (offs.to <= offs.from) {
      return text_view();
    }
    while (cur != end) {
      if (visited == offs.from) {
        sub_begin = cur;
      }
      if (visited == offs.to) {
        sub_end = cur;
        break;
      }
      auto adv = decode_utf8(cur, size_t(end - cur));
      cur += adv;
      visited++;
    }
    if (sub_begin == nullptr) {
      return text_view();
    }
    return text_view(sub_begin, size_t(sub_end - sub_begin));
  }

  size_t find(text_view needle) {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      return npos;
    }
    for (auto cur = begin(); cur < end() - needle_len; cur++) {
      if (text_view(cur, needle_len) == needle) {
        return size_t(cur - begin());
      }
    }
    return npos;
  }

  size_t rfind(text_view needle) const {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      return npos;
    }
    for (auto cur = end() - needle_len - 1; cur != begin() - 1; cur--) {
      if (text_view(cur, needle_len) == needle) {
        return size_t(cur - begin());
      }
    }
    return npos;
  }

  text_view lstrip() const noexcept {
    for (auto cur = begin(); cur != end(); cur++) {
      if (!std::isspace(*cur)) {
        return text_view(cur, size_t(end() - cur));
      }
    }
    return text_view();
  }

  text_view rstrip() const noexcept {
    for (auto cur = end() - 1; cur != begin() - 1; cur--) {
      if (!std::isspace(*cur)) {
        return text_view(begin(), size_t(cur - begin() + 1));
      }
    }
    return text_view();
  }

  text_view strip() const noexcept { return lstrip().rstrip(); }

  bool startswith(text_view right) const noexcept {
    if (right.code_units_count() > code_units_count()) {
      return false;
    }
    return text_view(begin(), right.code_units_count()) == right;
  }

  bool endswith(text_view right) const noexcept {
    if (right.code_units_count() > code_units_count()) {
      return false;
    }
    return text_view(end() - right.code_units_count(),
                     right.code_units_count()) == right;
  }

  template <typename S> text_view lower(S &out) const {
    for (auto c : *this) {
      out.push_back(char(std::tolower(c)));
    }
    return out;
  }

  string lower() const {
    auto out = string{};
    lower(out);
    return out;
  }

  template <typename S> text_view upper(S &out) const {
    for (auto c : *this) {
      out.push_back(char(std::toupper(c)));
    }
    return out;
  }

  string upper() const {
    auto out = string{};
    upper(out);
    return out;
  }

  template <typename V> auto split(text_view needle, V &parts) const {
    auto needle_len = needle.code_units_count();
    auto cur = begin();
    auto last = begin();
    if (needle_len == 0) {
      parts.push_back(*this);
      return parts;
    }
    for (; cur <= end() - needle_len; cur++) {
      if (text_view(cur, needle_len) == needle) {
        parts.emplace_back(last, cur - last);
        cur += needle_len;
        last = cur;
      }
    }
    parts.emplace_back(last, end() - last);
    return parts;
  }

  vector<text_view> split(text_view needle) const {
    auto parts = vector<text_view>{};
    split(needle, parts);
    return parts;
  }

  template <typename S>
  text_view replace(text_view needle, text_view replacement, size_t count,
                    S &out) const {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      out.append(__data, __size);
      return out;
    }
    auto last = begin();
    for (auto cur = begin(); cur != end() - needle_len && count > 0;) {
      if (text_view(cur, needle_len) == needle) {
        count--;
        out.append(last, cur - last);
        out.append(replacement.__data, replacement.__size);
        cur += needle_len;
        last = cur;
      } else {
        cur++;
      }
    }
    out.append(last, end() - last);
    return out;
  }

  string replace(text_view needle, text_view replacement, size_t count = -1) {
    auto out = string{};
    replace(needle, replacement, count, out);
    return out;
  }

private:
  char const *__data;
  size_t __size;
};

namespace literals {
constexpr text_view operator"" _v(char const *self, size_t len) {
  return text_view(self, len);
}
}
size_t len(text_view input) noexcept { return input.code_points_count(); }

std::ostream &operator<<(std::ostream &os, text_view const &value) {
  os.write(value.begin(), long(value.code_units_count()));
  return os;
}

bool operator==(text_view const &left, text_view const &right) {
  if (left.code_units_count() != right.code_units_count()) {
    return false;
  }
  auto left_cur = left.begin();
  auto right_cur = right.begin();
  auto remaining = left.code_units_count();
  while (remaining-- > 0) {
    if (*left_cur != *right_cur) {
      return false;
    }
    left_cur++;
    right_cur++;
  }
  return true;
}

bool operator!=(text_view const &left, text_view const &right) {
  return !(left == right);
}

class concated_text_views : public vector<text_view> {
public:
  using vector<text_view>::vector;

  template <typename S> text_view to_str(S &out) {
    auto len = size_t(0);
    for (auto const &view : *this) {
      len += view.code_units_count();
    }
    out.resize(len);
    char *cur = const_cast<char *>(out.data());
    for (auto const &view : *this) {
      auto view_len = view.code_units_count();
      strncpy(cur, view.begin(), view_len);
      cur += view_len;
    }
    return out;
  }

  string to_str() {
    auto out = string{};
    to_str(out);
    return out;
  }
};

concated_text_views operator+(text_view const &left, text_view const &right) {
  auto views = concated_text_views{};
  views.push_back(left);
  views.push_back(right);
  return views;
}

concated_text_views operator+(concated_text_views views,
                              text_view const &one_view) {
  views.push_back(one_view);
  return views;
}

concated_text_views operator+(text_view const &one_view,
                              concated_text_views views) {
  views.insert(views.begin(), one_view);
  return views;
}

concated_text_views operator+(concated_text_views left,
                              concated_text_views const &right) {
  left.insert(left.end(), right.begin(), right.end());
  return left;
}
}
}
