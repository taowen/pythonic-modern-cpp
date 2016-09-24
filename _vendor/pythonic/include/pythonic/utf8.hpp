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

class TextView;
bool operator==(TextView const &left, TextView const &right);

template <typename TextView> class TextViewIterator {
public:
  TextViewIterator(TextView text_view)
      : __data(text_view.c_str()), __size(text_view.code_units_count()),
        __next_step(0), __current_code_point(0) {
    advance();
  }
  TextViewIterator(TextView text_view, size_t current_pos)
      : __data(text_view.c_str() + current_pos), __size(0), __next_step(0),
        __current_code_point(0) {}

  char32_t operator*() { return __current_code_point; }

  TextViewIterator<TextView> &operator++() {
    advance();
    return *this;
  }

  TextViewIterator<TextView> operator++(int) {
    TextViewIterator<TextView> tmp(*this);
    operator++();
    return tmp;
  }

  bool operator==(TextViewIterator<TextView> const &right) {
    return __data == right.__data && __size == right.__size;
  }

  bool operator!=(TextViewIterator<TextView> const &right) {
    return !(*this == right);
  }

  char const *current() { return __data; }

private:
  void advance() {
    if (__size > 0) {
      __data += __next_step;
      __size -= __next_step;
      __next_step = decode_utf8(__data, __size, __current_code_point);
    }
  }
  char const *__data;
  size_t __size;
  char32_t __current_code_point;
  size_t __next_step;
};

class TextView {
public:
  constexpr TextView() noexcept : __data(nullptr), __size(0) {}
  TextView(char const *data) noexcept : __data(data), __size(strlen(data)) {}
  constexpr TextView(string const &str) noexcept
      : __data(str.c_str()), __size(str.size()) {}
  constexpr TextView(char const *data, size_t size) noexcept
      : __data(data), __size(size) {}
  constexpr size_t code_units_count() const noexcept { return __size; }
  constexpr char const *c_str() const noexcept { return __data; }
  constexpr char const *code_units_begin() const noexcept { return __data; }
  constexpr char const *code_units_end() const noexcept {
    return __data + __size;
  }

  TextViewIterator<TextView> begin() {
    return TextViewIterator<TextView>(*this);
  }

  TextViewIterator<TextView> end() {
    return TextViewIterator<TextView>(*this, code_units_count());
  }

  size_t code_points_count() const noexcept {
    auto end_iter = code_units_end();
    auto length = size_t(0);
    for (auto cur_iter = code_units_begin(); cur_iter != end_iter;) {
      auto adv = decode_utf8(cur_iter, size_t(end_iter - cur_iter));
      cur_iter += adv;
      length++;
    }
    return length;
  }

  TextView
  operator[](rng_detail::slice_bounds<size_t, ranges::end_fn> offs) const
      noexcept {
    return (*this)[{offs.from, code_points_count()}];
  }

  TextView operator[](
      rng_detail::slice_bounds<size_t, rng_detail::from_end_<long>> offs) const
      noexcept {
    return (*this)[{offs.from, long(code_points_count()) + offs.to.dist_}];
  }

  TextView operator[](
      rng_detail::slice_bounds<rng_detail::from_end_<long>, ranges::end_fn>
          offs) const noexcept {
    return (*this)[{long(code_points_count()) + offs.from.dist_,
                    code_points_count()}];
  }

  TextView
  operator[](rng_detail::slice_bounds<rng_detail::from_end_<long>> offs) const
      noexcept {
    return (*this)[{long(code_points_count()) + offs.from.dist_,
                    long(code_points_count()) + offs.to.dist_}];
  }

  TextView operator[](rng_detail::slice_bounds<long> offs) const noexcept {
    auto cur = this->code_units_begin();
    auto end = this->code_units_end();
    decltype(this->code_units_begin()) sub_begin = nullptr;
    auto sub_end = end;
    auto visited = 0;
    if (offs.to <= offs.from) {
      return TextView();
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
      return TextView();
    }
    return TextView(sub_begin, size_t(sub_end - sub_begin));
  }

