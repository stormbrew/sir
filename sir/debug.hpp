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

#ifndef _sir_debug_hpp_
#define _sir_debug_hpp_

#ifdef _WIN32
# include <windows.h>

namespace sir
{
	// The default template for debugbuf does not exist. It is specialized below for the
	// widechar and multibyte versions. Anything other than char or wchar_t should produce
	// an error.
	template <typename tChar>
	class debugbuf; 

	// ANSI/MultiByte version of debugbuf, calls OutputDebugStringA.
	template <>
	class debugbuf<char> : public basic_reportbase<char>
	{
		void writeline(const std::string &str, bool endl)
		{
			OutputDebugStringA(str.c_str());
			if (endl)
				OutputDebugStringA("\n");
		}
	};

	// WideChar version of debugbuf. Calls OutputDebugStringW.
	// This should not end up causing runtime errors in win9x if not used,
	// as the compiler should not instantiate the unused template to that degreee.
	template <>
	class debugbuf<wchar_t> : public basic_reportbase<wchar_t>
	{
		void writeline(const std::wstring &str, bool endl)
		{
			OutputDebugStringW(str.c_str());
			if (endl)
				OutputDebugStringW(L"\n");
		}
	};
#else
// If we're not in windows, we have no debug output.
namespace sir
{
	// so debugbuf does nothing.
	template 
	<
		typename tChar, 
		typename tTraits = std::char_traits<tChar>, 
		typename tAlloc = std::allocator<tChar> 
	>
	class debugbuf : public basic_reportbase<tChar, tTraits, tAlloc>
	{
		void writeline(const std::basic_string<tChar, tTraits, tAlloc> &str, bool endl)
		{}
	};

#endif

	// Generates the ANSI version of debugbuf.
	basic_reportbase<char> *debugger()
	{
		return new debugbuf<char>;
	}

	// Generates the WideChar version of debugbuf.
	basic_reportbase<wchar_t> *wdebugger()
	{
		return new debugbuf<wchar_t>;
	}
}


#endif
