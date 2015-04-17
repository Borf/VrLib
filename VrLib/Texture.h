#pragma once

#include <gl/glew.h>
#include <string>

namespace vrlib
{
	class Image;
	class Texture
	{
	public:
		Image* image;
		GLuint texid;

		Texture(const std::string &fileName);
	};
}