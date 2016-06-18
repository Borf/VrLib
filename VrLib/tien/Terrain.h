#pragma once

#include <string>
#include <vector>

namespace vrlib
{
	namespace tien
	{
		namespace components { class TerrainRenderer; }
		class Terrain
		{
			std::vector<std::vector<float> > heights;
			int height;
			int width;
		public:
			int getHeight() { return height; }
			int getWidth() { return width; }

			Terrain(const std::string &filename);


			friend class components::TerrainRenderer;
		};
	}
}