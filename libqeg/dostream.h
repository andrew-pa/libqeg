// This is a wrapper to make ::OutputDebugStringA behave like std::ostream
// Derived from code written by Sven Axelsson
// Documented: http://www.codeproject.com/KB/debug/debugout.aspx
#pragma once
#include <Windows.h>
#include <ostream>
#include <sstream>
#include <string>

namespace qeg 
{ 
	namespace detail
	{
#ifdef WIN32
		template <class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_debugbuf :     public std::basic_stringbuf<CharT, TraitsT>
		{
		public:    
			virtual ~basic_debugbuf() 
			{        
				sync();    
			}
		protected:    
			int sync() 
			{       
				output_debug_string(str().c_str());        
				str(std::basic_string<CharT>());    
				// Clear the string buffer       
				return 0;    
				}    
				void output_debug_string(const CharT *text) {}
		};
		template<>void basic_debugbuf<char>::output_debug_string(const char *text)
		{    
			::OutputDebugStringA(text);
		}
		template<>void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text)
		{    
			::OutputDebugStringW(text);
		}
		
		template<class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_dostream :     public std::basic_ostream<CharT, TraitsT>
		{
		public:
			basic_dostream() 
				: std::basic_ostream<CharT, TraitsT>(new basic_debugbuf<CharT, TraitsT>()) {}   
				~basic_dostream()     
				{        
					delete rdbuf();     
				}
		};
		typedef basic_dostream<char>    dostream;
		typedef basic_dostream<wchar_t> wdostream;
#endif

		template <class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_nullbuf : public std::basic_stringbuf<CharT, TraitsT>
		{
		public:
			virtual ~basic_nullbuf()
			{
				sync();
			}
		protected:
			int sync()
			{
				str(std::basic_string<CharT>());
				// Clear the string buffer       
				return 0;
			}
		};
		template<class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_null_stream : public std::basic_ostream<CharT, TraitsT>
		{
		public:
			basic_null_stream()
				: std::basic_ostream<CharT, TraitsT>(new basic_nullbuf<CharT, TraitsT>()) {}
			~basic_null_stream()
			{
				delete rdbuf();
			}
		};
		typedef basic_null_stream<char>    null_stream;
		typedef basic_null_stream<wchar_t> wnull_stream;
	}
}
				
