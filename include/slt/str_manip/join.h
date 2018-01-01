//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef STR_MANIP_LIB_JOIN_H
#define STR_MANIP_LIB_JOIN_H

#include <cassert>
#include <iterator>
#include <string_view>
#include <variant>

namespace slt {

// Joins a collection of string-like objects, inserting delim
// in between each joined strings
template <typename IteT, typename CharT>
struct joined_string {
  using delim_type = std::basic_string_view<CharT>;

  joined_string(IteT b, IteT e, delim_type d) : begin_(b), end_(e), delim_(d) {}

  struct iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = CharT;
    using pointer = CharT*;
    using reference = CharT;
    using iterator_category = std::input_iterator_tag;

    iterator(IteT b, IteT e, delim_type d)
        : current_payload_(b), payload_end_(e), delim_(d) {
      if (b != e) {
        next_char_ite_.emplace<0>(b->begin());
        // In case the first string is empty
        sanitize_();
      }
    }

    reference operator*() {
      return std::visit([](auto&& arg) { return (CharT)*arg; }, next_char_ite_);
    }

    bool operator!=(iterator const& rhs) const { return !(*this == rhs); }

    bool operator==(iterator const& rhs) const {
      return rhs.delim_.data() == delim_.data() &&
             rhs.delim_.length() == delim_.length() &&
             rhs.current_payload_ == current_payload_ &&
             rhs.payload_end_ == payload_end_ &&
             rhs.next_char_ite_ == next_char_ite_;
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

   private:
    // The role of this is to properly handle the "end of payload"
    // scenario, which has a lot of edge cases.
    void sanitize_() {
      assert(next_char_ite_.index() == 0);

      auto& p_ite = std::get<0>(next_char_ite_);
      if (p_ite == current_payload_->end()) {
        // We have reached the end of the current payload
        ++current_payload_;

        if (delim_.empty()) {
          // Skip any empty payload.
          while (current_payload_ != payload_end_ &&
                 current_payload_->begin() == current_payload_->end()) {
            ++current_payload_;
          }

          if (current_payload_ != payload_end_) {
            next_char_ite_.emplace<0>(current_payload_->begin());
          } else {
            next_char_ite_ = next_char_ite_t();
          }
        } else {
          // Don't put a delimiter after the last payload.
          if (current_payload_ != payload_end_) {
            next_char_ite_.emplace<1>(delim_.begin());
          } else {
            next_char_ite_ = next_char_ite_t();
          }
        }
      }
    }

    void advance_() {
      if (next_char_ite_.index() == 0) {
        // we are currently handling a payload
        auto& p_ite = std::get<0>(next_char_ite_);
        ++p_ite;
        sanitize_();
      } else {
        // we are currently handling a delimiter
        auto& d_ite = std::get<1>(next_char_ite_);
        ++d_ite;
        if (d_ite == delim_.end()) {
          // if we are here, then there should be at least one
          // payload left
          next_char_ite_.emplace<0>(current_payload_->begin());
          sanitize_();
        }
      }
    }

    // Beware: payload_ite_t and delim_ite_t could end up being the same type.
    using payload_ite_t = typename IteT::value_type::iterator;
    using delim_ite_t = typename delim_type::iterator;
    using next_char_ite_t = std::variant<payload_ite_t, delim_ite_t>;

    // Unless we are at the final end of the joined range, this always points
    // directly to the next character we are about to return.
    next_char_ite_t next_char_ite_;

    IteT current_payload_;
    IteT payload_end_;
    delim_type delim_;
  };

  iterator begin() const { return iterator(begin_, end_, delim_); }

  iterator end() const { return iterator(end_, end_, delim_); }

 private:
  IteT begin_;
  IteT end_;
  delim_type delim_;
};

// Returns a lazy string joiner that binds a range of strings with a delimiter
template <typename IteT, typename CharT>
joined_string<IteT, CharT> join(IteT begin, IteT end,
                                std::basic_string_view<CharT> delim) {
  return joined_string<IteT, CharT>(begin, end, delim);
}
}  // namespace slt
#endif
