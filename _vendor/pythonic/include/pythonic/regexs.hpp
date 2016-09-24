#pragma once
#define PCRE2_CODE_UNIT_WIDTH 8
#include "folly/Expected.h"
#include "pcre2.h"
#include "pythonic/utf8.hpp"
#include <iostream>

using namespace std;

namespace pythonic {
namespace re {

class Pattern {
public:
  Pattern(pcre2_code *data) : __data(data) {}
  ~Pattern() {
    if (__data != nullptr) {
      pcre2_code_free(__data);
    }
  }

  // move constructor
  Pattern(Pattern &&that) {
    this->__data = that.__data;
    that.__data = nullptr;
  }

  pcre2_code *data() const { return __data; }

private:
  Pattern(Pattern const &that) = delete;        // disable copy constructor
  Pattern &operator=(Pattern const &) = delete; // disable copy assignment
  pcre2_code *__data;
};

struct RegexError {
  decltype(__FILE__) file;
  decltype(__LINE__) line;
  int error_number;
  PCRE2_SIZE error_offset;
};

folly::Expected<Pattern, RegexError>
compile(utf8::text_view pattern_str, uint32_t options = 0,
        pcre2_compile_context *ccontext = nullptr) {
  options |= PCRE2_UTF;
  RegexError err{__FILE__, __LINE__};
  auto pattern_obj =
      pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern_str.begin()),
                    pattern_str.code_units_count(), options, &err.error_number,
                    &err.error_offset, ccontext);
  if (pattern_obj == nullptr) {
    return folly::makeUnexpected(err);
  }
  return Pattern(pattern_obj);
}

class Match {
public:
  bool is_matched;
  Match(utf8::text_view subject, pcre2_match_data *data)
      : __subject(subject), __data(data) {}
  ~Match() {
    if (__data != nullptr) {
      pcre2_match_data_free(__data);
    }
  }

  // move constructor
  Match(Match &&that)
      : __subject(that.__subject), __data(that.__data),
        is_matched(that.is_matched) {
    that.__data = nullptr;
  }

  static folly::Expected<Match, int>
  create(utf8::text_view subject, Pattern const &pattern,
         pcre2_general_context *gcontext = nullptr) {
    auto match_obj = pcre2_match_data_create_from_pattern(pattern.data(), NULL);
    if (match_obj == nullptr) {
      return folly::makeUnexpected(-1);
    }
    return Match(subject, match_obj);
  }

  pcre2_match_data *data() { return __data; }

  utf8::text_view group() {
    if (!is_matched) {
      return utf8::text_view();
    }
    auto ovector = pcre2_get_ovector_pointer(__data);
    return utf8::text_view(__subject.begin() + ovector[0],
                           ovector[1] - ovector[0]);
  }

private:
  Match(Match const &that) = delete;        // disable copy constructor
  Match &operator=(Match const &) = delete; // disable copy assignment
  pcre2_match_data *__data;
  utf8::text_view __subject;
};

folly::Expected<Match, RegexError>
search(utf8::text_view pattern_str, utf8::text_view subject,
       uint32_t pattern_options = 0, uint32_t match_options = 0,
       pcre2_compile_context *ccontext = nullptr) {
  auto e_pattern = compile(pattern_str, pattern_options, ccontext);
  if (e_pattern.hasError()) {
    return folly::makeUnexpected(e_pattern.error());
  }
  auto pattern = std::move(e_pattern.value());
  auto e_match = Match::create(subject, pattern, nullptr);
  if (e_match.hasError()) {
    return folly::makeUnexpected(
        RegexError{__FILE__, __LINE__, e_match.error()});
  }
  auto match = std::move(e_match.value());
  auto result = pcre2_match(
      pattern.data(), reinterpret_cast<PCRE2_SPTR>(subject.begin()),
      subject.code_units_count(), 0, match_options, match.data(), NULL);
  if (result == PCRE2_ERROR_NOMATCH) {
    match.is_matched = false;
    return std::move(match);
  }
  if (result < 0) {
    return folly::makeUnexpected(RegexError{__FILE__, __LINE__, result});
  }
  match.is_matched = true;
  return std::move(match);
  //    auto pattern = compile(pattern_str
  //  auto pattern = reinterpret_cast<PCRE2_SPTR>("abc");
  //  int errornumber;
  //  PCRE2_SIZE erroroffset;
  //  auto re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber,
  //                          &erroroffset, NULL);
  //  if (re == NULL) {
  //    return;
  //  }
  //  auto r2 = compile("abc\\p{").value();
  //  auto match_data = pcre2_match_data_create_from_pattern(re, NULL);
  //  auto subject = reinterpret_cast<PCRE2_SPTR>("abcd");
  //  auto rc = pcre2_match(re,         /* the compiled pattern */
  //                        subject,    /* the subject string */
  //                        4,          /* the length of the subject */
  //                        0,          /* start at offset 0 in the subject */
  //                        0,          /* default options */
  //                        match_data, /* block for storing the result */
  //                        NULL);      /* use default match context */
  //  auto ovector = pcre2_get_ovector_pointer(match_data);
  //  for (auto i = 0; i < rc; i++) {
  //    auto substring_start =
  //        reinterpret_cast<char const *>(subject + ovector[2 * i]);
  //    auto substring_length = ovector[2 * i + 1] - ovector[2 * i];
  //    cout << utf8::text_view(substring_start, substring_length) << endl;
  //  }
  //  pcre2_match_data_free(match_data);
}
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
