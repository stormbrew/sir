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

#ifndef _sir_sir_hpp_
#define _sir_sir_hpp_

#include <string>
#include <list>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <locale>

#include <memory>

namespace sir
{
	typedef unsigned long log_type;
	extern const log_type debug, error, warning, fatal, log, screen, log_all;

	typedef unsigned long option_type;
	extern const option_type timestamp, modulename, linefeeds, option_all;


	// This is a pure virtual base class that handles outputing the text
	// to an actual display. It's also responsible for appending newlines.
	// This is because different devices may have different ways of expressing
	// newlines. 
	// In the case that endl is false and the output device only accepts data
	// in full lines, this object must also buffer up to a std::endl delimiter.
	template 
	<
		typename tChar, 
		typename tTraits = std::char_traits<tChar>, 
		typename tAlloc = std::allocator<tChar> 
	>
	class basic_reportbase
	{
	public:
		virtual void writeline(const std::basic_string<tChar, tTraits, tAlloc> &str, bool endl) = 0;
		virtual ~basic_reportbase() {}
	};

	// forward declaration for basic_reporterstream's sake
	template 
	<
		typename tChar, 
		typename tTraits = std::char_traits<tChar>, 
		typename tAlloc = std::allocator<tChar> 
	>
	class basic_reporter;

	// this class is returned by basic_reporter::operator(). It holds a reference
	// to the basic_streambuf object that basic_reporter expects the line to be placed in.
	// On construction (including copy construction), it calls basic_reporter::lock() in
	// order to indicate that a line is being written to. On destruction, it calls
	// basic_reporter::unlock() to indicate that the line is done. This mechanism
	// makes it so that the reporterstream can be copied around, and the line is only
	// written once all outstanding objects are written to. Minor modifications may
	// even make it thread safe.
	template 
	<
		typename tChar, 
		typename tTraits = std::char_traits<tChar>, 
		typename tAlloc = std::allocator<tChar> 
	>
	class basic_reporterstream
	{
	private:
		typedef basic_reporter<tChar, tTraits, tAlloc> reporter_type;
		typedef std::basic_ostream<tChar, tTraits> stream_type;
		typedef std::basic_stringbuf<tChar, tTraits, tAlloc> streambuf_type;

		reporter_type *mReporter;
		stream_type mStream;
		log_type mLog;
		option_type mOptions;

	protected:
		basic_reporterstream(reporter_type *reporter, log_type log, option_type options)
		 : mReporter(reporter),
		   mStream(new streambuf_type),
		   mLog(log),
		   mOptions(options)
		{
			if (mOptions & timestamp)
			{
				// using time_t functions to do this. I'd prefer to use boost::time stuff,
				// but that's probably not very reasonable. It really is a shame C++
				// has no standard time classes.
				std::time_t t = std::time(NULL);
				char *tstr = std::ctime(&t);
				tstr[24] = '\0'; // cut off the \n

				mStream << tstr << " ";
			}

			if (mOptions & modulename)
				mStream << "[" << mReporter->module() << "] ";
		}

	public:
		basic_reporterstream(const basic_reporterstream &other)
		 : mReporter(other.mReporter),
		   mStream(other.mStream.rdbuf()),
		   mLog(other.mLog),
		   mOptions(other.mOptions)
		{
		}

		~basic_reporterstream()
		{
			streambuf_type *buf;
			if (buf = static_cast<streambuf_type*>(mStream.rdbuf()))
				mReporter->report(mLog, buf->str(), mOptions);
		}

		std::locale imbue(const std::locale &loc)
		{
			return mStream.imbue(loc);
		}

		template <typename tOther>
		stream_type &operator<<(const tOther &out)
		{
			mStream << out;
			return mStream;
		}

		// friend is ugly, but it seems needed here. A way around would be good however.
		friend class basic_reporter<tChar, tTraits, tAlloc>;
	};

