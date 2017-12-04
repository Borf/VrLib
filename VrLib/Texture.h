#pragma once

#include <GL/glew.h>
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
		Texture(Image* image);
		~Texture();
		virtual void update(float elapsedTime);
		Image* image;
		
		bool loaded;
		bool unloadImageAfterLoad = true;
		bool resizeToLog = false;
		bool textureRepeat = true;
		bool nearestFilter = false;
		

		bool usesAlphaChannel;

		void load();
		virtual void bind();

		void loadDds(const std::string &fileName);

		void setNearestFilter();
		void setTextureRepeat(bool repeat);
	};
}