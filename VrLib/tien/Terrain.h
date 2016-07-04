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

			Terrain(const std::string &filename, float stretch);

			friend class components::TerrainRenderer;

			inline bool isValid(int x, int y) { return x >= 0 && x < width && y >= 0 && y < height; }
			std::vector<float>& operator [] (int x);
		};
	}
}