  size_t find(TextView needle) {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      return npos;
    }
    for (auto cur = code_units_begin(); cur < code_units_end() - needle_len;
         cur++) {
      if (TextView(cur, needle_len) == needle) {
        return size_t(cur - code_units_begin());
      }
    }
    return npos;
  }

  size_t rfind(TextView needle) const {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      return npos;
    }
    for (auto cur = code_units_end() - needle_len - 1;
         cur != code_units_begin() - 1; cur--) {
      if (TextView(cur, needle_len) == needle) {
        return size_t(cur - code_units_begin());
      }
    }
    return npos;
  }

  TextView lstrip() const noexcept {
    for (auto cur = code_units_begin(); cur != code_units_end(); cur++) {
      if (!std::isspace(*cur)) {
        return TextView(cur, size_t(code_units_end() - cur));
      }
    }
    return TextView();
  }

  TextView rstrip() const noexcept {
    for (auto cur = code_units_end() - 1; cur != code_units_begin() - 1;
         cur--) {
      if (!std::isspace(*cur)) {
        return TextView(code_units_begin(),
                        size_t(cur - code_units_begin() + 1));
      }
    }
    return TextView();
  }

  TextView strip() const noexcept { return lstrip().rstrip(); }

  bool startswith(TextView right) const noexcept {
    if (right.code_units_count() > code_units_count()) {
      return false;
    }
    return TextView(code_units_begin(), right.code_units_count()) == right;
  }

  bool endswith(TextView right) const noexcept {
    if (right.code_units_count() > code_units_count()) {
      return false;
    }
    return TextView(code_units_end() - right.code_units_count(),
                    right.code_units_count()) == right;
  }

  template <typename S> TextView lower(S &out) const {
    out.reserve(code_units_count());
    for (auto cur = code_units_begin(); cur != code_units_end(); cur++) {
      out.push_back(char(std::tolower(*cur)));
    }
    return out;
  }

  string lower() const {
    auto out = string{};
    lower(out);
    return out;
  }

  template <typename S> TextView upper(S &out) const {
    out.reserve(code_units_count());
    for (auto cur = code_units_begin(); cur < code_units_end(); cur++) {
      out.push_back(char(std::toupper(*cur)));
    }
    return out;
  }

  string upper() const {
    auto out = string{};
    upper(out);
    return out;
  }

  template <typename V> auto split(TextView needle, V &parts) const {
    auto needle_len = needle.code_units_count();
    auto cur = code_units_begin();
    auto last = code_units_begin();
    if (needle_len == 0) {
      parts.push_back(*this);
      return parts;
    }
    for (; cur <= code_units_end() - needle_len; cur++) {
      if (TextView(cur, needle_len) == needle) {
        parts.emplace_back(last, cur - last);
        cur += needle_len;
        last = cur;
      }
    }
    parts.emplace_back(last, code_units_end() - last);
    return parts;
  }

  vector<TextView> split(TextView needle) const {
    auto parts = vector<TextView>{};
    split(needle, parts);
    return parts;
  }

  template <typename S>
  TextView replace(TextView needle, TextView replacement, size_t count,
                   S &out) const {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      out.append(__data, __size);
      return out;
    }
    auto last = code_units_begin();
    for (auto cur = code_units_begin();
         cur != code_units_end() - needle_len && count > 0;) {
      if (TextView(cur, needle_len) == needle) {
        count--;
        out.append(last, cur - last);
        out.append(replacement.__data, replacement.__size);
        cur += needle_len;
        last = cur;
      } else {
        cur++;
      }
    }
    out.append(last, code_units_end() - last);
    return out;
  }

  string replace(TextView needle, TextView replacement, size_t count = -1) {
    auto out = string{};
    replace(needle, replacement, count, out);
    return out;
  }

private:
  char const *__data;
  size_t __size;
};

namespace literals {
constexpr TextView operator"" _v(char const *self, size_t len) {
  return TextView(self, len);
}
}
size_t len(TextView input) noexcept { return input.code_points_count(); }

std::ostream &operator<<(std::ostream &os, TextView const &value) {
  os.write(value.code_units_begin(), long(value.code_units_count()));
  return os;
}

bool operator==(TextView const &left, TextView const &right) {
  if (left.code_units_count() != right.code_units_count()) {
    return false;
  }
  auto left_cur = left.code_units_begin();
  auto right_cur = right.code_units_begin();
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

bool operator!=(TextView const &left, TextView const &right) {
  return !(left == right);
}

class ConcatedTextViews : public vector<TextView> {
public:
  using vector<TextView>::vector;

  template <typename S> TextView to_str(S &out) {
    auto len = size_t(0);
    for (auto const &view : *this) {
      len += view.code_units_count();
    }
    out.resize(len);
    char *cur = const_cast<char *>(out.data());
    for (auto const &view : *this) {
      auto view_len = view.code_units_count();
      strncpy(cur, view.c_str(), view_len);
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

ConcatedTextViews operator+(TextView const &left, TextView const &right) {
  auto views = ConcatedTextViews{};
  views.push_back(left);
  views.push_back(right);
  return views;
}

ConcatedTextViews operator+(ConcatedTextViews views, TextView const &one_view) {
  views.push_back(one_view);
  return views;
}

ConcatedTextViews operator+(TextView const &one_view, ConcatedTextViews views) {
  views.insert(views.begin(), one_view);
  return views;
}

ConcatedTextViews operator+(ConcatedTextViews left,
                            ConcatedTextViews const &right) {
  left.insert(left.end(), right.begin(), right.end());
  return left;
}
}
}
