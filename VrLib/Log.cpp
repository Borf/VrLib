#include "Log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

namespace vrlib
{
	Log::EndLine Log::newline;

	Log::Log()
	{
		endline = true;
	}

	void Log::logString(char* fmt, ...)
	{
		if (endline)
		{
			endline = false;

			SYSTEMTIME beg;
			GetLocalTime(&beg);
			logString("[%02d:%02d:%02d:%03d]\t", beg.wHour, beg.wMinute, beg.wSecond, beg.wMilliseconds);
		}

		char text[10240];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf_s(text, 10240, fmt, ap);
		va_end(ap);

		std::cout << text;
#ifdef WIN32
		OutputDebugStringA(text);
#endif
	}

	Log& Log::operator<<(char* txt)
	{
		logString("%s", txt);
		return *this;
	}

	Log& Log::operator<<(int txt)
	{
		logString("%i", txt);
		return *this;
	}

	Log& Log::operator<<(unsigned long txt)
	{
		logString("%lu", txt);
		return *this;
	}

	Log& Log::operator<<(unsigned int txt)
	{
		logString("%u", txt);
		return *this;
	}

	Log& Log::operator<<(float txt)
	{
		logString("%f", txt);
		return *this;
	}

	Log& Log::operator<<(double txt)
	{
		logString("%f", txt);
		return *this;
	}

	Log& Log::operator<<(std::string txt)
	{
		logString("%s", txt.c_str());
		return *this;
	}

	Log& Log::operator<<(const EndLine& e)
	{
		logString("%s", "\n");
		endline = true;
		return *this;
	}

	std::string Log::format(char* fmt, ...)
	{
		char text[10240];
		va_list ap;
		if (fmt == NULL)
			return "";
		va_start(ap, fmt);
		vsprintf_s(text, 10240, fmt, ap);
		va_end(ap);

		return text;
	}
}