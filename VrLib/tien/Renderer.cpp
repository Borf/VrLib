#include "Renderer.h"

#include "components/ModelRenderer.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/Light.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace tien
	{

		Renderer::Renderer() : Node("Root", nullptr)
		{
			cameraNode = nullptr;
		}


		void Renderer::setTreeDirty()
		{
			treeDirty = true;
		}


		void Renderer::updateRenderables()
		{
			renderables.clear();
			lights.clear();
			fortree([this](Node* n)
			{
				if (n->getComponent<components::ModelRenderer>())
					renderables.push_back(n);
				if (n->getComponent<components::Light>())
					lights.push_back(n);
			});
		}



		void Renderer::init()
		{
			renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/default.vert", "data/vrlib/tien/shaders/default.frag");
			renderShader->bindAttributeLocation("a_position", 0);
			renderShader->bindAttributeLocation("a_normal", 1);
			renderShader->bindAttributeLocation("a_texcoord", 2);
			renderShader->link();
			renderShader->bindFragLocation("fragColor", 0);
			renderShader->bindFragLocation("fragNormal", 1);
			//shader->bindFragLocation("fragColor", 0);
			renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
			renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
			renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
			renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
			renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
			renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
			renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
			renderShader->use();
			renderShader->setUniform(RenderUniform::s_texture, 0);


			postLightingShader = new vrlib::gl::Shader<PostLightingUniform>("data/vrlib/tien/shaders/postLighting.vert", "data/vrlib/tien/shaders/postLighting.frag");
			postLightingShader->bindAttributeLocation("a_position", 0);
			postLightingShader->link();
			postLightingShader->bindFragLocation("fragColor", 0);
			postLightingShader->registerUniform(PostLightingUniform::windowSize, "windowSize");
			postLightingShader->registerUniform(PostLightingUniform::modelViewMatrix, "modelViewMatrix");
			postLightingShader->registerUniform(PostLightingUniform::projectionMatrix, "projectionMatrix");
			postLightingShader->registerUniform(PostLightingUniform::modelViewMatrixInv, "modelViewMatrixInv");
			postLightingShader->registerUniform(PostLightingUniform::projectionMatrixInv, "projectionMatrixInv");
			postLightingShader->registerUniform(PostLightingUniform::s_color, "s_color");
			postLightingShader->registerUniform(PostLightingUniform::s_normal, "s_normal");
			postLightingShader->registerUniform(PostLightingUniform::s_depth, "s_depth");
			postLightingShader->registerUniform(PostLightingUniform::lightType, "lightType");
			postLightingShader->registerUniform(PostLightingUniform::lightPosition, "lightPosition");
			postLightingShader->registerUniform(PostLightingUniform::lightDirection, "lightDirection");
			postLightingShader->registerUniform(PostLightingUniform::lightRange, "lightRange");
			postLightingShader->registerUniform(PostLightingUniform::lightColor, "lightColor");
			postLightingShader->use();
			postLightingShader->setUniform(PostLightingUniform::s_color, 0);
			postLightingShader->setUniform(PostLightingUniform::s_normal, 1);
			postLightingShader->setUniform(PostLightingUniform::s_depth, 2);
			gbuffers = nullptr;


			skydome = vrlib::Model::getModel<vrlib::gl::VertexP3>("data/vrlib/tien/models/skydome.obj");
			skydomeShader = new vrlib::gl::Shader<SkydomeUniforms>("data/vrlib/tien/shaders/skydome.vert", "data/vrlib/tien/shaders/skydome.frag");
			skydomeShader->bindAttributeLocation("a_position", 0);
			skydomeShader->link();
			skydomeShader->bindFragLocation("fragColor", 0);
			skydomeShader->registerUniform(SkydomeUniforms::projectionMatrix, "projectionMatrix");
			skydomeShader->registerUniform(SkydomeUniforms::modelViewMatrix, "modelViewMatrix");
			skydomeShader->registerUniform(SkydomeUniforms::glow, "glow");
			skydomeShader->registerUniform(SkydomeUniforms::color, "color");
			skydomeShader->registerUniform(SkydomeUniforms::sunDirection, "sunDirection");
			skydomeShader->use();
			skydomeShader->setUniform(SkydomeUniforms::color, 0);
			skydomeShader->setUniform(SkydomeUniforms::glow, 1);
			skydomeColor = vrlib::Texture::loadCached("data/vrlib/tien/Textures/sky.png");
			skydomeColor->setTextureRepeat(false);
			skydomeGlow = vrlib::Texture::loadCached("data/vrlib/tien/Textures/glow.png");
			skydomeGlow->setTextureRepeat(false);

			sun = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/vrlib/tien/models/sun.obj");
			moon = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/vrlib/tien/models/moon.obj");
			billboardShader = new vrlib::gl::Shader<BillboardUniforms>("data/vrlib/tien/shaders/billboard.vert", "data/vrlib/tien/shaders/billboard.frag");
			billboardShader->bindAttributeLocation("a_position", 0);
			billboardShader->bindAttributeLocation("a_texcoord", 2);
			billboardShader->link();
			billboardShader->bindFragLocation("fragColor", 0);
			billboardShader->registerUniform(BillboardUniforms::projectionMatrix, "projectionMatrix");
			billboardShader->registerUniform(BillboardUniforms::mat, "mat");
			billboardShader->registerUniform(BillboardUniforms::s_texture, "s_texture");
			billboardShader->use();
			billboardShader->setUniform(BillboardUniforms::s_texture, 0);



			std::vector<vrlib::gl::VertexP3> verts;
			vrlib::gl::VertexP3 vert;
			vrlib::gl::setP3(vert, glm::vec3(-1, -1, 0));	verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(1, -1, 0));	verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(1, 1, 0));	verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-1, 1,0));	verts.push_back(vert);
			sphere.x = verts.size();
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);

			sphere.y = verts.size();
			overlayVerts = new vrlib::gl::VBO<vrlib::gl::VertexP3>();
			overlayVerts->setData(verts.size(), verts.data(), GL_STATIC_DRAW);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			overlayVao = new vrlib::gl::VAO<vrlib::gl::VertexP3>(overlayVerts);

			mHead.init("MainUserHead");

		}

		void Renderer::update(float elapsedTime)
		{
			if (treeDirty)
			{
				updateRenderables();
				if (!cameraNode)
					cameraNode = findNodeWithComponent<components::Camera>();
				treeDirty = false;
			}

			fortree([this, &elapsedTime](Node* n)
			{
				for (Component* c : n->components)
					c->update(elapsedTime);
			});

			//TODO: update transform matrices

			std::function<void(Node*, const glm::mat4 &)> updateTransforms;
			updateTransforms = [this, &updateTransforms](Node* n, const glm::mat4 &parentTransform)
			{
				components::Transform* transform = n->getComponent<components::Transform>();
				if (transform)
				{
					transform->buildTransform();
					transform->globalTransform = parentTransform * transform->transform;
					for (auto c : n->children)
						updateTransforms(c, transform->globalTransform);
				}
				else
					for (auto c : n->children)
						updateTransforms(c, parentTransform);
			};
			updateTransforms(this, glm::mat4());
		}


		void Renderer::render(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
		{
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			if(!gbuffers)
				gbuffers = new vrlib::gl::FBO(viewport[2] - viewport[0], viewport[3] - viewport[1], true, vrlib::gl::FBO::Color, vrlib::gl::FBO::Normal);


			components::Camera* camera = cameraNode->getComponent<components::Camera>();

			renderShader->use();
			renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
			renderShader->setUniform(RenderUniform::viewMatrix, modelViewMatrix);
			renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
			renderShader->setUniform(RenderUniform::textureFactor, 1.0f);

			gbuffers->bind();
			glViewport(0, 0, gbuffers->getWidth(), gbuffers->getHeight());
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_BLEND);

			for (Node* c : renderables)
			{
				components::ModelRenderer* m = c->getComponent<components::ModelRenderer>();
				components::Transform* t = c->getComponent<components::Transform>();

				m->model->draw([this, t](const glm::mat4 &modelMatrix)
				{
					renderShader->setUniform(RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
					renderShader->setUniform(RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform * modelMatrix))));
				},
				[this](const vrlib::Material &material)
				{
					if (material.texture)
					{
						renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
						material.texture->bind();
					}
					else
					{
						renderShader->setUniform(RenderUniform::textureFactor, 0.0f);
						renderShader->setUniform(RenderUniform::diffuseColor, material.color.diffuse);
					}
				});
			}
			gbuffers->unbind();


			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffers->fboId);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, gbuffers->getWidth(), gbuffers->getHeight(),
				viewport[0], viewport[1], viewport[2], viewport[3],
				GL_DEPTH_BUFFER_BIT, GL_NEAREST);



			gbuffers->use();
			postLightingShader->use();
			postLightingShader->setUniform(PostLightingUniform::windowSize, glm::vec2(viewport[2] - viewport[0], viewport[3] - viewport[1]));
			postLightingShader->setUniform(PostLightingUniform::projectionMatrix, projectionMatrix);
			postLightingShader->setUniform(PostLightingUniform::projectionMatrixInv, glm::inverse(projectionMatrix));
			postLightingShader->setUniform(PostLightingUniform::modelViewMatrixInv, glm::inverse(modelViewMatrix));


			overlayVao->bind();
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			glDepthMask(GL_FALSE);

			for (Node* c : lights)
			{
				components::Light* l = c->getComponent<components::Light>();
				components::Transform* t = c->getComponent<components::Transform>();
				glm::vec3 pos(t->globalTransform * glm::vec4(0, 0, 0, 1));

				if(l->type == components::Light::Type::directional)
					glDisable(GL_DEPTH_TEST);
				else
					glEnable(GL_DEPTH_TEST);

				postLightingShader->setUniform(PostLightingUniform::modelViewMatrix, glm::scale(glm::translate(modelViewMatrix, pos), glm::vec3(l->range, l->range, l->range)));
				postLightingShader->setUniform(PostLightingUniform::lightType, (int)l->type);
				postLightingShader->setUniform(PostLightingUniform::lightPosition, pos);
				postLightingShader->setUniform(PostLightingUniform::lightRange, l->range);
				postLightingShader->setUniform(PostLightingUniform::lightColor, l->color);
				//todo: only draw volumes for point lights
				if(l->type == components::Light::Type::directional)
					glDrawArrays(GL_QUADS, 0, 4);
				else
					glDrawArrays(GL_TRIANGLES, sphere.x, sphere.y-sphere.x);
				glEnable(GL_BLEND);
			}
			
			glDepthMask(GL_TRUE);

			skydomeShader->use();
			skydomeShader->setUniform(SkydomeUniforms::projectionMatrix, projectionMatrix);
			skydomeShader->setUniform(SkydomeUniforms::modelViewMatrix, glm::scale(modelViewMatrix, glm::vec3(4.0,4.0,4.0)));

			glActiveTexture(GL_TEXTURE1);
			skydomeGlow->bind();
			glActiveTexture(GL_TEXTURE0);
			skydomeColor->bind();


			static float now = 0;
			now += 0.000125f;

			glm::vec3 sunDirection(0, cos(now), sin(now));

			//lights.front()->getComponent<components::Transform>()->position = sunDirection;
			//glm::vec3 sunDirection(0, 1, -1);
			sunDirection = glm::normalize(sunDirection);

			skydomeShader->setUniform(SkydomeUniforms::sunDirection, sunDirection);

			float k = float(glm::max(+glm::cos(glm::radians(now)), 0.0f));
			float b = float(glm::max(-glm::cos(glm::radians(now)), 0.0f));

			float A[4] = { 0.5f * k,
				0.5f * k,
				0.7f * k, 0.0f };

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			skydome->draw([](const glm::mat4 &mat) {});

			glm::vec3 cameraPos(modelViewMatrix * glm::vec4(0, 0, 0, 1));
			glm::vec3 pos = 4*45.0f * sunDirection;


			glDisable(GL_CULL_FACE);
			billboardShader->use();
			billboardShader->setUniform(BillboardUniforms::projectionMatrix, projectionMatrix);
			billboardShader->setUniform(BillboardUniforms::mat, glm::scale(glm::inverse(glm::lookAt(pos, cameraPos, glm::vec3(0, 1, 0))), glm::vec3(5,5,5)));
			sun->draw([](const glm::mat4 &mat) {}, [this](const Material& material) {
				material.texture->bind();
			});

			billboardShader->setUniform(BillboardUniforms::mat, glm::scale(glm::inverse(glm::lookAt(-pos, cameraPos, glm::vec3(0, 1, 0))), glm::vec3(15, 15, 15)));
			moon->draw([](const glm::mat4 &mat) {}, [this](const Material& material) {
				material.texture->bind();
			});

			//camera->target->bind();

			//camera->target->unbind();

			glDisable(GL_BLEND);

		}


	}
}