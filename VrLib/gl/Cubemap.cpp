#include "Cubemap.h"
#include "Shader.h"

#include <vrlib/stb_image.h>

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>


namespace vrlib
{
	namespace gl
	{

		/*
		Constructor
		shader = shader to use for drawing
		filenames = locations of the 6 textures
		cubemapVertices = (36) vertices of the cubemap
		Author: Bas Rops - 22-05-2014
		Last edit: <name> - dd-mm-yyyy
		*/
		Cubemap::Cubemap(ShaderProgram* shader, std::string filenames[6], const std::vector<glm::vec3> &cubemapVertices)
		{
			if (shader != NULL)
				this->shader = shader;
			else
				std::cout << "Error initializing Cubemap: Shader == NULL" << std::endl;

			//Generate 1 texture
			glGenTextures(1, &texid);
			//Bind the texture as a cubemap texture
			glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
			//Set cubemap parameters
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			//Load all 6 textures for the cubemap
			for (int i = 0; i < 6; i++)
			{
				std::string* filename = &filenames[i];

				//Open the file
				FILE* pFile = fopen(filename->c_str(), "rb");
				if (pFile)
				{
					//Get the data from the texture
					BYTE* data = stbi_load_from_file(pFile, &width, &height, &components, 4);
					//Close the file
					fclose(pFile);

					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

					//Data no longer required, since it has been copied into a texture
					delete data;
				}
				else
					std::cout << "Error while loading cubemap texture: " << *filename << " does not exist." << std::endl;
			}

			//Add all vertices of the cubemap to a VBO
			std::vector<VertexPosition> vertices;

			for(const glm::vec3 &vertex : cubemapVertices)
			{
				vertices.push_back(VertexPosition(vertex));
			}

			vbo = new VBO<VertexPosition>();

			vbo->bind();
			vbo->setData(vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
			vertices.clear();
		}

		/*
		Destructor
		Author: Bas Rops - 22-05-2014
		Last edit: <name> - dd-mm-yyyy
		*/
		Cubemap::~Cubemap(void)
		{
			delete shader;
			delete vbo;
		}

		/*
		Binds the current cubemap texture using texid
		Author: Bas Rops - 22-05-2014
		Last edit: <name> - dd-mm-yyyy
		*/
		void Cubemap::bindTexture()
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
		}

		/*
		Draw the cubemap
		Author: Bas Rops - 22-05-2014
		Last edit: <name> - dd-mm-yyyy
		*/
		void Cubemap::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix)
		{
			if (shader)
			{
				glm::mat4 projectionMatrixNoTranslation = projectionMatrix;
				projectionMatrixNoTranslation[3][0] = 0;
				projectionMatrixNoTranslation[3][1] = 0;
				projectionMatrixNoTranslation[3][2] = 0;
				projectionMatrixNoTranslation[3][3] = 1;

				//Remove the translation from the viewMatrix
				glm::mat4 viewMatrixNoTranslation = viewMatrix;
				viewMatrixNoTranslation[3][0] = 0;
				viewMatrixNoTranslation[3][1] = 0;
				viewMatrixNoTranslation[3][2] = 0;
				viewMatrixNoTranslation[3][3] = 1;

				glBindVertexArray(0);
				glDepthMask(GL_FALSE);
				shader->use();
				bindTexture();
				shader->setUniformMatrix4("viewProjectionMatrix", projectionMatrixNoTranslation * viewMatrixNoTranslation);
				vbo->bind();
				glEnableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				vbo->setVAO();
				glDrawArrays(GL_TRIANGLES, 0, vbo->getLength());
				vbo->unBind();
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				glDepthMask(GL_TRUE);
			}
		}
	}
}
