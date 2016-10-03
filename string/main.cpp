#include "catch_ext.hpp"
#include "catch_with_main.hpp"
#include "fmt/all.hpp"
#include "folly/Expected.h"
#include "folly/FBString.h"
#include "pythonic/len.hpp"
#include "pythonic/utf8/Utf8Encoded.hpp"
#include "pythonic/utf8/capitalize.hpp"
#include "pythonic/utf8/center.hpp"
#include "pythonic/utf8/find.hpp"
#include "pythonic/utf8/finditer.hpp"
#include <codecvt>
#include <iostream>
#include <limits>
#include <locale>
#include <string>

const auto a1 = std::numeric_limits<int>::digits;
const auto a2 = std::integral_constant<int, 1>::value;
namespace pyn = pythonic;
namespace utf8 = pyn::utf8;
namespace view = ranges::view;

TEST_CASE("len of empty") {
  CHECK(0 == pyn::len(U8("")));
  CHECK(0 == (U8("") | pyn::len));
}

TEST_CASE("len of english") {
  CHECK(5 == pyn::len(U8("hello")));
  CHECK(5 == (U8("hello") | pyn::len));
}

TEST_CASE("len of chinese") {
  CHECK(2 == pyn::len(U8("中文")));
  CHECK(2 == (U8("中文") | pyn::len));
}

TEST_CASE("len of incomplete codepoint") {
  CHECK(3 == pyn::len(U8("中文\xe4")));
  CHECK(3 == (U8("中文\xe4") | pyn::len));
}

TEST_CASE("len of vector") {
  CHECK(3 == pyn::len(std::vector<int>{1, 2, 3}));
  CHECK(3 == (std::vector<int>{1, 2, 3} | pyn::len));
}

TEST_CASE("capitalize") {
  CHECK(U8("Hello") == (utf8::capitalize(U8("hello")) | utf8::to_text));
  CHECK(U8("Hello") == (U8("hello") | utf8::capitalize | utf8::to_text));
}

TEST_CASE("center left 1 right 1") {
  CHECK(U8(" abc ") == (utf8::center(U8("abc"), 5) | utf8::to_text));
  CHECK(U8(" abc ") == (U8("abc") | utf8::center(5) | utf8::to_text));
}

TEST_CASE("center left 0 right 1") {
  CHECK(U8("abc ") == (utf8::center(U8("abc"), 4) | utf8::to_text));
  CHECK(U8("abc ") == (U8("abc") | utf8::center(4) | utf8::to_text));
}

TEST_CASE("center left 0 right 2 tail 1") {
  CHECK(U8("abc12 ") ==
        (U8("abc") | utf8::center(6, U8("12")) | utf8::to_text));
  CHECK(U8("abc12 ") == (utf8::center(U8("abc"), 6, U8("12")) | utf8::to_text));
}

TEST_CASE("find") {
  CHECK(1 == utf8::find(U8("abc"), U8("b")));
  CHECK(1 == (U8("abc") | utf8::find(U8("b"))));
}

TEST_CASE("finditer") {
  CHECK((std::vector<size_t>{0, 3}) ==
        (utf8::finditer(U8("abcab"), U8("ab")) | ranges::to_vector));
  CHECK((std::vector<size_t>{0, 3}) ==
        (U8("abcab") | utf8::finditer(U8("ab")) | ranges::to_vector));
}

// TEST_CASE("substr [0, 0)") { CHECK("" == ("hello"_v[{0, 0}])); }
// TEST_CASE("substr [0, 1)") { CHECK("h" == ("hello"_v[{0, 1}])); }
// TEST_CASE("substr [1, 1)") { CHECK("" == ("hello"_v[{1, 1}])); }
// TEST_CASE("substr [1, -1)") { CHECK("" == ("hello"_v[{1, -1}])); }
// TEST_CASE("substr tail") { CHECK("o" == ("hello"_v[{4, 5}])); }
// TEST_CASE("substr end beyond tail") { CHECK("o" == ("hello"_v[{4, 6}])); }
// TEST_CASE("substr begin beyond tail") { CHECK("" == ("hello"_v[{5, 6}])); }
// TEST_CASE("substr from end") { CHECK("ello" == ("hello"_v[{1,
// ranges::end}])); }
// TEST_CASE("substr from end - 1") {
//  CHECK("hell" == ("hello"_v[{0, ranges::end - 1}]));
//}
// TEST_CASE("substr from end - 1 to end") {
//  CHECK("o" == ("hello"_v[{ranges::end - 1, ranges::end}]));
//}
// TEST_CASE("substr from end - 2 to end - 1") {
//  CHECK("l" == ("hello"_v[{ranges::end - 2, ranges::end - 1}]));
//}

