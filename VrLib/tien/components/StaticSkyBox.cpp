#include "StaticSkyBox.h"
#include "Light.h"
#include "Transform.h"

#include <VrLib/tien/Node.h>
#include <VrLib/Texture.h>
#include <VrLib/Model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VrLib/gl/Cubemap.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			void StaticSkyBox::initialize()
			{
				skybox = vrlib::Model::getModel<vrlib::gl::VertexP3>("sphere.shape");
				shader = new vrlib::gl::Shader<Uniforms>("data/vrlib/tien/shaders/skybox.vert", "data/vrlib/tien/shaders/skybox.frag");
				shader->bindAttributeLocation("a_position", 0);
				shader->link();
				shader->bindFragLocation("fragColor", 0);
				shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
				shader->registerUniform(Uniforms::modelViewMatrix, "modelViewMatrix");
				shader->use();
				cubemap = new vrlib::gl::CubeMap();
				initialized = true;
			}

			void StaticSkyBox::update(float elapsedTime, Scene & scene)
			{
			}

			void StaticSkyBox::render(const glm::mat4 & projectionMatrix, const glm::mat4 & modelviewMatrix)
			{
				glDepthMask(GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDepthMask(0);
				glCullFace(GL_FRONT);
				glDisable(GL_BLEND);

				shader->use();
				shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
				shader->setUniform(Uniforms::modelViewMatrix, glm::scale(modelviewMatrix, glm::vec3(7.5, 7.5, 7.5))); // model is 50 big

				glActiveTexture(GL_TEXTURE0);
				cubemap->bind();

				skybox->draw([](const glm::mat4 &mat) {});

				glDepthMask(1);
				glCullFace(GL_BACK);


			}


			void StaticSkyBox::setTexture(int side, const std::string &texture)
			{
				cubemap->setTexture(side, texture);
			}



			nlohmann::json StaticSkyBox::toJson(nlohmann::json &meshes) const
			{
				nlohmann::json ret;
				ret["type"] = "StaticSkyBox";
				return ret;
			}
		}
	}
}

