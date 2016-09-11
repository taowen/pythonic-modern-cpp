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


TEST_CASE("aaa") {
    //! [001]
    for(auto x : view::ints(0, 6)) {
        cout << x * x << endl;
    }
    //! [001]
}