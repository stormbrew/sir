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

#include <iostream>
#include "sir/sir.hpp"
#include "sir/file.hpp"
#include "sir/debug.hpp"

using sir::reporter;
reporter so("test app", sir::timestamp | sir::linefeeds);

int main()
{
	using std::cin;
	using std::cout;
	using std::endl;
	
	so.bind(sir::standard(std::cout), sir::log);
	so.bind(sir::standard(std::cerr), sir::error | sir::fatal);
	so.bind(sir::file("test2.txt"), sir::error | sir::fatal);
	so.bind(sir::debugger(), sir::error | sir::fatal);

	so(sir::log) << "Hello";
	so(sir::fatal | sir::error) << "Goodbye, error " << 666;

	reporter::stream_type tmp = so(sir::log);
	tmp << "before ";
	
	so(sir::error) << "during";

	tmp << "after";

	cout << "Press Enter to finish" << endl;
	cin.sync();
	cin.get();

	return 0;
}
