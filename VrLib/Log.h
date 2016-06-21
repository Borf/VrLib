#pragma once

#include <VrLib/VrLib.h>

#include <string>

namespace vrlib
{
	class Log
	{
		bool endline;
		class EndLine{};

		void logString(char* format, ...);

	public:
		Log();

		static EndLine newline;

		Log& operator <<(char* txt);
		Log& operator <<(const char* txt);
		Log& operator <<(std::string txt);
		Log& operator <<(int txt);
		Log& operator <<(unsigned long txt);
		Log& operator <<(unsigned int txt);
		Log& operator <<(float txt);
		Log& operator <<(double txt);
		Log& operator <<(const EndLine& endline);


		static std::string format(char* fmt, ...); //TODO: move this to a better spot
	};


	extern Log logger;
}