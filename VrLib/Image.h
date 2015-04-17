#pragma once

#include <string>

namespace vrlib
{
	class Image
	{
	public:
		int width;
		int height;
		int depth;
		char* data;

		Image(const std::string &filename);
		Image();
		~Image();
	};
}