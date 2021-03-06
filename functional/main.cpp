#include <map>
#include <unordered_map>

#include <catch_ext.hpp>
#include <catch_with_main.hpp>
#include <json.hpp>
#include <range/v3/all.hpp>

const int abc = std::numeric_limits<int>::digits;

using namespace std;
using namespace ranges;
using json = nlohmann::json;

TEST_CASE("001") {
  //! [001]
  for (auto x : view::ints(0, 6)) {
    cout << x * x << endl;
  }
  //! [001]
}

TEST_CASE("002") {
  //! [002]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  for (auto const &color : colors) {
    cout << color << endl;
  }
  //! [002]
}

TEST_CASE("003") {
  //! [003]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  CHECK((vector<string>{"yellow", "blue", "green", "red"}) ==
        (colors | view::reverse | to_vector));
  //! [003]
}

TEST_CASE("004") {
  //! [004]
  auto names = vector<string>{"raymond", "rachel", "matthew"};
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto zipped = view::zip(names, colors);
  CHECK((vector<pair<string, string>>{
            {"raymond", "red"}, {"rachel", "green"}, {"matthew", "blue"}}) ==
        (zipped | to_vector));
  for (auto const & [ name, color ] : zipped) {
    cout << name << " " << color << endl;
  }
  //! [004]
}

TEST_CASE("005") {
  //! [005]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  for (auto const & [ i, color ] : view::zip(view::iota(0), colors)) {
    cout << i << " " << color << endl; 
  }
  //! [005]
}

TEST_CASE("006") {
  //! [006]
  auto d = unordered_map<string, string>{
      {"matthew", "blue"}, {"rachel", "green"}, {"raymond", "red"}};
  for (auto const & [ k, v ] : d) {
    cout << k << " " << v << endl;
  }
  //! [006]
}

TEST_CASE("007") {
  //! [007]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto sorted = action::sort(colors);
  CHECK((vector<string>{"blue", "green", "red", "yellow"}) ==
        (sorted | to_vector));
  //! [007]
}

TEST_CASE("008") {
  //! [008]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto sorted = action::sort(colors, greater<string>());
  CHECK((vector<string>{"yellow", "red", "green", "blue"}) ==
        (sorted | to_vector));
  //! [008]
}

TEST_CASE("009") {
  //! [009]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto sorted =
      action::sort(colors, less<int>(), [](auto const &e) { return e.size(); });
  CHECK((vector<string>{"red", "blue", "green", "yellow"}) ==
        (sorted | to_vector));
  //! [009]
}

TEST_CASE("010") {
  //! [010]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto actual = vector<int>{
      colors | view::transform([](auto const &e) { return e.size(); })};
  CHECK((vector<int>{3, 5, 4, 6}) == actual);
  //! [010]
}

TEST_CASE("011") {
  //! [011]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  CHECK(ranges::any_of(colors, [](const auto &e) { return e == "green"; }));
  //! [011]
}

TEST_CASE("012") {
  //! [012]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto colors_view = colors | view::all;
  CHECK((vector<string>{"green"}) == (colors_view[{1, 2}] | to_vector));
  CHECK((vector<string>{"red", "green"}) == (colors_view[{0, 2}] | to_vector));
  CHECK((vector<string>{"green", "blue", "yellow"}) ==
        (colors_view[{1, ranges::end}] | to_vector));
  CHECK((vector<string>{"red", "green", "blue"}) ==
        (colors_view[{0, ranges::end - 1}] | to_vector));
  //! [012]
}

TEST_CASE("013") {
  //! [013]
  auto d = unordered_map<string, string>{
      {"matthew", "blue"}, {"rachel", "green"}, {"raymond", "red"}};
  // 取出 keys，此处不是lazy操作
  auto keys = vector<string>{d | view::keys};
  for (const auto &k : keys) {
    if (k.find("r") == 0) {
      // 因为 keys 不是lazy操作，此处的删除不会影响遍历
      d.erase(k);
    }
  }
  CHECK((unordered_map<string, string>{{"matthew", "blue"}}) == d);
  //! [013]
}

TEST_CASE("014") {
  //! [014]
  auto colors = vector<string>{"red", "green", "blue", "yellow"};
  auto d =
      unordered_map<string, int>{colors | view::transform([](const auto &e) {
                                   return make_pair(e, e.size());
                                 })};
  CHECK((unordered_map<string, int>{
            {"red", 3}, {"green", 5}, {"yellow", 6}, {"blue", 4}}) == d);
  //! [014]
}
