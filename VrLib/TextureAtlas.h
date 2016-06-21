#pragma once
#include <VrLib/VrLib.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <map>
#include <string>

namespace vrlib
{
	class TASubTexture
	{
		std::string filename;
		glm::vec2 startPosition;
		glm::vec2 dimensions;

	public:
		TASubTexture(std::string filename, glm::vec2 startPosition, glm::vec2 dimensions);
		~TASubTexture();

		std::string getFilename();
		glm::vec2 getStartPosition();
		glm::vec2 getDimensions();
	};

	class TextureAtlas
	{
		GLuint textureID;
		std::map<std::string, TASubTexture*> subTextures;
		int currentX, currentY, maxY;

	public:
		TextureAtlas();
		~TextureAtlas();

		TASubTexture* addTexture(std::string filename);
		int getTextureSize();
		void bind();
	};
}
