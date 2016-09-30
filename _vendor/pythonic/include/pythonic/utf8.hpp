#pragma once

#include "folly/FBString.h"
#include "folly/FBVector.h"
#include "range/v3/all.hpp"
#include <experimental/optional>
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

template <typename TextView> class TextIterator {

  char const *__data;
  size_t __size;
  char32_t __current_code_point;
  size_t __next_step;
  using my_type = TextIterator<TextView>;

public:
  TextIterator(TextView text)
      : __data(text.c_str()), __size(text.code_units_count()), __next_step(0),
        __current_code_point(0) {
    advance();
  }
  TextIterator(TextView text, size_t current_pos)
      : __data(text.c_str() + current_pos), __size(0), __next_step(0),
        __current_code_point(0) {}

  char32_t operator*() { return __current_code_point; }

  my_type &operator++() {
    advance();
    return *this;
  }

  my_type operator++(int) {
    my_type tmp(*this);
    operator++();
    return tmp;
  }

  bool operator==(my_type const &right) {
    return __data == right.__data && __size == right.__size;
  }

  bool operator!=(my_type const &right) { return !(*this == right); }

  char const *current() { return __data; }

private:
  void advance() {
    if (__size > 0) {
      __data += __next_step;
      __size -= __next_step;
      __next_step = decode_utf8(__data, __size, __current_code_point);
    }
  }
};

class RawTextOwner {
public:
  constexpr RawTextOwner() noexcept : __data(nullptr), __size(0) {}
  RawTextOwner(RawTextOwner const &that) noexcept = default;
  constexpr RawTextOwner(folly::fbstring const &data) noexcept
      : __data(data.c_str()), __size(data.size()) {}
  constexpr RawTextOwner(folly::fbstring data) noexcept = delete;
  constexpr RawTextOwner(char const *data, size_t size) noexcept
      : __data(data), __size(size) {}

  char const *begin() const { return __data; }
  size_t size() const { return __size; }
  RawTextOwner substr(size_t pos, size_t size) const {
    return RawTextOwner(__data + pos, size);
  }

private:
  char const *__data;
  size_t __size;
};

class SharedTextOwner {
public:
  SharedTextOwner() noexcept {}
  SharedTextOwner(SharedTextOwner const &that) = default;
  SharedTextOwner(SharedTextOwner &&that) noexcept
      : __data(std::move(that.__data)) {}
  SharedTextOwner(folly::fbstring data) noexcept : __data(std::move(data)) {}
  SharedTextOwner(char const *data, size_t len) noexcept {
    __data = __data.append(data, len);
  }

  SharedTextOwner &operator=(SharedTextOwner const &that) {
    __data = that.__data;
    return *this;
  }
  SharedTextOwner &operator=(SharedTextOwner &&that) {
    __data = std::move(that.__data);
    return *this;
  }

  char const *begin() const { return __data.c_str(); }
  size_t size() const { return __data.size(); }
  SharedTextOwner substr(size_t pos, size_t size) const {
    return SharedTextOwner(__data.substr(pos, size));
  }

private:
  folly::fbstring __data;
};

