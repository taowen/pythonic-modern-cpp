#include <catch_ext.hpp>
#include <catch_with_main.hpp>
#include <codecvt>
#include <experimental/string_view>
#include <fmt/all.hpp>
#include <locale>
#include <pythonic/strings.hpp>
#include <range/v3/all.hpp>
#include <regex>
#include <string>
#include <tcb/utf_ranges/view.hpp>

const int abc = std::__1::numeric_limits<int>::digits;

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
