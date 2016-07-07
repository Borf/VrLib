#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

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
			float stretch;
		public:
			int getHeight() { return height; }
			int getWidth() { return width; }
			float getStretch() { return stretch; }

			Terrain(const std::string &filename, float stretch);

			friend class components::TerrainRenderer;

			glm::vec3 getPosition(const glm::vec2 &pos);


			inline bool isValid(int x, int y) { return x >= 0 && x < width && y >= 0 && y < height; }
			std::vector<float>& operator [] (int x);
		};
	}
}