// TEST_CASE("001") {
//  //! [001]
//  // utf32 is the full representation, but not memory efficient
//  auto str1 = u32string_view(U"中文字符");
//  // substr works for utf32
//  CHECK(U"文字" == (str1.substr(1, 2)));
//  // size works for utf32
//  CHECK(4 == (str1.size()));

//  // utf8 is memory efficient but no built-in support for codepoint
//  auto str2 = string_view(u8"中文字符");
//  // size does not work for utf8,
//  // 2 codepoint, 6 code unit
//  CHECK(12 == (str2.size()));
//  // substr does not work for utf8
//  CHECK(u8"文字" != (str2.substr(1, 2)));

//  // use utf8 functions to work with utf8 string
//  auto str3 = u8"中文字符"_v;
//  //  CHECK(4 == utf8::len(str3));
//  CHECK(12 == (str3.code_units_count()));
//  CHECK(4 == (str3.code_points_count()));
//  CHECK(u8"文字" == (str3[{1, 3}]));

//  // utf8::Text is iterable as char32_t
//  auto count = 0;
//  for (char32_t c : str3) {
//    count += 1;
//  }
//  CHECK(4 == count);
//  //! [001]
//}

// TEST_CASE("002") {
//  //! [002]
//  auto str = u8R"!!(abc
// def)!!"_v;
//  CHECK(u8"abc\ndef" == str);
//  //! [002]
//}

// TEST_CASE("split empty with empty") {
//  auto parts = u8""_v.split(u8"");
//  CHECK(folly::fbvector<utf8::TextView>{u8""} == parts);
//}

// TEST_CASE("split none empty with empty") {
//  auto parts = u8"hello"_v.split(u8"");
//  CHECK(folly::fbvector<utf8::TextView>{u8"hello"} == parts);
//}

// TEST_CASE("split with match in begin") {
//  auto parts = u8"hello"_v.split(u8"h");
//  CHECK((folly::fbvector<utf8::TextView>{u8"", u8"ello"}) == parts);
//}

// TEST_CASE("split with match in end") {
//  auto parts = u8"hello"_v.split(u8"o");
//  CHECK((folly::fbvector<utf8::TextView>{u8"hell", u8""}) == parts);
//}

// TEST_CASE("split with multiple code units") {
//  auto parts = u8"hello"_v.split(u8"ll");
//  CHECK((folly::fbvector<utf8::TextView>{u8"he", u8"o"}) == parts);
//}

// TEST_CASE("split with multiple code units not found") {
//  auto parts = u8"hello"_v.split(u8"lli");
//  CHECK((folly::fbvector<utf8::TextView>{u8"hello"}) == parts);
//}

// TEST_CASE("003") {
//  //! [003]
//  auto parts = u8"中文字符"_v.split(u8"文字");
//  CHECK(2 == parts.size());
//  CHECK(u8"中" == parts[0]);
//  CHECK(u8"符" == parts[1]);
//  //! [003]
//}

// TEST_CASE("004") {
//  //! [004]
//  // concat two
//  auto str1 = u8"hello"_v;
//  auto str2 = u8" world"_v;
//  CHECK(u8"hello world" == ((str1 + str2).to_str()));

//  // concat many
//  auto str3 = u8"!"_v;
//  CHECK(u8"hello world!" == ((str1 + str2 + str3).to_str()));
//  CHECK(u8"hello world!" == ((str1 + (str2 + str3)).to_str()));
//  CHECK(u8"hello! world!" == (((str1 + str3) + (str2 + str3)).to_str()));

//  // allocate string intenrally
//  CHECK(u8"hello world" == ((str1 + str2).to_str()));
//  //! [004]
//}

// TEST_CASE("005") {
//  //! [005]
//  // positional
//  using namespace fmt::literals;
//  CHECK(u8"hello world" == (u8"{} {}"_format(u8"hello"_v, u8"world")));
//  // named
//  CHECK(u8"hello world" ==
//        ("{v1} {v2}"_format(u8"v1"_a = u8"hello", u8"v2"_a = u8"world")));
//  // format
//  CHECK(u8"3.14" == (u8"{:.2f}"_format(3.14159)));
//  //! [005]
//}

// TEST_CASE("006") {
//  //! [006]
//  CHECK(u8"hello world" == (u8"Hello World"_v.lower()));
//  CHECK(u8"HELLO WORLD" == (u8"Hello World"_v.upper()));
//  CHECK(u8"中文字符" == (u8"中文字符"_v.upper()));
//  //! [006]
//}

