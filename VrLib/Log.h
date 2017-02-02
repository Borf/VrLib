#pragma once

#include <VrLib/VrLib.h>
#include <VrLib/json.hpp>

#include <string>
#include <glm/glm.hpp>

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
		Log& operator <<(const glm::vec2 &vec);
		Log& operator <<(const glm::vec3 &vec);
		Log& operator <<(const glm::vec4 &vec);
		Log& operator <<(const EndLine& endline);
		Log& operator <<(const json &json);

		static std::string format(char* fmt, ...); //TODO: move this to a better spot
	};


	extern Log logger;
}