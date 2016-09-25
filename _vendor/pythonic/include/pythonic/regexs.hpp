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

class RegexObjectNameRange : public ranges::view_facade<RegexObjectNameRange> {

  friend ranges::range_access;

  uint32_t name_count;
  PCRE2_SPTR name_table;
  uint32_t name_entry_size;

  struct cursor {
  private:
    RegexObjectNameRange *rng_;

  public:
    cursor() = default;
    explicit cursor(RegexObjectNameRange &rng) : rng_(&rng) {}
    void next() { rng_->next(); }
    std::pair<int, utf8::TextView> get() const {
      int number = (rng_->name_table[0] << 8) | rng_->name_table[1];
      auto name =
          utf8::TextView(reinterpret_cast<char const *>(rng_->name_table + 2));
      return {number, name};
    }
    bool done() const { return rng_->name_count == 0; }
  };
  cursor begin_cursor() { return cursor{*this}; }
  void next() {
    name_count--;
    name_table += name_entry_size;
  }

public:
  RegexObjectNameRange(pcre2_code *pattern) {
    pcre2_pattern_info(pattern, PCRE2_INFO_NAMECOUNT, &name_count);
    pcre2_pattern_info(pattern, PCRE2_INFO_NAMETABLE, &name_table);
    pcre2_pattern_info(pattern, PCRE2_INFO_NAMEENTRYSIZE, &name_entry_size);
  }
};

class RegexObject {
  pcre2_code *__data;

public:
  RegexObject(pcre2_code *data) : __data(data) {}
  ~RegexObject() {
    if (__data != nullptr) {
      pcre2_code_free(__data);
    }
  }

  // move constructor
  RegexObject(RegexObject &&that) {
    this->__data = that.__data;
    that.__data = nullptr;
  }

  pcre2_code *data() const { return __data; }

  RegexObjectNameRange names() const {
    uint32_t name_count;
    PCRE2_SPTR name_table;
    uint32_t name_entry_size;
    return RegexObjectNameRange(__data);
  }

private:
  RegexObject(RegexObject const &that) = delete; // disable copy constructor
  RegexObject &
  operator=(RegexObject const &) = delete; // disable copy assignment
};

struct RegexError {
  decltype(__FILE__) file;
  decltype(__LINE__) line;
  int error_number;
  PCRE2_SIZE error_offset;
};

folly::Expected<RegexObject, RegexError>
compile(utf8::TextView pattern_str, uint32_t options = 0,
        pcre2_compile_context *ccontext = nullptr) {
  options |= PCRE2_UTF;
  RegexError err{__FILE__, __LINE__};
  auto pattern_obj =
      pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern_str.c_str()),
                    pattern_str.code_units_count(), options, &err.error_number,
                    &err.error_offset, ccontext);
  if (pattern_obj == nullptr) {
    return folly::makeUnexpected(err);
  }
  return RegexObject(pattern_obj);
}

using GroupDict = unordered_map<utf8::TextView, utf8::TextView>;

class MatchObject {
  pcre2_match_data *__data;
  utf8::TextHolder __subject;
  shared_ptr<RegexObject> __pattern;

public:
  MatchObject(utf8::TextView subject, shared_ptr<RegexObject> pattern,
              pcre2_match_data *data)
      : __subject(subject.retain()), __pattern(pattern), __data(data) {}
  ~MatchObject() {
    if (__data != nullptr) {
      pcre2_match_data_free(__data);
    }
  }

  // move constructor
  MatchObject(MatchObject &&that)
      : __subject(std::move(that.__subject)), __pattern(std::move(that.__pattern)), __data(that.__data) {
    that.__data = nullptr;
  }

  static folly::Expected<MatchObject, int>
  create(utf8::TextView subject, shared_ptr<RegexObject> pattern,
         pcre2_general_context *gcontext = nullptr) {
    auto match_obj =
        pcre2_match_data_create_from_pattern(pattern->data(), NULL);
    if (match_obj == nullptr) {
      return folly::makeUnexpected(-1);
    }
    return MatchObject(subject, pattern, match_obj);
  }

  pcre2_match_data *data() { return __data; }

  utf8::TextView group() { return group(0); }
  utf8::TextView group(int i) {
    if (i >= groups_count()) {
      return utf8::TextView();
    }
    auto ovector = pcre2_get_ovector_pointer(__data);
    return __subject._.code_units(ovector[i * 2],
                                  ovector[i * 2 + 1] - ovector[i * 2]);
  }

  folly::fbvector<utf8::TextView> groups() {
    folly::fbvector<utf8::TextView> v;
    for (auto i = 1; i < groups_count(); i++) {
      v.push_back(group(i));
    }
    return std::move(v);
  }

  GroupDict groupdict() {
    auto dict = GroupDict{};
    for (auto const &p : __pattern->names()) {
      auto number = p.first;
      auto name = p.second;
      dict[name] = group(number);
    }
    return dict;
  }

  int groups_count() { return pcre2_get_ovector_count(__data); }

private:
  MatchObject(MatchObject const &that) = delete; // disable copy constructor
  MatchObject &
  operator=(MatchObject const &) = delete; // disable copy assignment
};

folly::Expected<MatchObject, RegexError>
search(utf8::TextView pattern_str, utf8::TextView subject,
       uint32_t pattern_options = 0, uint32_t match_options = 0,
       pcre2_compile_context *ccontext = nullptr) {
  auto e_pattern = compile(pattern_str, pattern_options, ccontext);
  if (e_pattern.hasError()) {
    return folly::makeUnexpected(e_pattern.error());
  }
  auto pattern = make_shared<RegexObject>(std::move(e_pattern.value()));
  auto e_match = MatchObject::create(subject, pattern, nullptr);
  if (e_match.hasError()) {
    return folly::makeUnexpected(
        RegexError{__FILE__, __LINE__, e_match.error()});
  }
  auto match = std::move(e_match.value());
  auto result = pcre2_match(
      pattern->data(), reinterpret_cast<PCRE2_SPTR>(subject.code_units_begin()),
      subject.code_units_count(), 0, match_options, match.data(), NULL);
  if (result < 0) {
    return folly::makeUnexpected(RegexError{__FILE__, __LINE__, result});
  }
  return std::move(match);
}
}
}
