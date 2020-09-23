#include "Terrain.h"

#include <VrLib/Image.h>

namespace vrlib
{
	namespace tien
	{
		Terrain::Terrain(const std::string &fileName, float stretch)
		{
			this->stretch = stretch;
			vrlib::Image image(fileName);

			width = image.width;
			height = image.height;

			heights.resize(image.width, std::vector<float>(image.height, 0.0f));
			for (int x = 0; x < image.width; x++)
				for (int y = 0; y < image.height; y++)
					heights[x][y] = image.data[(x + image.width * y) * image.depth] / 256.0f * stretch;
		}

		Terrain::Terrain()
		{
			this->stretch = 1;
			width = 0;
			height = 0;
		}

		void Terrain::setSize(int width, int height)
		{
			this->width = width;
			this->height = height;
			heights.resize(width, std::vector<float>(height, 0.0f));
		}

		void Terrain::setHeight(int x, int y, float height)
		{
			heights[x][y] = height;
		}

		glm::vec3 Terrain::getPosition(const glm::vec2 &p)
		{
			int rx = (int)p.x;
			int ry = (int)p.y;
			if (rx < 0 || ry < 0 || rx >= width-1 || ry >= height-1)
				return glm::vec3(p.x, 0.01, p.y);

			glm::vec3 p1 = glm::vec3(rx, ry, heights[rx][ry]);
			glm::vec3 p2 = glm::vec3(rx + 1, ry + 1, heights[rx + 1][ry + 1]);
			glm::vec3 p3 = glm::vec3(rx, ry + 1, heights[rx][ry + 1]);

			float l1 = (((p2.y - p3.y) * (p.x - p3.x)) + ((p3.x - p2.x) * (p.y - p3.y))) / (((p2.y - p3.y) * (p1.x - p3.x)) + ((p3.x - p2.x) * (p1.y - p3.y)));
			float l2 = (((p3.y - p1.y) * (p.x - p3.x)) + ((p1.x - p3.x) * (p.y - p3.y))) / (((p2.y - p3.y) * (p1.x - p3.x)) + ((p3.x - p2.x) * (p1.y - p3.y)));
			float l3 = 1 - l1 - l2;

			if (l3 < 0 || l3 > 1)
			{
				p1 = glm::vec3(rx, ry, heights[rx][ry]);
				p2 = glm::vec3(rx + 1, ry + 1, heights[rx + 1][ry + 1]);
				p3 = glm::vec3(rx + 1, ry, heights[rx + 1][ry]);

				l1 = (((p2.y - p3.y) * (p.x - p3.x)) + ((p3.x - p2.x) * (p.y - p3.y))) / (((p2.y - p3.y) * (p1.x - p3.x)) + ((p3.x - p2.x) * (p1.y - p3.y)));
				l2 = (((p3.y - p1.y) * (p.x - p3.x)) + ((p1.x - p3.x) * (p.y - p3.y))) / (((p2.y - p3.y) * (p1.x - p3.x)) + ((p3.x - p2.x) * (p1.y - p3.y)));
				l3 = 1 - l1 - l2;

			}

			float z = (l1 * p1.z) + (l2 * p2.z) + (l3 * p3.z);

			return glm::vec3(p.x, z, p.y);
		}


		std::vector<float>& Terrain::operator [] (int x)
		{
			return heights[x];
		}

	}
}