template <typename TextOwner> class TextReadOperations {
  using my_type = TextReadOperations<TextOwner>;
  TextOwner __owner; // owner of the block of memory
public:
  constexpr TextReadOperations() noexcept : __owner(TextOwner()) {}
  constexpr TextReadOperations(TextReadOperations<TextOwner> const &that)
      : __owner(that.__owner) {}
  constexpr TextReadOperations(TextReadOperations<TextOwner> &&that)
      : __owner(std::move(that.__owner)) {}
  TextReadOperations(TextOwner owner) noexcept : __owner(std::move(owner)) {}
  TextReadOperations(folly::fbstring const &data) noexcept
      : __owner(TextOwner(data)) {}
  explicit TextReadOperations(folly::fbstring data) noexcept
      : __owner(TextOwner(std::move(data))) {}
  TextReadOperations(std::string const &data) noexcept
      : __owner(TextOwner(data.c_str(), data.size())) {}
  TextReadOperations(const char *data) noexcept
      : __owner(TextOwner(data, strlen(data))) {}
  constexpr TextReadOperations(const char *data, size_t size) noexcept
      : __owner(TextOwner(data, size)) {}

  my_type &operator=(my_type const &that) {
    __owner = that.__owner;
    return *this;
  }

  my_type &operator=(my_type &&that) {
    __owner = std::move(that.__owner);
    return *this;
  }

  constexpr size_t code_units_count() const noexcept { return __owner.size(); }
  constexpr char const *c_str() const noexcept { return __owner.begin(); }
  constexpr char const *code_units_begin() const noexcept {
    return __owner.begin();
  }
  constexpr char const *code_units_end() const noexcept {
    return __owner.begin() + __owner.size();
  }
  constexpr my_type code_units(size_t pos, size_t size) const noexcept {
    return my_type(__owner.substr(pos, size));
  }

  TextIterator<my_type> begin() { return TextIterator<my_type>(*this); }

  TextIterator<my_type> end() {
    return TextIterator<my_type>(*this, code_units_count());
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

  my_type
  operator[](rng_detail::slice_bounds<size_t, ranges::end_fn> offs) const
      noexcept {
    return (*this)[{offs.from, code_points_count()}];
  }

  my_type operator[](
      rng_detail::slice_bounds<size_t, rng_detail::from_end_<long>> offs) const
      noexcept {
    return (*this)[{offs.from, long(code_points_count()) + offs.to.dist_}];
  }

  my_type operator[](
      rng_detail::slice_bounds<rng_detail::from_end_<long>, ranges::end_fn>
          offs) const noexcept {
    return (*this)[{long(code_points_count()) + offs.from.dist_,
                    code_points_count()}];
  }

  my_type
  operator[](rng_detail::slice_bounds<rng_detail::from_end_<long>> offs) const
      noexcept {
    return (*this)[{long(code_points_count()) + offs.from.dist_,
                    long(code_points_count()) + offs.to.dist_}];
  }

  my_type operator[](rng_detail::slice_bounds<long> offs) const noexcept {
    auto cur = this->code_units_begin();
    auto end = this->code_units_end();
    decltype(this->code_units_begin()) sub_begin = nullptr;
    auto sub_end = end;
    auto visited = 0;
    if (offs.to <= offs.from) {
      return my_type();
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
      return my_type();
    }
    return code_units(sub_begin - code_units_begin(),
                      size_t(sub_end - sub_begin));
  }

  template <typename T> size_t find(T needle) {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      return npos;
    }
    for (auto cur = code_units_begin(); cur < code_units_end() - needle_len;
         cur++) {
      if (0 == strncmp(cur, needle.code_units_begin(), needle_len)) {
        return size_t(cur - code_units_begin());
      }
    }
    return npos;
  }

  size_t find(char const *needle) {
    return find(TextReadOperations<RawTextOwner>(needle));
  }

  template <typename T> size_t rfind(T needle) const {
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      return npos;
    }
    for (auto cur = code_units_end() - needle_len - 1;
         cur != code_units_begin() - 1; cur--) {
      if (0 == strncmp(cur, needle.code_units_begin(), needle_len)) {
        return size_t(cur - code_units_begin());
      }
    }
    return npos;
  }

  size_t rfind(char const *needle) {
    return rfind(TextReadOperations<RawTextOwner>(needle));
  }

  my_type lstrip() const noexcept {
    for (auto cur = code_units_begin(); cur != code_units_end(); cur++) {
      if (!std::isspace(*cur)) {
        return code_units(cur - code_units_begin(),
                          size_t(code_units_end() - cur));
      }
    }
    return my_type();
  }

  my_type rstrip() const noexcept {
    for (auto cur = code_units_end() - 1; cur != code_units_begin() - 1;
         cur--) {
      if (!std::isspace(*cur)) {
        return code_units(0, size_t(cur - code_units_begin() + 1));
      }
    }
    return my_type();
  }

  my_type strip() const noexcept { return lstrip().rstrip(); }

  template <typename T> bool startswith(T right) const noexcept {
    if (right.code_units_count() > code_units_count()) {
      return false;
    }
    return 0 == strncmp(code_units_begin(), right.code_units_begin(),
                        right.code_units_count());
  }

  bool startswith(char const *right) const noexcept {
    return startswith(TextReadOperations<RawTextOwner>(right));
  }

  template <typename T> bool endswith(T right) const noexcept {
    if (right.code_units_count() > code_units_count()) {
      return false;
    }
    return 0 == strncmp(code_units_end() - right.code_units_count(),
                        right.code_units_begin(), right.code_units_count());
  }

  bool endswith(char const *right) const noexcept {
    return endswith(TextReadOperations<RawTextOwner>(right));
  }

  folly::fbstring lower() const {
    auto out = folly::fbstring{};
    out.reserve(code_units_count());
    for (auto cur = code_units_begin(); cur != code_units_end(); cur++) {
      out.push_back(char(std::tolower(*cur)));
    }
    return out;
  }

  folly::fbstring upper() const {
    auto out = folly::fbstring{};
    out.reserve(code_units_count());
    for (auto cur = code_units_begin(); cur < code_units_end(); cur++) {
      out.push_back(char(std::toupper(*cur)));
    }
    return out;
  }

  template <typename T> folly::fbvector<my_type> split(T needle) const {
    auto parts = folly::fbvector<my_type>{};
    auto needle_len = needle.code_units_count();
    auto cur = code_units_begin();
    auto last = code_units_begin();
    if (needle_len == 0) {
      parts.push_back(*this);
      return parts;
    }
    for (; cur <= code_units_end() - needle_len; cur++) {
      if (0 ==
          strncmp(cur, needle.code_units_begin(), needle.code_units_count())) {
        parts.emplace_back(
            __owner.substr(last - code_units_begin(), cur - last));
        cur += needle_len;
        last = cur;
      }
    }
    parts.emplace_back(
        __owner.substr(last - code_units_begin(), code_units_end() - last));
    return parts;
  }

  folly::fbvector<my_type> split(char const *needle) const {
    return split(TextReadOperations<RawTextOwner>(needle));
  }

  template <typename T1, typename T2>
  folly::fbstring replace(T1 needle, T2 replacement, size_t count = -1) {
    auto out = folly::fbstring{};
    auto needle_len = needle.code_units_count();
    if (needle_len == 0) {
      out.append(code_units_begin(), code_units_count());
      return out;
    }
    auto last = code_units_begin();
    for (auto cur = code_units_begin();
         cur != code_units_end() - needle_len && count > 0;) {
      if (0 ==
          strncmp(cur, needle.code_units_begin(), needle.code_units_count())) {
        count--;
        out.append(last, cur - last);
        out.append(replacement.code_units_begin(),
                   replacement.code_units_count());
        cur += needle_len;
        last = cur;
      } else {
        cur++;
      }
    }
    out.append(last, code_units_end() - last);
    return out;
  }

  folly::fbstring replace(char const *needle, char const *replacement,
                          size_t count = -1) {
    return replace(TextReadOperations<RawTextOwner>(needle),
                   TextReadOperations<RawTextOwner>(replacement), count);
  }
};

