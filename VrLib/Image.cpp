#include "Image.h"

#include "stb_image.h"
#include <iostream>

namespace vrlib
{
	Image::Image(const std::string &filename)
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
			stbi_set_flip_vertically_on_load(true);
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

}