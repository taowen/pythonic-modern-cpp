#include "catch.hpp"
#include "pythonic/utf8.hpp"
#include "range/v3/all.hpp"
#include <string>
#include <unordered_map>

namespace Catch {

using namespace pythonic;
using namespace std;

utf8::ConcatedTexts describe(string const &str) {
  return "\"" + utf8::TextView(str) + "\"";
}
utf8::ConcatedTexts describe(char const *str) {
  return "\"" + utf8::TextView(str) + "\"";
}
utf8::ConcatedTexts describe(utf8::TextView str) { return "\"" + str + "\""; }
utf8::ConcatedTexts describe(utf8::SharedText str) {
  return "\"" + utf8::TextView(str.code_units_begin(), str.code_units_count()) +
         "\"";
}

template <typename T> string describe(const T &obj) { return to_string(obj); }

template <typename key_type, typename value_type>
struct StringMaker<unordered_map<key_type, value_type>> {
  static std::string convert(unordered_map<key_type, value_type> const &value) {
    auto desc = utf8::ConcatedTexts{"{ "};
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