using TextView = TextReadOperations<RawTextOwner>;
using SharedText = TextReadOperations<SharedTextOwner>;

namespace literals {
constexpr TextView operator"" _v(char const *self, size_t len) {
  return TextView(self, len);
}
}
template <typename TextOwner>
size_t len(TextReadOperations<TextOwner> input) noexcept {
  return input.code_points_count();
}
size_t len(char const *input) noexcept { return len(TextView(input)); }

template <typename TextOwner>
std::ostream &operator<<(std::ostream &os,
                         TextReadOperations<TextOwner> const &value) {
  os.write(value.code_units_begin(), long(value.code_units_count()));
  return os;
}

template <typename TextOwner1, typename TextOwner2>
bool operator==(TextReadOperations<TextOwner1> const &left,
                TextReadOperations<TextOwner2> const &right) {
  if (left.code_units_count() != right.code_units_count()) {
    return false;
  }
  return 0 == strncmp(left.code_units_begin(), right.code_units_begin(),
                      left.code_units_count());
}

template <typename TextOwner>
bool operator==(TextReadOperations<TextOwner> const &left, char const *right) {
  return left == TextView(right);
}
template <typename TextOwner>
bool operator==(char const *left, TextReadOperations<TextOwner> const &right) {
  return TextView(left) == right;
}

template <typename TextOwner1, typename TextOwner2>
bool operator!=(TextReadOperations<TextOwner1> const &left,
                TextReadOperations<TextOwner2> const &right) {
  return !(left == right);
}

template <typename TextOwner>
bool operator!=(TextReadOperations<TextOwner> const &left, char const *right) {
  return left != TextView(right);
}
template <typename TextOwner>
bool operator!=(char const *left, TextReadOperations<TextOwner> const &right) {
  return TextView(left) != right;
}

class ConcatedTexts : public vector<TextView> {
public:
  using vector<TextView>::vector;

  ConcatedTexts(TextView const &text) { push_back(text); }

  template <typename S> void to_str(S &out) const {
    auto len = size_t(0);
    for (auto const &view : *this) {
      len += view.code_units_count();
    }
    out.reserve(len);
    char *cur = const_cast<char *>(out.data());
    for (auto const &view : *this) {
      auto view_len = view.code_units_count();
      out.append(view.c_str(), view_len);
      cur += view_len;
    }
  }

  ConcatedTexts &operator+=(ConcatedTexts const &that) {
    insert(end(), that.begin(), that.end());
    return *this;
  }

  ConcatedTexts &operator+=(TextView const &that) {
    push_back(that);
    return *this;
  }

  folly::fbstring to_str() const {
    folly::fbstring out;
    to_str(out);
    return out;
  }

  operator folly::fbstring() const {
    folly::fbstring out;
    to_str(out);
    return out;
  }

  operator std::string() const {
    std::string out;
    to_str(out);
    return out;
  }
};

ConcatedTexts operator+(TextView const &left, TextView const &right) {
  auto views = ConcatedTexts{};
  views.push_back(left);
  views.push_back(right);
  return views;
}

ConcatedTexts operator+(ConcatedTexts views, TextView const &one_view) {
  views.push_back(one_view);
  return views;
}

ConcatedTexts operator+(TextView const &one_view, ConcatedTexts views) {
  views.insert(views.begin(), one_view);
  return views;
}

ConcatedTexts operator+(ConcatedTexts left, ConcatedTexts const &right) {
  left.insert(left.end(), right.begin(), right.end());
  return left;
}
}
}

namespace std {
using pythonic::utf8::TextView;
using pythonic::utf8::SharedText;
template <> struct hash<TextView> {
  size_t operator()(TextView const &x) const {
    size_t hash = 0;
    for (auto cur = x.code_units_begin(); cur != x.code_units_end(); cur++) {
      hash = (hash << 5) - hash + *cur;
    }
    return hash;
  }
};
template <> struct hash<SharedText> {
  size_t operator()(SharedText const &x) const {
    size_t hash = 0;
    for (auto cur = x.code_units_begin(); cur != x.code_units_end(); cur++) {
      hash = (hash << 5) - hash + *cur;
    }
    return hash;
  }
};
}
