#include "Terrain.h"

#include <VrLib/Image.h>

namespace vrlib
{
	namespace tien
	{
		Terrain::Terrain(const std::string &fileName, float stretch)
		{
			vrlib::Image image(fileName);

			width = image.width;
			height = image.height;

			heights.resize(image.width, std::vector<float>(image.height, 0.0f));
			for (int x = 0; x < image.width; x++)
				for (int y = 0; y < image.height; y++)
					heights[x][y] = image.data[(x + image.width * y) * image.depth] / 256.0f * stretch;
		}



		std::vector<float>& Terrain::operator [] (int x)
		{
			return heights[x];
		}

	}
}