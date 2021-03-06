#include "catch_ext.hpp"
#include "catch_with_main.hpp"
#include "fmt/all.hpp"
#include "folly/Expected.h"
#include "folly/FBString.h"
#include "pythonic/len.hpp"
#include "pythonic/utf8/Utf8Encoded.hpp"
#include "pythonic/utf8/capitalize.hpp"
#include "pythonic/utf8/center.hpp"
#include "pythonic/utf8/code_points.hpp"
#include "pythonic/utf8/code_units.hpp"
#include "pythonic/utf8/find.hpp"
#include "pythonic/utf8/finditer.hpp"
#include "pythonic/utf8/count.hpp"
#include "pythonic/utf8/endswith.hpp"
#include "pythonic/utf8/sub.hpp"
#include "pythonic/utf8/expandtabs.hpp"
#include "pythonic/utf8/index.hpp"
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
using namespace pyn::utf8::literals;


TEST_CASE("len") {
    CHECK(0 == pyn::len(""_u));
    CHECK(5 == pyn::len("hello"_u));
    CHECK(2 == pyn::len("中文"_u));
    CHECK(3 == pyn::len("中文\xe4"_u));
    CHECK(3 == pyn::len(std::vector<int> {1, 2, 3}));
}

TEST_CASE("capitalize") {
    CHECK("Hello"_u == utf8::capitalize("hello"_u));
    CHECK("中文"_u == utf8::capitalize("中文"_u));
}

TEST_CASE("center") {
    CHECK(" abc "_u == utf8::center("abc"_u, 5));
    CHECK("abc "_u == utf8::center("abc"_u, 4));
    CHECK("abc12 "_u == utf8::center("abc"_u, 6, "12"_u));
}

TEST_CASE("find") {
    CHECK(1 == utf8::find("abc"_u, "b"_u));
    CHECK(std::string::npos == utf8::find("abc"_u, "d"_u));
}

TEST_CASE("finditer") {
    CHECK((std::vector<size_t> {0, 3}) ==
          (utf8::finditer("abcab"_u, "ab"_u) | ranges::to_vector));
}

TEST_CASE("code_units") {
    auto chars = std::vector<char> {};
    for (auto c : utf8::code_units("abc"_u)) {
        chars.push_back(c);
    }
    CHECK((std::vector<char> {'a', 'b', 'c'}) == chars);
}

TEST_CASE("code_points") {
    auto chars = std::vector<utf8::TextView> {};
    for (auto c : utf8::code_points("中文"_u)) {
        chars.push_back(c);
    }
    CHECK((std::vector<utf8::TextView> {"中"_u, "文"_u}) == chars);
}

TEST_CASE("count") {
    CHECK(2 == utf8::count("aa"_u, "a"_u));
}

TEST_CASE("endswith") {
    CHECK(utf8::endswith("hello"_u, "lo"_u));
    CHECK(!utf8::endswith("hello"_u, "l"_u));
}

TEST_CASE("sub") {
    CHECK("he__o"_u == (utf8::sub("hello"_u, "l"_u, [](auto i){ return "_"_u; })));
}

TEST_CASE("expandtabs") {
    CHECK("  hello"_u == (utf8::expandtabs("\thello"_u, 2)));
    CHECK("1 hello"_u == (utf8::expandtabs("1\thello"_u, 2)));
}

TEST_CASE("index") {
    CHECK(1 == utf8::index("abc"_u, "b"_u).value());
    CHECK(!bool(utf8::index("abc"_u, "d"_u)));
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
