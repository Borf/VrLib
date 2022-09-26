#include "MeshRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.hpp>
#include <VrLib/util.h>
#include <VrLib/Image.h>
#include <VrLib/math/Ray.h>
#include <VrLib/Texture.h>
#include "Transform.h"
#include "../Node.h"
#include "../Renderer.h"

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
				renderContextDeferred = ModelDeferredRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
				renderContextForward = ModelForwardRenderContext::getInstance("");
			}

			MeshRenderer::MeshRenderer(const nlohmann::json &data, const nlohmann::json &totalJson)
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
				renderContextDeferred = ModelDeferredRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
				if (data.find("forwardShader") == data.end())
					renderContextForward = ModelForwardRenderContext::getInstance("");
				else
					renderContextForward = ModelForwardRenderContext::getInstance(data["forwardShader"]);
			}

			MeshRenderer::~MeshRenderer()
			{

			}

			nlohmann::json MeshRenderer::toJson(nlohmann::json &meshes) const
			{
				nlohmann::json ret;
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
				builder->beginGroup("Mesh Triangles");
				builder->addLabel(std::to_string(mesh->indices.size() / 3));
				builder->endGroup();


				builder->beginGroup("Casts Shadows");
				builder->addCheckbox(castShadow, [this](bool newValue) {castShadow = newValue; });
				builder->endGroup();

				builder->beginGroup("Cull backfaces");
				builder->addCheckbox(cullBackFaces, [this](bool newValue) {	cullBackFaces = newValue; });
				builder->endGroup();

				builder->beginGroup("Use deferred");
				builder->addCheckbox(useDeferred, [this](bool newValue) {	useDeferred = newValue; });
				builder->endGroup();

				{
					builder->addTitle("Material");

					builder->beginGroup("Ambient", true);
					builder->addColorBox(mesh->material.color.ambient, [this](const glm::vec4 &newValue) { mesh->material.color.ambient = newValue; });
					builder->endGroup();
					builder->beginGroup("Diffuse", true);
					builder->addColorBox(mesh->material.color.diffuse, [this](const glm::vec4 &newValue) { mesh->material.color.diffuse = newValue; });
					builder->endGroup();
					builder->beginGroup("Specular", true);
					builder->addColorBox(glm::vec4(mesh->material.color.specular,1), [this](const glm::vec4 &newValue) { mesh->material.color.specular = glm::vec3(newValue); });
					builder->endGroup();

					builder->beginGroup("Texture", false);
					auto textureBox = builder->addTextureBox(mesh->material.texture ? mesh->material.texture->image->fileName : "", [this](const std::string &newFile) {
						mesh->material.texture = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, textureBox]()
					{
						textureBox->setText("");
						mesh->material.texture = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Normalmap", false);
					auto normalmapBox = builder->addTextureBox(mesh->material.normalmap ? mesh->material.normalmap->image->fileName : "", [this](const std::string &newFile) {
						mesh->material.normalmap = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, normalmapBox]()
					{
						normalmapBox->setText("");
						mesh->material.normalmap = nullptr;
					});
					builder->endGroup();


					builder->beginGroup("Specularmap", false);
					auto specBox = builder->addTextureBox(mesh->material.normalmap ? mesh->material.normalmap->image->fileName : "", [this](const std::string &newFile) {
						mesh->material.specularmap = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, specBox]()
					{
						specBox->setText("");
						mesh->material.specularmap = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Shinyness");
					builder->addFloatBox(mesh->material.color.shinyness, 0, 1000, [this](float newValue) { mesh->material.color.shinyness = newValue; });
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
					prevMesh = mesh;
				}
//				if(...)
//				mesh->material.texture->update(elapsedTime);
			}


			void MeshRenderer::drawDeferredPass()
			{
				if (!useDeferred)
					return;
				components::Transform* t = node->getComponent<Transform>();

				ModelDeferredRenderContext* context = dynamic_cast<ModelDeferredRenderContext*>(renderContextDeferred);
				context->renderShader->use(); //TODO: only call this once!

				context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::modelMatrix, t->globalTransform);
				context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				if (mesh->material.texture)
				{
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::textureFactor, 1.0f);
					mesh->material.texture->bind();
				}
				else
				{
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::textureFactor, 0.0f);
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::diffuseColor, mesh->material.color.diffuse);
				}

				context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::shinyness, mesh->material.color.shinyness);

				glActiveTexture(GL_TEXTURE1);
				if (mesh->material.normalmap)
					mesh->material.normalmap->bind();
				else
					context->defaultNormalMap->bind();

				glActiveTexture(GL_TEXTURE2);
				if (mesh->material.specularmap)
					mesh->material.specularmap->bind();
				else
					context->white->bind();
				glActiveTexture(GL_TEXTURE0);


				if (vao)
				{
					vao->bind();
					glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
					Renderer::drawCalls++;
					vao->unBind();
				}
			}

			void MeshRenderer::drawForwardPass()
			{
				if (useDeferred)
					return;
				components::Transform* t = node->getComponent<Transform>();

				ModelForwardRenderContext* context = dynamic_cast<ModelForwardRenderContext*>(renderContextForward);
				context->renderShader->use(); //TODO: only call this once!

				context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::modelMatrix, t->globalTransform);
				context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				if (mesh->material.texture)
				{
					context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::textureFactor, 1.0f);
					mesh->material.texture->bind();
				}
				else
				{
					context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::textureFactor, 0.0f);
					context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::diffuseColor, mesh->material.color.diffuse);
				}

				glActiveTexture(GL_TEXTURE1);
				if (mesh->material.normalmap)
					mesh->material.normalmap->bind();
				else
					context->defaultNormalMap->bind();
				glActiveTexture(GL_TEXTURE0);


				if (vao)
				{
					vao->bind();
					glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
					Renderer::drawCalls++;
					vao->unBind();
				}
			}

			void MeshRenderer::drawShadowMap()
			{
				if (!castShadow || !mesh || !visible)
					return;
				if (!cullBackFaces)
					glDisable(GL_CULL_FACE);
				components::Transform* t = node->getComponent<Transform>();
				ModelShadowRenderContext* context = dynamic_cast<ModelShadowRenderContext*>(renderContextShadow);
				context->renderShader->use(); //TODO: only call this once!
				context->renderShader->setUniform(ModelShadowRenderContext::RenderUniform::modelMatrix, t->globalTransform);

				vao->bind();
				glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
				Renderer::drawCalls++;


				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);

				//TODO
			}


			void MeshRenderer::ModelDeferredRenderContext::init()
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
				renderShader->registerUniform(RenderUniform::s_specularmap, "s_specularmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->registerUniform(RenderUniform::shinyness, "shinyness");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_specularmap, 2);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");
				white = vrlib::Texture::loadCached("data/vrlib/tien/textures/white.png");

			}

			void MeshRenderer::ModelDeferredRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}




			MeshRenderer::ModelForwardRenderContext::ModelForwardRenderContext(const std::string & shader)
			{
				shaderFile = shader;
				if (shaderFile == "")
					shaderFile = "data/vrlib/tien/shaders/ModelRenderer.forward";

			}

			void MeshRenderer::ModelForwardRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>(shaderFile + ".vert", shaderFile + ".frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->link();
				renderShader->bindFragLocation("fragColor", 0);
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
				renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
				renderShader->registerUniform(RenderUniform::s_normalmap, "s_normalmap");
				renderShader->registerUniform(RenderUniform::s_specularmap, "s_specularmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->registerUniform(RenderUniform::shinyness, "shinyness");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_specularmap, 2);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void MeshRenderer::ModelForwardRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
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

			MeshRenderer::Mesh::Mesh(const nlohmann::json &data)
			{
				guid = data["id"].get<std::string>();

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

				if (data.find("material") != data.end())
				{
					if (data["material"].find("diffuse") != data["material"].end())
						material.texture = vrlib::Texture::loadCached(data["material"]["diffuse"]);
					if (data["material"].find("normal") != data["material"].end())
						material.normalmap = vrlib::Texture::loadCached(data["material"]["normal"]);
					if (data["material"].find("specular") != data["material"].end())
						material.specularmap = vrlib::Texture::loadCached(data["material"]["specular"]);
					if (data["material"].find("color") != data["material"].end())
					{
						for (int i = 0; i < 3; i++)
						{
							material.color.ambient[i] = data["material"]["color"]["ambient"][i];
							material.color.diffuse[i] = data["material"]["color"]["diffuse"][i];
							material.color.specular[i] = data["material"]["color"]["specular"][i];
						}
						material.color.shinyness = data["material"]["color"]["shinyness"];
					}
				}
			}

			nlohmann::json MeshRenderer::Mesh::toJson()
			{
				nlohmann::json ret;
				ret["id"] = guid;

				for (auto i : indices)
					ret["indices"].push_back((int)i);

				for (auto &v : vertices)
				{
					nlohmann::json vv;
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

				ret["material"] = nlohmann::json::object();

				if(material.texture && material.texture->image)
					ret["material"]["diffuse"] = material.texture->image->fileName;
				if (material.normalmap && material.normalmap->image)
					ret["material"]["normal"] = material.normalmap->image->fileName;
				if (material.specularmap && material.specularmap->image)
					ret["material"]["specular"] = material.normalmap->image->fileName;

				for (int i = 0; i < 3; i++)
				{
					ret["material"]["color"]["ambient"].push_back(material.color.ambient[i]);
					ret["material"]["color"]["diffuse"].push_back(material.color.diffuse[i]);
					ret["material"]["color"]["specular"].push_back(material.color.specular[i]);
				}
				ret["material"]["color"]["shinyness"] = material.color.shinyness;

				return ret;
			}

			void MeshRenderer::Mesh::collisionFractions(const vrlib::math::Ray & ray, std::function<bool(float)> callback)
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
							if (!callback(f))
								return;
				}
			}




			MeshRenderer::Cube::Cube()
			{
				//top/bottom
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, .5f, -.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, .5f, .5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, .5f, .5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, .5f, -.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 1)));

				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, -.5f, -.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, -.5f, .5f), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, -.5f, .5f), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, -.5f, -.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 1)));
				//sides
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, -.5f, -.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, -.5f, .5f), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, .5f, .5f), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, .5f, -.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 1, 0)));

				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, -.5f, -.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, -.5f, .5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, .5f, .5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, .5f, -.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 1, 0)));
				//front/back
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, -.5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, -.5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, .5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, .5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0, 1, 0)));

				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, -.5f, .5f), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, -.5f, .5f), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, .5f, .5f), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(0, 1, 0)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, .5f, .5f), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0, 1, 0)));

				indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });
				indices.insert(indices.end(), { 6, 5, 4, 7, 6, 4 });

				indices.insert(indices.end(), { 10, 9, 8, 11, 10, 8 });
				indices.insert(indices.end(), { 12, 13, 14, 12, 14, 15 });

				indices.insert(indices.end(), { 18, 17, 16, 19, 18, 16 });
				indices.insert(indices.end(), { 20, 21, 22, 20, 22, 23 });
			}

			MeshRenderer::Plane::Plane()
			{
				//top/bottom
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, 0, -.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(-.5f, 0, .5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, 0, .5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 1)));
				vertices.push_back(vrlib::gl::VertexP3N2B2T2T2(glm::vec3(.5f, 0, -.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 1)));
				indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });
			}
			MeshRenderer::Sphere::Sphere()
			{
				int stacks = 10;
				int slices = 10;

				float thetaInc = glm::pi<float>() / stacks;
				float phiInc = 2 * glm::pi<float>() / slices;

				for (float theta = -glm::half_pi<float>(); theta < glm::half_pi<float>(); theta += thetaInc)
				{
					for (float phi = 0; phi < glm::two_pi<float>(); phi += phiInc)
					{
						float x1 = glm::cos(theta) * glm::cos(phi);
						float y1 = glm::cos(theta) * glm::sin(phi);
						float z1 = glm::sin(theta);

						float x2 = glm::cos(theta + thetaInc) * glm::cos(phi + phiInc);
						float y2 = glm::cos(theta + thetaInc) * glm::sin(phi + phiInc);
						float z2 = glm::sin(theta + thetaInc);

						float x3 = glm::cos(theta + thetaInc) * glm::cos(phi);
						float y3 = glm::cos(theta + thetaInc) * glm::sin(phi);
						float z3 = glm::sin(theta + thetaInc);

						float x4 = glm::cos(theta) * glm::cos(phi + phiInc);
						float y4 = glm::cos(theta) * glm::sin(phi + phiInc);
						float z4 = glm::sin(theta);

						vrlib::gl::VertexP3N2B2T2T2 v;
						setP3(v, glm::vec3(x1, y1, z1));		setN3(v, glm::vec3(x1, y1, z1));		setT2(v, glm::vec2(asin(x1) / glm::pi<float>() + 0.5f, asin(y1) / glm::pi<float>() + 0.5f));		vrlib::gl::setTan3(v, glm::vec3(0, 1, 0));	 vertices.push_back(v);
						setP3(v, glm::vec3(x2, y2, z2));		setN3(v, glm::vec3(x2, y2, z2));		setT2(v, glm::vec2(asin(x2) / glm::pi<float>() + 0.5f, asin(y2) / glm::pi<float>() + 0.5f));		vrlib::gl::setTan3(v, glm::vec3(0, 1, 0));	vertices.push_back(v);
						setP3(v, glm::vec3(x3, y3, z3));		setN3(v, glm::vec3(x3, y3, z3));		setT2(v, glm::vec2(asin(x3) / glm::pi<float>() + 0.5f, asin(y3) / glm::pi<float>() + 0.5f));		vrlib::gl::setTan3(v, glm::vec3(0, 1, 0));	vertices.push_back(v);
						setP3(v, glm::vec3(x4, y4, z4));		setN3(v, glm::vec3(x4, y4, z4));		setT2(v, glm::vec2(asin(x4) / glm::pi<float>() + 0.5f, asin(y4) / glm::pi<float>() + 0.5f));		vrlib::gl::setTan3(v, glm::vec3(0, 1, 0));	vertices.push_back(v);
					}
				}
				indices.clear();
				for (unsigned short i = 0; i < vertices.size(); i += 4)
				{
					indices.push_back(i + 0);
					indices.push_back(i + 1);
					indices.push_back(i + 2);

					indices.push_back(i + 3);
					indices.push_back(i + 1);
					indices.push_back(i + 0);
				}
			}
		}
	}
}