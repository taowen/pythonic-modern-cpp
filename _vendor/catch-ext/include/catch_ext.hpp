#include "catch.hpp"
#include "pythonic/utf8.hpp"
#include "range/v3/all.hpp"
#include <string>
#include <unordered_map>

namespace Catch {

using namespace pythonic;
using namespace std;

utf8::ConcatedTextViews describe(string const &str) {
  return "\"" + utf8::TextView(str) + "\"";
}
utf8::ConcatedTextViews describe(char const *str) {
  return "\"" + utf8::TextView(str) + "\"";
}
utf8::ConcatedTextViews describe(utf8::TextView str) {
  return "\"" + str + "\"";
}

template <typename T> string describe(const T &obj) { return to_string(obj); }

template <typename key_type, typename value_type>
struct StringMaker<unordered_map<key_type, value_type>> {
  static std::string convert(unordered_map<key_type, value_type> const &value) {
    auto desc = utf8::ConcatedTextViews{"{ "};
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
