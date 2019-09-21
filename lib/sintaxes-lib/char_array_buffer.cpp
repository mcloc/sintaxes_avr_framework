// Copyright 2007 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file BOOST_LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#include "char_array_buffer.hpp"

#include <functional>
#include <cassert>
#include <cstring>

char_array_buffer::char_array_buffer(const char *data, unsigned int len) :
		begin_(data), end_(data + len), current_(data) {
}

char_array_buffer::int_type char_array_buffer::underflow() {
	if (current_ == end_) {
		return traits_type::eof();
	}
	return traits_type::to_int_type(*current_);     // HERE!
}

char_array_buffer::int_type char_array_buffer::uflow() {
    if (current_ == end_) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*current_++);   // HERE!
}

char_array_buffer::int_type char_array_buffer::pbackfail(int_type ch) {
    if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1])) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*--current_);   // HERE!
}

int char_array_buffer::showmanyc() {
    return end_ - current_;
}
