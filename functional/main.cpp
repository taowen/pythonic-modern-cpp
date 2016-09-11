#include <map>
#include <unordered_map>

#include <catch_ext.hpp>
#include <catch_with_main.hpp>
#include <json.hpp>
#include <range/v3/all.hpp>

const int abc = std::__1::numeric_limits<int>::digits;

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
