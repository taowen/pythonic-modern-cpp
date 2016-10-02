#pragma once
#define PCRE2_CODE_UNIT_WIDTH 8
#include "folly/Expected.h"
#include "folly/FBVector.h"
#include "pcre2.h"
#include "pythonic/utf8.hpp"
#include "range/v3/all.hpp"
#include <iostream>
#include <unordered_map>
#include <utility>

using namespace std;

namespace pythonic {
namespace re {

// class RegexObjectNameRange : public ranges::view_facade<RegexObjectNameRange>
// {

//  friend ranges::range_access;

//  uint32_t name_count;
//  PCRE2_SPTR name_table;
//  uint32_t name_entry_size;

//  struct cursor {
//  private:
//    RegexObjectNameRange *rng_;

//  public:
//    cursor() = default;
//    explicit cursor(RegexObjectNameRange &rng) : rng_(&rng) {}
//    void next() { rng_->next(); }
//    std::pair<int, utf8::TextView> get() const {
//      int number = (rng_->name_table[0] << 8) | rng_->name_table[1];
//      auto name =
//          utf8::TextView(reinterpret_cast<char const *>(rng_->name_table +
//          2));
//      return {number, name};
//    }
//    bool done() const { return rng_->name_count == 0; }
//  };
//  cursor begin_cursor() { return cursor{*this}; }
//  void next() {
//    name_count--;
//    name_table += name_entry_size;
//  }

// public:
//  RegexObjectNameRange(pcre2_code *pattern) {
//    pcre2_pattern_info(pattern, PCRE2_INFO_NAMECOUNT, &name_count);
//    pcre2_pattern_info(pattern, PCRE2_INFO_NAMETABLE, &name_table);
//    pcre2_pattern_info(pattern, PCRE2_INFO_NAMEENTRYSIZE, &name_entry_size);
//  }
//};

// class RegexObject {
//  shared_ptr<pcre2_code> __data;

// public:
//  RegexObject(pcre2_code *data)
//      : __data(shared_ptr<pcre2_code>(
//            data, [](pcre2_code *re) { pcre2_code_free(re); })) {}

//  // move constructor
//  RegexObject(RegexObject &&that) : __data(std::move(that.__data)) {}
//  // copy constructor
//  RegexObject(RegexObject const &that) = default;

//  pcre2_code *data() const { return __data.get(); }

//  RegexObjectNameRange names() const {
//    uint32_t name_count;
//    PCRE2_SPTR name_table;
//    uint32_t name_entry_size;
//    return RegexObjectNameRange(data());
//  }
//};

// struct RegexError {
//  decltype(__FILE__) file;
//  decltype(__LINE__) line;
//  int error_number;
//  PCRE2_SIZE error_offset;
//};

// folly::Expected<RegexObject, RegexError>
// compile(utf8::TextView pattern_str, uint32_t options = 0,
//        pcre2_compile_context *ccontext = nullptr) {
//  options |= PCRE2_UTF;
//  RegexError err{__FILE__, __LINE__};
//  auto pattern_obj =
//      pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern_str.c_str()),
//                    pattern_str.code_units_count(), options,
//                    &err.error_number,
//                    &err.error_offset, ccontext);
//  if (pattern_obj == nullptr) {
//    return folly::makeUnexpected(err);
//  }
//  return RegexObject(pattern_obj);
//}

// using GroupDict = unordered_map<utf8::TextView, utf8::TextView>;

// class MatchObject {
//  shared_ptr<pcre2_match_data> __data;
//  utf8::SharedText __subject;
//  RegexObject __pattern;

// public:
//  MatchObject(utf8::SharedText subject, RegexObject pattern,
//              pcre2_match_data *data)
//      : __subject(std::move(subject)), __pattern(std::move(pattern)),
//        __data(shared_ptr<pcre2_match_data>(
//            data, [](pcre2_match_data *ptr) { pcre2_match_data_free(ptr); }))
//            {
//    auto ovector = pcre2_get_ovector_pointer(data);
//    ovector[0] = -1;
//    ovector[1] = 0;
//  }

//  // move constructor
//  MatchObject(MatchObject &&that)
//      : __subject(std::move(that.__subject)),
//        __pattern(std::move(that.__pattern)), __data(std::move(that.__data))
//        {}

//  // copy constructor
//  MatchObject(MatchObject const &that)
//      : __subject(that.__subject), __pattern(that.__pattern),
//        __data(that.__data) {}

//  static folly::Expected<MatchObject, int>
//  create(utf8::SharedText subject, RegexObject pattern,
//         pcre2_general_context *gcontext = nullptr) {
//    auto match_obj = pcre2_match_data_create_from_pattern(pattern.data(),
//    NULL);
//    if (match_obj == nullptr) {
//      return folly::makeUnexpected(-1);
//    }
//    return MatchObject(subject, pattern, match_obj);
//  }

//  pcre2_match_data *data() const { return __data.get(); }

//  utf8::TextView group() const { return group(0); }
//  utf8::TextView group(int i) const {
//    if (i >= groups_count()) {
//      return utf8::TextView();
//    }
//    auto ovector = pcre2_get_ovector_pointer(data());
//    return utf8::TextView(__subject.code_units_begin() + ovector[i * 2],
//                          ovector[i * 2 + 1] - ovector[i * 2]);
//  }

//  folly::fbvector<utf8::TextView> groups() const {
//    folly::fbvector<utf8::TextView> v;
//    for (auto i = 1; i < groups_count(); i++) {
//      v.push_back(group(i));
//    }
//    return std::move(v);
//  }

//  GroupDict groupdict() const {
//    auto dict = GroupDict{};
//    for (auto const &p : __pattern.names()) {
//      auto number = p.first;
//      auto name = p.second;
//      dict[name] = group(number);
//    }
//    return dict;
//  }

//  int groups_count() const { return pcre2_get_ovector_count(data()); }

//  utf8::TextView subject() {
//    return utf8::TextView(__subject.code_units_begin(),
//                          __subject.code_units_count());
//  }

//  size_t last_position() {
//    auto ovector = pcre2_get_ovector_pointer(data());
//    return ovector[1];
//  }

//  size_t last_size() {
//    auto ovector = pcre2_get_ovector_pointer(data());
//    return ovector[1] - ovector[0];
//  }
//};

// class RegexSearchRange : public ranges::view_facade<RegexSearchRange> {

//  friend ranges::range_access;

//  RegexObject __pattern;
//  MatchObject __match;
//  uint32_t __match_options;

//  struct cursor {
//  private:
//    RegexSearchRange *__rng;

//  public:
//    cursor() = default;
//    explicit cursor(RegexSearchRange &rng) : __rng(&rng) {}
//    void next() { __rng->next(); }
//    MatchObject &get() const { return __rng->__match; }
//    bool done() const { return __rng->__match.last_size() == 0; }
//  };
//  cursor begin_cursor() { return cursor{*this}; }
//  void next() {
//    auto start_offset = __match.last_position();
//    auto subject = __match.subject();
//    for (;;) {
//      auto result =
//          pcre2_match(__pattern.data(),
//                      reinterpret_cast<PCRE2_SPTR>(subject.code_units_begin()),
//                      subject.code_units_count(), start_offset,
//                      __match_options,
//                      __match.data(), NULL);
//      if (result < 0) {
//        // TODO: handle CRLF case mentioned in PCRE2 demo
//        if (start_offset < subject.code_units_count()) {
//          start_offset +=
//              utf8::decode_utf8(subject.code_units_begin() + start_offset,
//                                subject.code_units_count() - start_offset);
//        } else {
//          break;
//        }
//      } else {
//        break;
//      }
//    }
//  }

// public:
//  RegexSearchRange(RegexObject pattern, MatchObject match,
//                   uint32_t match_options)
//      : __pattern(std::move(pattern)), __match(std::move(match)),
//        __match_options(match_options) {
//    next();
//  }
//};

// folly::Expected<RegexSearchRange, RegexError>
// finditer(utf8::TextView pattern_str, utf8::SharedText subject,
//         uint32_t pattern_options = 0, uint32_t match_options = 0,
//         pcre2_compile_context *ccontext = nullptr) {
//  auto e_pattern = compile(pattern_str, pattern_options, ccontext);
//  if (e_pattern.hasError()) {
//    return folly::makeUnexpected(e_pattern.error());
//  }
//  auto pattern = std::move(e_pattern.value());
//  auto e_match = MatchObject::create(subject, pattern, nullptr);
//  if (e_match.hasError()) {
//    return folly::makeUnexpected(
//        RegexError{__FILE__, __LINE__, e_match.error()});
//  }
//  return RegexSearchRange(std::move(pattern), std::move(e_match.value()),
//                          match_options);
//}

// folly::Expected<MatchObject, RegexError>
// search(utf8::TextView pattern_str, utf8::SharedText subject,
//       uint32_t pattern_options = 0, uint32_t match_options = 0,
//       pcre2_compile_context *ccontext = nullptr) {
//  auto matches =
//      finditer(pattern_str, subject, pattern_options, match_options,
//      ccontext);
//  if (matches) {
//    auto match = matches.value().front();
//    if (match.last_size() == 0) {
//      return folly::makeUnexpected(RegexError{__FILE__, __LINE__, 1});
//    }
//    return match;
//  } else {
//    return folly::makeUnexpected(matches.error());
//  }
//}
}
}
