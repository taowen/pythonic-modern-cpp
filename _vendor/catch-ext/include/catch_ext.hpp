#include <catch.hpp>
#include <folly/FBString.h>
#include <pythonic/utf8/Utf8Encoded.hpp>
#include <range/v3/all.hpp>
#include <string>
#include <unordered_map>

namespace Catch {

using namespace pythonic;
using namespace std;

std::string describe(std::string_view str) {
  auto desc = std::string(str.begin(), str.end());
  return "\"" + desc + "\"";
}

std::string describe(folly::fbstring const &str) {
  auto desc = std::string(str.begin(), str.end());
  return "\"" + desc + "\"";
}

template <typename T> struct StringMaker<utf8::Utf8Encoded<T>> {
  static std::string convert(utf8::Utf8Encoded<T> const &value) {
    return describe(value.data);
  }
};

template <typename key_type, typename value_type>
struct StringMaker<unordered_map<key_type, value_type>> {
  static std::string convert(unordered_map<key_type, value_type> const &value) {
    auto desc = std::string{"{ "};
    for (const auto &e : value) {
      desc += "{ ";
      desc += describe(e.first);
      desc += ", ";
      desc += describe(e.second);
      desc += " } ";
    }
    desc += "}";
    return desc;
  }
};

template <typename key_type, typename value_type>
struct StringMaker<pair<key_type, value_type>> {
  static std::string convert(pair<key_type, value_type> const &value) {
    return "{ " + describe(value.first) + ", " + describe(value.second) + " }";
  }
};
}
