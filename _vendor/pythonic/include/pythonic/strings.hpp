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
        auto lower(string_view input, locale const &l = std::locale()) {
          return input | view::transform([&l](auto c) { return std::tolower(c, l); }) |
                 to_<string>();
        }
        //! [lower]
        //! [upper]
        auto upper(string_view input, locale const &l = std::locale()) {
          return input | view::transform([&l](auto c) { return std::toupper(c, l); }) |
                 to_<string>();
        }
        //! [upper]
        //! [startswith]
        auto startswith(string_view haystack, string_view needle) {
          return ranges::equal(haystack | view::slice(size_t(0), needle.size()), needle);
        }
        //! [startswith]
        //! [endswith]
        auto endswith(string_view haystack, string_view needle) {
          return ranges::equal(haystack | view::slice(ranges::end - needle.size(), ranges::end), needle);
        }
        //! [endswith]
        //! [lstrip]
        auto lstrip(string_view input, locale const &l = std::locale()) {
          auto begin = input.begin();
          auto left_iterator = ranges::find_if(
              begin, input.end(), [&l](auto const &c) { return !std::isspace(c, l); });
          return input.substr(left_iterator - begin);
        }
        //! [lstrip]
        //! [rstrip]
        auto rstrip(string_view input, locale const &l = std::locale()) {
          auto rend = input.rend();
          auto right_iterator =
              ranges::find_if(input.rbegin(), rend,
                              [&l](auto const &c) { return !std::isspace(c, l); });
          return input.substr(0, rend - right_iterator);
        }
        //! [rstrip]
        //! [strip]
        auto strip(string_view input, locale l = std::locale()) {
          return rstrip(lstrip(input, l), l);
        }
        //! [strip]
        //! [replace]
        auto replace(string_view haystack, string_view needle, string_view replacement,
                         size_t count = numeric_limits<size_t>::max()) {
          auto needle_size = needle.size();
          auto replaced = string();
          replaced.reserve(haystack.size());
          auto pos = 0;
          while (count > 0) {
            auto next = haystack.find(needle, pos);
            if (next == string::npos) {
              break;
            } else {
              replaced.append(haystack.substr(pos, next - pos));
              replaced.append(replacement);
              pos = next + needle_size;
              count -= 1;
            }
          }
          replaced.append(haystack.substr(pos));
          return replaced;
        }
        //! [replace]
    }
}