// TEST_CASE("007") {
//  //! [007]
//  CHECK(u8"Hello World"_v.startswith(u8"He"));
//  CHECK(!(u8"Hello World"_v.startswith(u8"Hello World!")));
//  //! [007]
//}

// TEST_CASE("008") {
//  //! [008]
//  CHECK(u8"Hello World"_v.endswith(u8"ld"));
//  CHECK(!(u8"Hello World"_v.endswith(u8"Hello World!")));
//  //! [008]
//}

// TEST_CASE("009") {
//  //! [009]
//  CHECK(u8"hello world" == (u8" hello world"_v.lstrip()));
//  CHECK(u8"hello world" == (u8"hello world "_v.rstrip()));
//  CHECK(u8"hello world" == (u8" hello world "_v.strip()));
//  //! [009]
//}

// TEST_CASE("010") {
//  //! [010]
//  CHECK(2 == (u8"hello"_v.find(u8"l")));
//  CHECK(utf8::npos == (u8"hello"_v.find(u8"!")));
//  CHECK(3 == (u8"hello"_v.rfind(u8"l")));
//  CHECK(2 == (u8"hello"_v.rfind(u8"ll")));
//  CHECK(0 == (u8"hello"_v.rfind(u8"h")));
//  //! [010]
//}

// TEST_CASE("011") {
//  //! [011]
//  CHECK(u8"he__o" == u8"hello"_v.replace(u8"l", u8"_"));
//  CHECK(u8"he_lo" == u8"hello"_v.replace(u8"l", u8"_", 1));
//  CHECK(u8"he__o" == u8"hello"_v.replace(u8"l", u8"_", -1));
//  //! [011]
//}

// TEST_CASE("012") {
//  // . will match utf8 codepoint
//  CHECK(u8"字符" == re::search(u8"字.", u8"中文字符").value().group());
//  // use UCP to enable \w matching codepoint, will slowdown the search
//  CHECK(u8"字符" ==
//        re::search(u8"字\\w", u8"中文字符", PCRE2_UCP).value().group());
//  // not matched
//  CHECK(!bool(re::search(u8"abc", u8"中文字符")));
//  // invlaid regex pattern
//  CHECK(!bool(re::search(u8"\\", u8"中文字符")));
//}

// TEST_CASE("013") {
//  auto match = re::search(u8"(.)(.)", u8"中文字符").value();
//  CHECK(u8"中文" == match.group());
//  CHECK(u8"中" == match.group(1));
//  CHECK(u8"文" == match.group(2));
//  CHECK(3 == match.groups_count());
//  CHECK(u8"" == match.group(3));
//  CHECK((folly::fbvector<utf8::TextView>{u8"中", u8"文"}) == match.groups());
//}

// TEST_CASE("014") {
//  auto match = re::search(u8"(?P<p1>.)(.)", u8"中文字符").value();
//  CHECK(u8"中" == match.group(1));
//  CHECK(u8"文" == match.group(2));
//  CHECK((re::GroupDict{{u8"p1", u8"中"}}) == match.groupdict());
//  auto str = utf8::SharedText(u8"hello");
//}

// auto test_match_retain_ownership() {
//  folly::fbstring str;
//  str.append(u8"中文");
//  str.append(u8"字符");
//  auto match = re::search(u8"(.)(.)", str).value();
//  return match;
//}

// TEST_CASE("match retain ownership") {
//  auto match = test_match_retain_ownership();
//  CHECK(u8"中文" == match.group(0));
//  CHECK(u8"中" == match.group(1));
//}

// TEST_CASE("015") {
//  auto matches = re::finditer(u8"中", u8"中文中").value();
//  auto matched_groups =
//      matches |
//      view::transform([](auto const &match) { return match.group(); }) |
//      ranges::to_vector;
//  CHECK((vector<utf8::TextView>{u8"中", u8"中"}) == matched_groups);
//}

// TEST_CASE("014") {
//  //! [014]
//  auto match =
//      regexs::search(R"!!((e\wl)O)!!", "HELLO", regex_constants::icase);
//  CHECK(!match.empty());
//  CHECK(string_view("ELLO") == regexs::get_group(match));
//  CHECK(string_view("ELLO") == regexs::get_group(match, 0));
//  CHECK(string_view("ELL") == regexs::get_group(match, 1));
//  //! [014]
//}

// TEST_CASE("015") {
//  //! [015]
//  auto result = regexs::sub(
//      R"!!([h|l]+)!!",
//      [](cmatch const &m) { return strings::upper(regexs::get_group(m)); },
//      "hello");
//  CHECK("HeLLo" == result);
//  //! [015]
//}
