#pragma once

namespace pythonic {
namespace utf8 {
inline namespace v1 {
template <typename T> class Utf8Encoded {
  using my_type = Utf8Encoded<T>;

public:
  T data;
  Utf8Encoded(T data) : data(data) {}
  Utf8Encoded() {}
  Utf8Encoded(my_type const &that) : data(that.data) {}
  Utf8Encoded(my_type &&that) : data(std::move(that.data)) {}
  my_type &operator=(my_type const &that) {
    data = that.data;
    return *this;
  }
  my_type &operator=(my_type &&that) {
    data = std::move(that.data);
    return *this;
  }
  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto size() { return data.size(); }
};

Utf8Encoded<string_view> utf8_cast(char const *data) {
  return Utf8Encoded<string_view>{data};
}

template <typename T> Utf8Encoded<T> utf8_cast(T data) {
  return Utf8Encoded<T>{data};
}

#define U8(X) pythonic::utf8::utf8_cast(u8##X)
}
}
}
