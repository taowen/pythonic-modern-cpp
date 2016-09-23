#include <range/v3/all.hpp>
#include <regex>

using namespace ranges;
using namespace std;

namespace pythonic {
namespace regexs {
//        //! [search]
//        auto get_group(cmatch const& match, size_t n=0) {
//            auto a_sub_match = match[n];
//            return string_view(a_sub_match.first, a_sub_match.length());
//        }
//        auto search(regex const& pattern_re, string_view haystack,
//                    regex_constants::match_flag_type match_flag =
//                    regex_constants::match_default) {
//            auto match = cmatch();
//            regex_search(haystack.begin(), haystack.end(), match, pattern_re,
//            match_flag);
//            return match;
//        }
//        auto search(string_view pattern, string_view haystack,
//        regex::flag_type regex_flag = regex::ECMAScript,
//                    regex_constants::match_flag_type match_flag =
//                    regex_constants::match_default) {
//            regex pattern_re(pattern.data(), pattern.size(), regex_flag);
//            return search(pattern_re, haystack, match_flag);
//        }
//        //! [search]
//        //! [sub]
//        template <class UnaryFunction>
//        string sub(regex const &pattern_re, UnaryFunction f, string_view
//        haystack, size_t n=-1) {
//          string s;
//          size_t positionOfLastMatch = 0;
//          auto first = haystack.begin();
//          auto last = haystack.end();
//          auto endOfLastMatch = first;

//          auto callback = [&](cmatch const&match) {
//            auto positionOfThisMatch = match.position(0);
//            auto diff = positionOfThisMatch - positionOfLastMatch;

//            auto startOfThisMatch = endOfLastMatch;
//            std::advance(startOfThisMatch, diff);

//            s.append(endOfLastMatch, startOfThisMatch);
//            s.append(f(match));

//            auto lengthOfMatch = match.length(0);

//            positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

//            endOfLastMatch = startOfThisMatch;
//            std::advance(endOfLastMatch, lengthOfMatch);
//          };

//          cregex_iterator begin(first, last, pattern_re), end;
//          for (auto cur=begin; cur != end && n > 0; ++cur, --n) {
//            callback(*cur);
//          }

//          s.append(endOfLastMatch, last);

//          return s;
//        }

//        template <class UnaryFunction>
//        auto sub(string_view pattern, UnaryFunction f, string_view haystack,
//        size_t n=-1, regex::flag_type regex_flag = regex::ECMAScript) {
//          return sub(regex(pattern.data(), pattern.size(), regex_flag), f,
//          haystack, n);
//        }
//        //! [sub]
}
}
