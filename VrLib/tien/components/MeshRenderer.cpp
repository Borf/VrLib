#include "MeshRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.h>
#include <VrLib/util.h>
#include <VrLib/Image.h>
#include "Transform.h"
#include "../Node.h"


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			MeshRenderer::MeshRenderer(Mesh* mesh)
			{
				this->mesh = mesh;
				vao = nullptr;
				if (mesh)
					updateMesh();
				castShadow = true;
				renderContextDeferred = ModelRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
			}

			MeshRenderer::MeshRenderer(const json::Value &json, const json::Value &totalJson)
			{
				vao = nullptr;
				this->mesh = nullptr;
				static std::map<std::string, Mesh*> meshes;	//TODO: clear cache !

				if (json.isMember("mesh") && json["mesh"].asString() != "")
				{
					if (meshes.find(json["mesh"]) == meshes.end())
					{
						for (const auto& m : totalJson["meshes"])
							if (m["id"].asString() == json["mesh"].asString())
								meshes[json["mesh"]] = new Mesh(m);
					}
					mesh = meshes[json["mesh"]];
				}

				if(mesh)
					updateMesh();
				castShadow = json["castShadow"];
				renderContextDeferred = ModelRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
			}

			MeshRenderer::~MeshRenderer()
			{

			}

			vrlib::json::Value MeshRenderer::toJson(json::Value &meshes) const
			{
				vrlib::json::Value ret;
				ret["type"] = "meshrenderer";
				ret["castShadow"] = castShadow;
				ret["mesh"] = "";
				if (mesh)
				{
					ret["mesh"] = mesh->guid;
					bool found = false;
					for (size_t i = 0; i < meshes.size(); i++)
						if (meshes[i]["id"] == mesh->guid)
							found = true;
					if (!found)
						meshes.push_back(mesh->toJson());
				}


				return ret;
			}

			void MeshRenderer::updateMesh()
			{
				vbo.setData(mesh->vertices.size(), &mesh->vertices[0], GL_STATIC_DRAW);
				vio.setData(mesh->indices.size(), &mesh->indices[0], GL_STATIC_DRAW);

				if (!vao)
					vao = new gl::VAO(&vbo);
				vao->bind();
				vio.bind();
				vao->unBind();
			}


			void MeshRenderer::drawDeferredPass()
			{
				components::Transform* t = node->getComponent<Transform>();

				ModelRenderContext* context = dynamic_cast<ModelRenderContext*>(renderContextDeferred);
				context->renderShader->use(); //TODO: only call this once!

				context->renderShader->setUniform(ModelRenderContext::RenderUniform::modelMatrix, t->globalTransform);
				context->renderShader->setUniform(ModelRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				if (mesh->material.texture)
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 1.0f);
					mesh->material.texture->bind();
					glActiveTexture(GL_TEXTURE1);
					if (mesh->material.normalmap)
						mesh->material.normalmap->bind();
					else
						context->defaultNormalMap->bind();
					glActiveTexture(GL_TEXTURE0);

				}
				else
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 0.0f);
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::diffuseColor, mesh->material.color.diffuse);
				}


				if (vao)
				{
					vao->bind();
					glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
					vao->unBind();
				}
			}


			void MeshRenderer::drawShadowMap()
			{
				if (!castShadow)
					return;
				components::Transform* t = node->getComponent<Transform>();
				ModelShadowRenderContext* context = dynamic_cast<ModelShadowRenderContext*>(renderContextShadow);
				context->renderShader->use(); //TODO: only call this once!
				context->renderShader->setUniform(ModelShadowRenderContext::RenderUniform::modelMatrix, t->globalTransform);


			}


			void MeshRenderer::ModelRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/default.vert", "data/vrlib/tien/shaders/default.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->link();
				renderShader->bindFragLocation("fragColor", 0);
				renderShader->bindFragLocation("fragNormal", 1);
				//shader->bindFragLocation("fragColor", 0);
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
				renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
				renderShader->registerUniform(RenderUniform::s_normalmap, "s_normalmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void MeshRenderer::ModelRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}


			void MeshRenderer::ModelShadowRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/defaultShadow.vert", "data/vrlib/tien/shaders/defaultShadow.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->link();
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::outputPosition, "outputPosition");				
				renderShader->use();
			}

			void MeshRenderer::ModelShadowRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
			}
			void MeshRenderer::ModelShadowRenderContext::useCubemap(bool use)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::outputPosition, use);
			}


			MeshRenderer::Mesh::Mesh()
			{
				guid = vrlib::util::getGuid();
			}

			MeshRenderer::Mesh::Mesh(const json::Value & json)
			{
				guid = json["id"];

				for (int i : json["indices"])
					indices.push_back(i);
				for (const auto &v : json["vertices"])
				{
					vrlib::gl::VertexP3N2B2T2T2 vv;
					vrlib::gl::setP3(vv, glm::vec3(v["pos"][0], v["pos"][1], v["pos"][2]));
					vrlib::gl::setN3(vv, glm::vec3(v["normal"][0], v["normal"][1], v["normal"][2]));
					vrlib::gl::setTan3(vv, glm::vec3(v["tan"][0], v["tan"][1], v["tan"][2]));
					vrlib::gl::setBiTan3(vv, glm::vec3(v["bitan"][0], v["bitan"][1], v["bitan"][2]));
					vrlib::gl::setT2(vv, glm::vec2(v["tex"][0], v["tex"][1]));
					vertices.push_back(vv);
				}

				if(json["material"].isMember("diffuse"))
					material.texture = vrlib::Texture::loadCached(json["material"]["diffuse"].asString());
				if (json["material"].isMember("normal"))
					material.normalmap = vrlib::Texture::loadCached(json["material"]["normal"].asString());

			}

			vrlib::json::Value MeshRenderer::Mesh::toJson()
			{
				vrlib::json::Value ret;
				ret["id"] = guid;

				for (auto i : indices)
					ret["indices"].push_back((int)i);

				for (auto &v : vertices)
				{
					vrlib::json::Value vv;
					vv["pos"].push_back(v.px);
					vv["pos"].push_back(v.py);
					vv["pos"].push_back(v.pz);

					vv["normal"].push_back(v.nx);
					vv["normal"].push_back(v.ny);
					vv["normal"].push_back(v.nz);

					vv["tan"].push_back(v.tanx);
					vv["tan"].push_back(v.tany);
					vv["tan"].push_back(v.tanz);

					vv["bitan"].push_back(v.bitanx);
					vv["bitan"].push_back(v.bitany);
					vv["bitan"].push_back(v.bitanz);

					vv["tex"].push_back(v.tx);
					vv["tex"].push_back(v.ty);

					ret["vertices"].push_back(vv);
				}

				if(material.texture && material.texture->image)
					ret["material"]["diffuse"] = material.texture->image->fileName;
				if(material.normalmap && material.normalmap->image)
					ret["material"]["normal"] = material.normalmap->image->fileName;


				return ret;
			}

		}
	}
}