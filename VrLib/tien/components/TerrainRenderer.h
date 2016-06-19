#pragma once

#include "Renderable.h"

#include <VrLib/gl/VIO.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/Vertex.h>

namespace vrlib
{
	namespace tien
	{
		class Terrain;
		namespace components
		{
			class TerrainRenderer : public Renderable
			{
				gl::VBO<gl::VertexP3N2B2T2T2> vbo;
				gl::VIO<unsigned int> vio;
				gl::VAO<gl::VertexP3N2B2T2T2>* vao;
			public:
				TerrainRenderer(Terrain* terrain);
				Terrain* terrain;
				bool smoothNormals;
			};
		}
	}
}