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
		
		bool loaded;
		bool unloadImageAfterLoad = true;
		bool resizeToLog = false;
		bool textureRepeat = true;
		bool nearestFilter = false;
		

		Texture(const std::string &fileName);
		void load();
		void bind();

		void loadDds(const std::string &fileName);
	};
}