	// this class simply provides a copyable version of basic_stringbuf. The normal
	// version is uncopyable for obvious reasons, but we need to store it in a container.
	// Since the copy only happens in the std::list<> constructors, it does not even matter
	// if the string arrives intact anyways, as it will be empty.
	template 
	<
		typename tChar,
		typename tTraits,
		typename tAlloc
	>
	class copyable_stringbuf : public std::basic_stringbuf<tChar, tTraits, tAlloc>
	{
	public:
		copyable_stringbuf()
		 : std::basic_stringbuf<tChar, tTraits, tAlloc>()
		{}

		copyable_stringbuf(const copyable_stringbuf &other)
		 : std::basic_stringbuf<tChar, tTraits, tAlloc>(other.str()) // extract string and insert it into this stringbuf.
		{}
	};

	// This is the meat of the library. It holds a set of objects to output to (basic_reporterbase)
	// and manages the user input (basic_reporterstream). Each outstanding basic_reportstream object
	// increments the lock count. When all basic_reportstream objects have been destroyed (lock count is 0),
	// it should be safe to output all outstanding items.
	template 
	<
		typename tChar, 
		typename tTraits, 
		typename tAlloc
	>
	class basic_reporter
	{
	protected:
		typedef basic_reportbase<tChar, tTraits, tAlloc> reportbase_type;
		typedef std::basic_string<tChar, tTraits, tAlloc> string_type;

	public:
		typedef basic_reporterstream<tChar, tTraits, tAlloc> stream_type;

		basic_reporter(const string_type &module, option_type options = option_all)
		 : mModuleName(module),
		   mOptions(options | modulename)
		{}
		basic_reporter(option_type options)
		 : mOptions(options)
		{}

		~basic_reporter()
		{
			typename output_list::iterator it = mOutput.begin();
			while (it != mOutput.end())
				delete (it++)->second;
		}

		void report(log_type log, const std::string &line, option_type options)
		{
			// enumerate the output methods.
			typename output_list::iterator itOut = mOutput.begin();
			while (itOut != mOutput.end())
			{
				// if any bits in the log_type fields match, write the line
				// out.
				if (log & itOut->first)
					itOut->second->writeline(line, mOptions & linefeeds);

				itOut++;
			}
		}

		const std::string &module()
		{
			return mModuleName;
		}

		std::locale imbue(const std::locale &loc)
		{
			// I have no use for locales right now, so I'm not even sure if this
			// stuff works. It compiles ok on both VC7 and GCC 3.x though, so I'm
			// not complaining.
			std::locale change = loc;
			std::swap(change, mLocale);
			return change;
		}

		// Note that bind() takes ownership of the object. The reason it takes a
		// pointer instead of using a copy constructor is that most of the output
		// methods may be singletons of sorts. This makes it easier to write the
		// plugin objects because they don't have to mess with copy constructors.
		// This is perhaps subject to change, however.
		void bind(reportbase_type *obj, log_type log = log_all)
		{
			// add the output object here.
			mOutput.push_back( output_item(log, obj) );
		}

		// This returns a basic_reportstream object with a basic_stringbuf object
		// as it's buffer. No copies of the basic_reportstream object should live
		// their parent basic_report object. This perhaps needs fixing so that
		// such a situation is harmless at worst.
		stream_type operator()(log_type log, option_type options)
		{
			stream_type stream(this, log, options);
			stream.imbue(mLocale);

			return stream;
		}

		stream_type operator()(log_type log = log_all)
		{
			return operator()(log, mOptions);
		}
		
		friend class basic_reporterstream<tChar, tTraits, tAlloc>;

	private:
		// defines the list types for the output types
		typedef std::pair<log_type, reportbase_type*> output_item;
		typedef std::list<output_item> output_list;
		output_list mOutput;

		// module name, if any.
		string_type mModuleName;
		// options specified at construction. May be overridden by operator().
		option_type mOptions;

		// the locale specified by imbue, if any, or the default one otherwise.
		std::locale mLocale;
	};

	// typedefs for simple (common) reporter types.
	typedef basic_reporter<char> reporter;
	typedef basic_reporter<wchar_t> wreporter;
}

#endif
