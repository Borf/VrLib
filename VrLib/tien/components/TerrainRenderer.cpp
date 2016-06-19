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
						gl::setN3(v, glm::vec3(0, 1, 0));
						gl::setTan3(v, glm::vec3(1, 0, 0));
						gl::setBiTan3(v, glm::vec3(0, 0, 1));

						gl::setP3(v, glm::vec3(x, 0, y));
						gl::setT2(v, glm::vec2(x, y)/10.0f);
						vertices.push_back(v);

						gl::setP3(v, glm::vec3(x+1, 0, y));
						gl::setT2(v, glm::vec2(x+1, y) / 10.0f);
						vertices.push_back(v);

						gl::setP3(v, glm::vec3(x+1, 0, y+1));
						gl::setT2(v, glm::vec2(x, y+1) / 10.0f);
						vertices.push_back(v);

						gl::setP3(v, glm::vec3(x, 0, y+1));
						gl::setT2(v, glm::vec2(x, y+1) / 10.0f);
						vertices.push_back(v);
					}
				}


				vbo.bind();
				vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
				vao = new gl::VAO<gl::VertexP3N2B2T2T2>(&vbo);



			}

		}
	}
}