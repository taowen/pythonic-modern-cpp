#include <catch_ext.hpp>
#include <catch_with_main.hpp>
#include <codecvt>
#include <experimental/string_view>
#include <fmt/all.hpp>
#include <locale>
#include <range/v3/all.hpp>
#include <regex>
#include <string>

const int abc = std::__1::numeric_limits<int>::digits;

using namespace std;
using namespace ranges;

TEST_CASE("001") {
  //! [001]
  auto str1 = string_view("hello world");
  CHECK("o w" == (str1.substr(4, 3)));
  //! [001]
}
