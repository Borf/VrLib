#include "Image.h"

#include "stb_image.h"
#include <iostream>

#include <VrLib/stb_image_write.h>


namespace vrlib
{
	Image::Image(const std::string &filename, bool flip)
	{
		if (filename.substr(filename.size() - 4) == ".dds")
		{
			throw "cannot be loaded";

		}
		else
		{
			width = -1;
			height = -1;
			depth = -1;
			usesAlpha = false;
			stbi_set_flip_vertically_on_load(flip);
			data = stbi_load(filename.c_str(), &width, &height, &depth, 4);
			if (!data)
			{
				const char* err = stbi_failure_reason();
				std::cout << "Error loading file " << filename << std::endl;
				std::cout << err << std::endl;
				return;
			}

			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
					if (data[(x + width*y) * 4 + 3] < 250)
					{
						usesAlpha = true;
						break;
					}

		}
	}


	Image::Image(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->depth = 4;
		this->usesAlpha = false;

		data = new unsigned char[width*height*depth];
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				for (int i = 0; i < 4; i++)
					data[4 * (x + width*y) + i] = 255;
	}



	Image::~Image()
	{
		if (data)
			stbi_image_free(data);
		data = NULL;
	}

	void Image::unload()
	{
		if (data)
			stbi_image_free(data);
		data = NULL;
	}

	void Image::save(const std::string & fileName)
	{
		stbi_write_png("test.png", width, height, depth, data,0);
	}

}