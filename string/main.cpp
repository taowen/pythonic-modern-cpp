#include <catch_ext.hpp>
#include <catch_with_main.hpp>
#include <codecvt>
#include <experimental/string_view>
#include <fmt/all.hpp>
#include <locale>
#include <range/v3/all.hpp>
#include <regex>
#include <string>
#include <tcb/utf_ranges/view.hpp>

const int abc = std::__1::numeric_limits<int>::digits;

using namespace std;
using namespace ranges;

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
