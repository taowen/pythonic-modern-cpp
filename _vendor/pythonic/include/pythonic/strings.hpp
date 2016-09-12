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
        //! [lower]
        string lower(string_view input, locale const &l = std::locale()) {
          return input | view::transform([&l](auto c) { return std::tolower(c, l); }) |
                 to_<string>();
        }
        //! [lower]
        //! [upper]
        string upper(string_view input, locale const &l = std::locale()) {
          return input | view::transform([&l](auto c) { return std::toupper(c, l); }) |
                 to_<string>();
        }
        //! [upper]
        //! [startswith]
        bool startswith(string_view haystack, string_view needle) {
          return ranges::equal(haystack | view::slice(size_t(0), needle.size()), needle);
        }
        //! [startswith]
    }
}
