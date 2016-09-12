#include <range/v3/all.hpp>

using namespace ranges;
using namespace std;

namespace pythonic {
    namespace strings {
        //! [split]
        auto split(string_view input, string_view delimeter) {
          auto to_string_view = [](auto const &r) {
            return string_view(&*r.begin(), ranges::distance(r));
          };
          return view::split(input, delimeter) | view::transform(to_string_view);
        }
        //! [split]
    }
}
