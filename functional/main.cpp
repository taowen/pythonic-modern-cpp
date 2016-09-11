#include <unordered_map>
#include <map>

#include <catch_with_main.hpp>
#include <catch_ext.hpp>
#include <range/v3/all.hpp>
#include <json.hpp>

const int abc = std::__1::numeric_limits<int>::digits;

using namespace std;
using namespace ranges;
using json = nlohmann::json;


TEST_CASE("001") {
    //! [001]
    for(auto x : view::ints(0, 6)) {
        cout << x * x << endl;
    }
    //! [001]
}

TEST_CASE("002") {
    //! [002]
    auto colors = vector<string>{"red", "green", "blue", "yellow"};
    for(auto const& color : colors) {
        cout << color << endl;
    }
    //! [002]
}

TEST_CASE("003") {
    //! [003]
    auto colors = vector<string>{"red", "green", "blue", "yellow"};
    CHECK((vector<string>{"yellow", "blue", "green", "red"})
          ==
          (colors | view::reverse | to_vector));
    //! [003]
}

TEST_CASE("004") {
    //! [004]
    auto names = vector<string>{"raymond", "rachel", "matthew"};
    auto colors = vector<string>{"red", "green", "blue", "yellow"};
    auto zipped = view::zip(names, colors);
    CHECK((vector<pair<string, string>>{
               {"raymond", "red"}, {"rachel", "green"}, {"matthew", "blue"}
           })
          ==
          (zipped | to_vector));
    for(const auto& [name, color] : zipped) {
        cout << name << " " << color << endl;
    }
    //! [004]
}