#include <catch_ext.hpp>
#include <catch_with_main.hpp>
#include <codecvt>
#include <experimental/string_view>
#include <fmt/all.hpp>
#include <locale>
#include <pythonic/regexs.hpp>
#include <pythonic/strings.hpp>
#include <range/v3/all.hpp>
#include <regex>
#include <string>
#include <tcb/utf_ranges/view.hpp>

const auto a1 = std::__1::numeric_limits<int>::digits;
const auto a2 =
    std::__1::basic_string_view<char, std::__1::char_traits<char>>::npos;

using namespace std;
using namespace ranges;
using namespace pythonic;

TEST_CASE("001") {
  //! [001]
  auto str1 = string_view("hello world");
  CHECK("o w" == (str1.substr(4, 3)));
  //! [001]
}

TEST_CASE("002") {
  //! [002]
  auto str1 = string_view(R"!!(abc
def)!!");
  CHECK("abc\ndef" == str1);
  //! [002]
}

TEST_CASE("003") {
  //! [003]
  auto str1 = u16string_view(u"中文");
  CHECK(u"文" == (str1.substr(1)));
  CHECK(2 == (str1.size()));
  CHECK(6 == (string_view("中文").size()));
  //! [003]
}

TEST_CASE("004") {
  //! [004]
  auto str1 =
      u16string_view(u"中文") | tcb::utf_ranges::view::utf8 | to_<string>();
  CHECK(string_view("中文") == str1);
  //! [004]
}

TEST_CASE("005") {
  //! [005]
  auto parts = strings::split("hello world", " ") | to_vector;
  CHECK("hello" == parts[0]);
  CHECK("world" == parts[1]);
  //! [005]
}

TEST_CASE("006") {
  //! [006]
  // concat two
  auto str1 = string_view("hello");
  auto str2 = string_view(" world");
  CHECK("hello world" == (view::concat(str1, str2) | to_<string>()));

  // concat many
  auto str3 = string_view("!");
  auto str123 = vector<string_view>{str1, str2, str3};
  CHECK("hello world!" == (str123 | view::join | to_<string>()));

  // string itself is mutable
  // think every string is java StringBuilder
  auto mut_s = string();
  mut_s.reserve(str1.size() + str2.size() + str3.size());
  mut_s.append(str1);
  mut_s.append(str2);
  mut_s.append(str3);
  CHECK("hello world!" == mut_s);
  //! [006]
}

TEST_CASE("007") {
  //! [007]
  // positional
  using namespace fmt::literals;
  CHECK("hello world" == ("{} {}"_format("hello", "world")));
  // named
  CHECK("hello world" ==
        ("{v1} {v2}"_format("v1"_a = "hello", "v2"_a = "world")));
  // format
  CHECK("3.14" == ("{:.2f}"_format(3.14159)));
  //! [007]
}

TEST_CASE("008") {
  //! [008]
  CHECK("hello world" == (strings::lower("Hello World")));
  CHECK("HELLO WORLD" == (strings::upper("Hello World")));
  //! [008]
}

TEST_CASE("009") {
  //! [009]
  CHECK(strings::startswith("Hello World", "He"));
  CHECK(!strings::startswith("Hello World", "Hello World!"));
  //! [009]
}

TEST_CASE("010") {
  //! [010]
  CHECK(strings::endswith("Hello World", "ld"));
  CHECK(!strings::endswith("Hello World", "Hello World!"));
  //! [010]
}

TEST_CASE("011") {
  //! [011]
  CHECK(string_view("hello world") == strings::lstrip(" hello world"));
  CHECK(string_view("hello world") == strings::rstrip("hello world "));
  CHECK(string_view("hello world") == strings::strip("\thello world\t"));
  //! [011]
}

TEST_CASE("012") {
  //! [012]
  auto str = string_view("hello");
  CHECK(2 == str.find("l"));
  CHECK(string::npos == str.find("!"));
  CHECK(3 == str.rfind("l"));
  //! [012]
}

TEST_CASE("013") {
  //! [013]
  auto str = string_view("hello");
  CHECK("he__o" == strings::replace(str, "l", "_"));
  CHECK("he_lo" == strings::replace(str, "l", "_", 1));
  CHECK("he__o" == strings::replace(str, "l", "_", -1));
  //! [013]
}

TEST_CASE("014") {
  //! [014]
  auto match =
      regexs::search(R"!!((e\wl)O)!!", "HELLO", regex_constants::icase);
  CHECK(!match.empty());
  CHECK(string_view("ELLO") == regexs::get_group(match));
  CHECK(string_view("ELLO") == regexs::get_group(match, 0));
  CHECK(string_view("ELL") == regexs::get_group(match, 1));
  //! [014]
}

TEST_CASE("015") {
  //! [015]
  auto result = regexs::sub(
      R"!!([h|l]+)!!",
      [](cmatch const &m) { return strings::upper(regexs::get_group(m)); },
      "hello");
  CHECK("HeLLo" == result);
  //! [015]
}
