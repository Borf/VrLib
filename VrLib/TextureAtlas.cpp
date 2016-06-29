#include "TextureAtlas.h"

#include "stb_image.h"

#include <iostream>

#define TEXTURE_ATLAS_SIZE 4096

namespace vrlib
{

	/*
	Constructor
	Creates a new texture with NULL as data
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	TextureAtlas::TextureAtlas()
	{
		glGenTextures(1, &textureID);
		bind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_ATLAS_SIZE, TEXTURE_ATLAS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		this->currentX = this->currentY = this->maxY = 0;
	}

	/*
	Destructor
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	TextureAtlas::~TextureAtlas()
	{
		for(auto subTexture : subTextures)
		{
			delete subTexture.second;
		}
		subTextures.clear();
	}

	/*
	Add a texture to the textureAtlas
	return nullptr when texture could not be added
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	TASubTexture* TextureAtlas::addTexture(std::string filename)
	{
		//Make sure the correct texture is bound
		bind();

		//Check if map already contains texture, if so, return that
		if (subTextures.count(filename) <= 0)
		{
			int width, height, components;
			unsigned char* data;

			//Open the file
			FILE* pFile = fopen(filename.c_str(), "rb");
			if (pFile)
			{
				//Get the data from the texture
				data = stbi_load_from_file(pFile, &width, &height, &components, 4);
				//Close the file
				fclose(pFile);

				//Check if the image width fits in the texture atlas
				if (this->currentX + width <= TEXTURE_ATLAS_SIZE)
				{
					//Check if the image height fits in the texture atlas
					if (this->currentY + height <= TEXTURE_ATLAS_SIZE)
					{
						//Load texture at currentX and currentY with width and height
						glTexSubImage2D(GL_TEXTURE_2D, 0, currentX, currentY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

						//Then create a new TATexture and add it to subTextures
						subTextures[filename] = new TASubTexture(filename, glm::vec2(currentX, currentY), glm::vec2(width, height));

						//Increase currentX after creating the texture and TASubTexture
						this->currentX += width;

						//Increase the maxY if needed
						if (this->currentY + height > maxY)
							this->maxY = this->currentY + height;
					}
					else
					{
						std::cout << "Error while loading texture: " << filename << " does not fit in the texture atlas anymore. CurrentX/Y("
							<< this->currentX << ", " << this->currentY << "), MaxY(" << maxY << "), ImageSize("
							<< width << ", " << height << ')' << std::endl;
						delete data;
						return nullptr;
					}
				}
				else
				{
					//Check if the image height fits in the texture atlas
					if (maxY + height <= TEXTURE_ATLAS_SIZE)
					{
						if (width <= TEXTURE_ATLAS_SIZE)
						{
							//Reset currentX and set currentY to dimensions.y (starting a new line for textures)
							currentX = 0;
							currentY = maxY;

							//Load texture at currentX and currentY with width and height
							glTexSubImage2D(GL_TEXTURE_2D, 0, currentX, currentY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

							//Then create a new TATexture and add it to subTextures
							subTextures[filename] = new TASubTexture(filename, glm::vec2(currentX, currentY), glm::vec2(width, height));

							//Increase currentX after creating the texture and TASubTexture
							this->currentX += width;

							//Increase the maxY if needed
							if (this->currentY + height > maxY)
								maxY = this->currentY + height;
						}
					}
					else
					{
						std::cout << "Error while loading texture: " << filename << " does not fit in the texture atlas anymore. CurrentX/Y("
							<< this->currentX << ", " << this->currentY << "), MaxY(" << maxY << "), ImageSize("
							<< width << ", " << height << ')' << std::endl;
						delete data;
						return nullptr;
					}
				}

				//Data no longer required, since it has been copied into a texture
				delete data;
			}
			else
				std::cout << "Error while loading texture: " << filename << " does not exist." << std::endl;
		}

		return subTextures[filename];
	}

	/*
	Retrieve the size (both x and y) of the TextureAtlas
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	int TextureAtlas::getTextureSize()
	{
		return TEXTURE_ATLAS_SIZE;
	}

	/*
	Bind the textureAtlas so it can be used by OpenGL
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	void TextureAtlas::bind()
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	/*
	Constructor
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	TASubTexture::TASubTexture(std::string filename, glm::vec2 startPosition, glm::vec2 dimensions)
	{
		this->filename = filename;
		this->startPosition = startPosition;
		this->dimensions = dimensions;
	}

	/*
	Destructor
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	TASubTexture::~TASubTexture()
	{
	}

	/*
	Retrieve the name of the file of the subtexture
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	std::string TASubTexture::getFilename()
	{
		return this->filename;
	}

	/*
	Retrieve the startposition of the subtexture in the textureAtlas
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	glm::vec2 TASubTexture::getStartPosition()
	{
		return this->startPosition;
	}

	/*
	Retrieve the dimensions of the subtexture
	Author: Bas Rops - 03-06-2014
	Last edit: <name> - dd-mm-yyyy
	*/
	glm::vec2 TASubTexture::getDimensions()
	{
		return this->dimensions;
	}

}