#include "TerrainRenderer.h"
#include "../Terrain.h"
#include <vector>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TerrainRenderer::TerrainRenderer(Terrain* terrain)
			{
				smoothNormals = true;
				this->terrain = terrain;

				
				std::vector<gl::VertexP3N2B2T2T2> vertices;
				for (int x = 0; x < terrain->width-1; x++)
				{
					for (int y = 0; y < terrain->height - 1; y++)
					{
						gl::VertexP3N2B2T2T2 v;
						vertices.push_back(v)
					}
				}



			}

		}
	}
}