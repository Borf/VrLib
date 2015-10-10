#pragma once

#include <gl/glew.h>
#include <string>
#include <map>

namespace vrlib
{
	class Image;
	class Texture
	{
		GLuint texid;
		Texture(const std::string &fileName);

	public:
		static std::map<std::string, Texture*> cache;
		static Texture* loadCached(const std::string &fileName);

		Image* image;
		
		bool loaded;
		bool unloadImageAfterLoad = true;
		bool resizeToLog = false;
		bool textureRepeat = true;
		bool nearestFilter = false;
		

		void load();
		void bind();

		void loadDds(const std::string &fileName);
	};
}