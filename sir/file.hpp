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

#ifndef _sir_file_hpp_
#define _sir_file_hpp_

#include <fstream>
#include <stdexcept>

#include "sir.hpp"

namespace sir
{
	// This class implements basic_reportbase (see sir.hpp) for a basic_streambuf derived object (filebuf, stringbuf, etc)
	// Note that the streambuf is deleted on destruction. Use basic_reportbuf_nodelete if it should not delete. This could
	// probably be done better as a policy template.
	template 
	<
		typename tChar, 
		typename tTraits = std::char_traits<tChar>, 
		typename tAlloc = std::allocator<tChar> 
	>
	class basic_reportbuf : public basic_reportbase<tChar, tTraits, tAlloc>
	{
	private:
		std::basic_streambuf<tChar, tTraits> *mBuf;

	public:
		basic_reportbuf(std::basic_streambuf<tChar, tTraits> *buf)
		 : mBuf(buf)
		{}

		void writeline(const std::basic_string<tChar, tTraits, tAlloc> &str, bool endl)
		{
			if (mBuf)
			{
				mBuf->sputn(str.data(), str.length());
				if (endl)
					mBuf->sputn("\n", 1);
				mBuf->pubsync(); // gcc std::basic_filebuf doesn't syncronize as often as VC's. It may not output anything if this isn't called.
			}
		}

		std::basic_streambuf<tChar, tTraits> *rdbuf()
		{
			return mBuf;
		}
		std::basic_streambuf<tChar, tTraits> *rdbuf(std::basic_streambuf<tChar, tTraits> *buf)
		{
			std::swap(buf, mBuf);
			return buf;
		}
		~basic_reportbuf()
		{
			delete mBuf;
		}
	};

	// this specializes basic_reportbuf so it doesn't delete the streambuf. This is primarily used for
	// cout/cerr where the streambuf object should never be deleted by outside forces.
    template 
	<
        typename tChar,
        typename tTraits = std::char_traits<tChar>,
        typename tAlloc = std::allocator<tChar>
    >
	class basic_reportbuf_nodelete : public basic_reportbuf<tChar, tTraits, tAlloc>
	{
	public:
		basic_reportbuf_nodelete(std::basic_streambuf<tChar, tTraits> *buf)
		 : basic_reportbuf<tChar, tTraits, tAlloc>(buf)
		{}

		~basic_reportbuf_nodelete()
		{
			// set the buffer to null before being destroyed.
			rdbuf(NULL);
		}
	};

	// This function generates a reportbase object out of a file.
	// Arguments are the same as std::fstream::open(), except std::string
	// is used instead of char*
	basic_reportbase<char> *file(const std::string &filename, 
		std::ios_base::openmode mode = std::ios_base::out | std::ios_base::app)
	{
		std::basic_filebuf<char> *buf = new std::basic_filebuf<char>;
		buf->open(filename.c_str(), mode);
		return new basic_reportbuf<char>(buf);
	}

	// Generates a reportbase object out of an existing ostream object, such as
	// cout or cerr. The streambuf object taken from the stream will not be
	// destroyed at any point.
	basic_reportbase<char> *standard(std::basic_ostream<char> &stream)
	{
		return new basic_reportbuf_nodelete<char>( stream.rdbuf() );
	}
}

#endif
