/*
 * Copyright (c) 2003 Megan Batty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "sir/sir.hpp"
#include <limits>

const sir::log_type 
      sir::debug = 0x1, 
      sir::error = 0x2, 
      sir::warning = 0x4, 
      sir::fatal = 0x8, 
      sir::log = 0x10, 
      sir::screen = 0x20,
      sir::log_all = std::numeric_limits<sir::log_type>::max();

const sir::option_type 
      sir::timestamp = 0x1,
      sir::linefeeds = 0x2,
      sir::modulename = 0x4,
      sir::option_all = std::numeric_limits<sir::option_type>::max();



