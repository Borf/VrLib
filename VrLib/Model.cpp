#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "Model.h"
#include "models/SimpleModel.h"
#include "models/AssimpModel.h"
#include "models/LoLModel.h"

#include <VrLib/math/Ray.h>
#include "Log.h"
#include <algorithm>
#include <climits>

namespace vrlib
{
	std::vector<glm::vec3> CollisionMesh::collisions(const vrlib::math::Ray & ray)
	{
		std::vector<float> col = collisionFractions(ray);
		std::vector<glm::vec3> result;
		for (auto c : col)
			result.push_back(ray.mOrigin + c * ray.mDir);
		return result;
	}


	template<class VertexFormat>
	Model* Model::getModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
#ifdef _DEBUG
		logger << "Loading " << fileName << Log::newline;
#endif
		std::string extension = fileName;
		if(extension.find(".") != std::string::npos)
			extension = extension.substr(extension.rfind("."));
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == ".shape")
		{
			SimpleModel<VertexFormat>* m = new SimpleModel<VertexFormat>(fileName, options);
			return m;
		}
		else if (extension == ".skn")
		{
			return new LoLModel<VertexFormat>(fileName, options);
		}
		else
		{
			AssimpModel<VertexFormat>* m = new AssimpModel<VertexFormat>(fileName, options);
			if (!m->vao)
			{
				delete m;
				m = NULL;
			}
			return m;
		}



		logger << "Error loading model " << fileName << ", file extension not known ("<<extension<<")" << Log::newline;
		return NULL;
	}




	ModelInstance::ModelInstance(Model* model)
	{
		this->model = model;
	}
	void ModelInstance::draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<bool(const Material&)> &materialCallback)
	{
		model->draw(modelviewMatrixCallback, materialCallback);
	}



	template<class VertexFormat>
	void vrlib::Model::handleModelLoadOptions(std::vector<VertexFormat> &vertices, const ModelLoadOptions &options)
	{
		calculateAABB(getTriangles());
		if (options.size > 0)
			scaleToSize(vertices, options.size);
		if ((options.options & ModelLoadOptions::RepositionToCenter) != 0)
			recenterToCenter(vertices);
		if ((options.options & ModelLoadOptions::RepositionToCenterBottom) != 0)
			recenterToCenterBottom(vertices);
		calculateAABB(getTriangles());
	}




	void Model::calculateAABB(const std::vector<glm::vec3> &verts)
	{
		aabb.bounds[0] = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		aabb.bounds[1] = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);

		for (const glm::vec3 &v : verts)
		{
			aabb.bounds[0] = glm::min(aabb.bounds[0], v);
			aabb.bounds[1] = glm::max(aabb.bounds[1], v);
		}
	}


	template<class VertexFormat>
	void vrlib::Model::scaleToSize(std::vector<VertexFormat> &vertices, float maxSize)
	{
		float fac = 0;
		glm::vec3 diff = aabb.bounds[1] - aabb.bounds[0];
		fac = glm::max(fac, diff.x);
		fac = glm::max(fac, diff.y);
		fac = glm::max(fac, diff.z);

		fac = fac / maxSize;

		for (VertexFormat& v : vertices)
		{
			v.px /= fac;
			v.py /= fac;
			v.pz /= fac;
		}
//		calculateAABB(getTriangles());
	}




	template<class VertexFormat>
	void vrlib::Model::recenterToCenter(std::vector<VertexFormat> &vertices)
	{
		glm::vec3 diff = (aabb.bounds[1] - aabb.bounds[0]) / 2.0f;

		for (VertexFormat& v : vertices)
		{
			v.px = v.px - aabb.bounds[0].x - diff.x;
			v.py = v.py - aabb.bounds[0].y - diff.y;
			v.pz = v.pz - aabb.bounds[0].z - diff.z;
		}
		calculateAABB(getTriangles());
	}
	template<class VertexFormat>
	void Model::recenterToCenterBottom(std::vector<VertexFormat> &vertices)
	{
		glm::vec3 diff = (aabb.bounds[1] - aabb.bounds[0]) / 2.0f;

		for (VertexFormat& v : vertices)
		{
			v.px = v.px - aabb.bounds[0].x - diff.x;
			v.py = v.py - aabb.bounds[0].y;
			v.pz = v.pz - aabb.bounds[0].z - diff.z;
		}
		calculateAABB(getTriangles());
	}

	std::vector<float> Model::collisionFractions(const math::Ray &ray)
	{
		std::vector<float> result;
		if (!aabb.hasRayCollision(ray, 0, 10000))
			return result;
		std::vector<glm::vec3> verts = getTriangles();

		float f = 0;
		for (size_t i = 0; i < verts.size(); i += 3)
			if (ray.LineIntersectPolygon(&verts[i], 3, f))
				if(f > 0)
					result.push_back(f);
		return result;
	}


#define prototypes(x) \
	template Model* Model::getModel< gl::x >(const std::string &fileName, const ModelLoadOptions &options); \
	template void Model::handleModelLoadOptions< gl::x >(std::vector<gl::x> &vertices, const ModelLoadOptions &options); \
	template void Model::scaleToSize< gl::x >(std::vector<gl::x> &vertices, float maxSize); \
	template void Model::recenterToCenter< gl::x >(std::vector<gl::x> &vertices);


	prototypes(VertexP3);
	prototypes(VertexP3T2);
	prototypes(VertexP3N3);
	prototypes(VertexP3N3T2);
	prototypes(VertexP3N3T2B4B4);
	prototypes(VertexP3N2B2T2T2);
	prototypes(VertexP3N2B2T2T2B4B4);

	
}