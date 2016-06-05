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
		static std::map<std::string, std::pair<Texture*, int>> cache;
		static Texture* loadCached(const std::string &fileName);
		static void unloadCached(Texture* texture);

		Image* image;
		
		bool loaded;
		bool unloadImageAfterLoad = true;
		bool resizeToLog = false;
		bool textureRepeat = true;
		bool nearestFilter = false;
		

		void load();
		void bind();

		void loadDds(const std::string &fileName);

		void setNearestFilter();
		void setTextureRepeat(bool repeat);
	};
}