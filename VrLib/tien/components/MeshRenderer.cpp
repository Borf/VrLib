#include "MeshRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.hpp>
#include <VrLib/util.h>
#include <VrLib/Image.h>
#include <VrLib/math/Ray.h>
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

			MeshRenderer::MeshRenderer(const json &data, const json &totalJson)
			{
				vao = nullptr;
				this->mesh = nullptr;
				static std::map<std::string, Mesh*> meshes;	//TODO: clear cache !

				if (data.find("mesh") != data.end() && data["mesh"] != "")
				{
					if (meshes.find(data["mesh"]) == meshes.end())
					{
						for (const auto& m : totalJson["meshes"])
							if (m["id"] == data["mesh"])
								meshes[data["mesh"]] = new Mesh(m);
					}
					mesh = meshes[data["mesh"]];
				}

				if(mesh)
					updateMesh();
				castShadow = data["castShadow"];
				renderContextDeferred = ModelRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
			}

			MeshRenderer::~MeshRenderer()
			{

			}

			json MeshRenderer::toJson(json &meshes) const
			{
				json ret;
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

			void MeshRenderer::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Mesh Renderer");
				if (folded)
					return;
				builder->beginGroup("Mesh Vertices");
				builder->addLabel(std::to_string(mesh->vertices.size()));
				builder->endGroup();


				builder->beginGroup("Casts Shadows");
				builder->addCheckbox(castShadow, [this](bool newValue) {castShadow = newValue; });
				builder->endGroup();

				builder->beginGroup("Cull backfaces");
				builder->addCheckbox(cullBackFaces, [this](bool newValue) {	cullBackFaces = newValue; });
				builder->endGroup();

				{
					builder->addTitle("Material");

					builder->beginGroup("Ambient", true);
					builder->addTextBox("#AABBCC", [](const std::string &newText) {}); //TODO: color picker
					builder->endGroup();
					builder->beginGroup("Diffuse", true);
					builder->addTextBox("#AABBCC", [](const std::string &newText) {}); //TODO: color picker
					builder->endGroup();
					builder->beginGroup("Specular", true);
					builder->addTextBox("#AABBCC", [](const std::string &newText) {}); //TODO: color picker
					builder->endGroup();

					builder->beginGroup("Texture", false);
					auto textureBox = builder->addTextureBox((materialOverride.texture && materialOverride.texture->image) ? materialOverride.texture->image->fileName : "", [this](const std::string &newFile) {
						materialOverride.texture = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, textureBox]()
					{
						textureBox->setText("");
						materialOverride.texture = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Normalmap", false);
					auto normalBox = builder->addTextureBox((materialOverride.normalmap && materialOverride.normalmap->image) ? materialOverride.normalmap->image->fileName : "", [this](const std::string &newFile) {
						materialOverride.normalmap = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, normalBox]()
					{
						normalBox->setText("");
						materialOverride.normalmap = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Specularmap", false);
					auto specBox = builder->addTextureBox((materialOverride.specularmap && materialOverride.specularmap->image) ? materialOverride.specularmap->image->fileName : "", [this](const std::string &newFile) {
						materialOverride.specularmap = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, specBox]()
					{
						specBox->setText("");
						materialOverride.specularmap = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Shinyness");
					builder->addTextBox(builder->toString(materialOverride.color.shinyness), [this](const std::string &newValue) { materialOverride.color.shinyness = (float)atof(newValue.c_str()); });
					builder->endGroup();

				}

				builder->beginGroup("");

				builder->endGroup();
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

			void MeshRenderer::update(float elapsedTime, Scene & scene)
			{
				if (mesh != prevMesh)
				{
					materialOverride = mesh->material;
					prevMesh = mesh;
				}
//				if(...)
//				mesh->material.texture->update(elapsedTime);
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
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/ModelRenderer.deferred.vert", "data/vrlib/tien/shaders/ModelRenderer.deferred.frag");
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
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/ModelRenderer.shadow.vert", "data/vrlib/tien/shaders/ModelRenderer.shadow.frag");
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

			MeshRenderer::Mesh::Mesh(const json &data)
			{
				guid = data["id"];

				for (int i : data["indices"])
					indices.push_back(i);
				for (const auto &v : data["vertices"])
				{
					vrlib::gl::VertexP3N2B2T2T2 vv;
					vrlib::gl::setP3(vv, glm::vec3(v["pos"][0], v["pos"][1], v["pos"][2]));
					vrlib::gl::setN3(vv, glm::vec3(v["normal"][0], v["normal"][1], v["normal"][2]));
					vrlib::gl::setTan3(vv, glm::vec3(v["tan"][0], v["tan"][1], v["tan"][2]));
					vrlib::gl::setBiTan3(vv, glm::vec3(v["bitan"][0], v["bitan"][1], v["bitan"][2]));
					vrlib::gl::setT2(vv, glm::vec2(v["tex"][0], v["tex"][1]));
					vertices.push_back(vv);
				}

				if(data["material"].find("diffuse") != data["material"].end())
					material.texture = vrlib::Texture::loadCached(data["material"]["diffuse"]);
				if (data["material"].find("normal") != data["material"].end())
					material.normalmap = vrlib::Texture::loadCached(data["material"]["normal"]);

			}

			json MeshRenderer::Mesh::toJson()
			{
				json ret;
				ret["id"] = guid;

				for (auto i : indices)
					ret["indices"].push_back((int)i);

				for (auto &v : vertices)
				{
					json vv;
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

			std::vector<float> MeshRenderer::Mesh::collisionFractions(const vrlib::math::Ray & ray)
			{
				std::vector<float> result;
//				if (!aabb.hasRayCollision(ray, 0, 10000)) //TODO
//					return result;
				float f = 0;

				glm::vec3 v[3];

				for (size_t i = 0; i < indices.size(); i += 3)
				{
					for (int ii = 0; ii < 3; ii++)
						v[ii] = gl::getP3(vertices[indices[i + ii]]);
					if (ray.LineIntersectPolygon(v, 3, f))
						if (f > 0)
							result.push_back(f);
				}
				return result;
			}

		}
	}
}