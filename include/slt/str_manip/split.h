//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef STR_MANIP_LIB_SPLIT_H
#define STR_MANIP_LIB_SPLIT_H

#include <iterator>
#include <string_view>

namespace slt {

// Splits a string based on the passed delimiter.
// Be wary that since this is all string_view based, using this range, or any of
// its result past the lifetime of the source data is undefined behavior.
//
// The prefered way of using the string splitter is to simply use the split()
// function defined at the bottom of this file.
//
// Notes:
//  - Splitting the empty string returns an empty range.

template <typename CharT>
struct split_string {
  split_string(std::basic_string_view<CharT> src,
               std::basic_string_view<CharT> d)
      : source(src), delim(d) {}

  struct iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = std::basic_string_view<CharT>;
    using pointer = std::basic_string_view<CharT>*;
    using reference = std::basic_string_view<CharT>;
    using iterator_category = std::input_iterator_tag;

    iterator(value_type str, value_type delim)
        : remainder_(str), delim_(delim) {
      advance_();
    }

    iterator& operator++() {
      advance_();
      return *this;
    }

    iterator operator++(int) {
      iterator other = *this;
      advance_();
      return other;
    }

    iterator& operator=(iterator const&) = default;
    value_type operator*() const { return sub_str_; }
    value_type const* operator->() const { return &sub_str_; }

    bool operator==(iterator const& rhs) const {
      return sub_str_.data() == rhs.sub_str_.data() &&
             sub_str_.length() == rhs.sub_str_.length() &&
             remainder_.data() == rhs.remainder_.data() &&
             remainder_.length() == rhs.remainder_.length() && delim_ == delim_;
    }

    bool operator!=(iterator const& rhs) const { return !(*this == rhs); }

   private:
    value_type sub_str_;
    value_type remainder_;
    value_type delim_;

    void advance_() {
      auto delim_pos = remainder_.find(delim_);

      if (delim_pos == value_type::npos) {
        sub_str_ = remainder_;
        remainder_ = value_type();
      } else {
        sub_str_ = value_type(&*remainder_.begin(), delim_pos);
        remainder_.remove_prefix(delim_pos + delim_.length());
      }
    }
  };

  iterator begin() const {
    // special case: an empty string
    if (source.length() == 0) {
      return end();
    }
    return iterator(source, delim);
  }

  iterator end() const {
    return iterator(std::basic_string_view<CharT>(), delim);
  }

 private:
  std::basic_string_view<CharT> source;
  std::basic_string_view<CharT> delim;
};

// Returns a lazy string splitter.
template <typename CharT>
split_string<CharT> split(std::basic_string_view<CharT> str,
                          std::basic_string_view<CharT> delim) {
  return split_string<CharT>(str, delim);
}
}  // namespace slt
#endif
