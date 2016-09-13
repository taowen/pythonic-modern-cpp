#include <range/v3/all.hpp>
#include <regex>

using namespace ranges;
using namespace std;

namespace pythonic {
    namespace regexs {
        //! [search]
        class string_view_cmatch: public cmatch {
        public:
            string_view group(size_type n=0) {
                auto a_sub_match = (*this)[n];
                return string_view(a_sub_match.first, a_sub_match.length());
            }
        };
        auto search(regex const& pattern_re, string_view haystack,
                    regex_constants::match_flag_type match_flag = regex_constants::match_default) {
            auto match = string_view_cmatch();
            regex_search(haystack.begin(), haystack.end(), match, pattern_re, match_flag);
            return match;
        }
        auto search(string_view pattern, string_view haystack, regex::flag_type regex_flag = regex::ECMAScript,
                    regex_constants::match_flag_type match_flag = regex_constants::match_default) {
            regex pattern_re(pattern.data(), pattern.size(), regex_flag);
            return search(pattern_re, haystack, match_flag);
        }
        //! [search]
    }
}

