#define NOMINMAX
#include "Model.h"
#include "models/SimpleModel.h"
#include "models/AssimpModel.h"
#include "models/LoLModel.h"
#include "Log.h"
#include <algorithm>
#include <climits>

namespace vrlib
{
	template<class VertexFormat>
	Model* Model::getModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
#ifdef _DEBUG
		logger << "Loading " << fileName << Log::newline;
#endif
		std::string extension = fileName;
		extension = extension.substr(extension.rfind("."));
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == ".shape")
		{
			return new SimpleModel<VertexFormat>(fileName, options);
		}
		else if (extension == ".skn")
		{
			return new LoLModel<VertexFormat>(fileName, options);
		}
		else
			return new AssimpModel<VertexFormat>(fileName, options);



		logger << "Error loading model " << fileName << ", file extension not known ("<<extension<<")" << Log::newline;
		return NULL;
	}




	ModelInstance::ModelInstance(Model* model)
	{
		this->model = model;
	}
	void ModelInstance::draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const Material&)> &materialCallback)
	{
		model->draw(modelviewMatrixCallback, materialCallback);
	}



	template<class VertexFormat>
	void vrlib::Model::handleModelLoadOptions(std::vector<VertexFormat> &vertices, const ModelLoadOptions &options)
	{
		if (options.size > 0)
			scaleToSize(vertices, options.size);
		if ((options.options & ModelLoadOptions::RepositionToCenter) != 0)
			recenterToCenter(vertices);
		if ((options.options & ModelLoadOptions::RepositionToCenterBottom) != 0)
			recenterToCenterBottom(vertices);
		calculateAABB(getVertices(100));
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
		calculateAABB(getVertices(100));
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
		calculateAABB(getVertices(100));
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
		calculateAABB(getVertices(100));
	}






	template Model* Model::getModel< gl::VertexP3 >(const std::string &fileName, const ModelLoadOptions &options);
	template Model* Model::getModel< gl::VertexP3N3 >(const std::string &fileName, const ModelLoadOptions &options);
	template Model* Model::getModel< gl::VertexP3N3T2 >(const std::string &fileName, const ModelLoadOptions &options);

	template void Model::handleModelLoadOptions< gl::VertexP3 >(std::vector<gl::VertexP3> &vertices, const ModelLoadOptions &options);
	template void Model::handleModelLoadOptions< gl::VertexP3N3 >(std::vector<gl::VertexP3N3> &vertices, const ModelLoadOptions &options);
	template void Model::handleModelLoadOptions< gl::VertexP3N3T2 >(std::vector<gl::VertexP3N3T2> &vertices, const ModelLoadOptions &options);

	template void Model::scaleToSize< gl::VertexP3 >(std::vector<gl::VertexP3> &vertices, float maxSize);
	template void Model::scaleToSize< gl::VertexP3N3 >(std::vector<gl::VertexP3N3> &vertices, float maxSize);
	template void Model::scaleToSize< gl::VertexP3N3T2 >(std::vector<gl::VertexP3N3T2> &vertices, float maxSize);

	template void Model::recenterToCenter< gl::VertexP3 >(std::vector<gl::VertexP3> &vertices);
	template void Model::recenterToCenter< gl::VertexP3N3 >(std::vector<gl::VertexP3N3> &vertices);
	template void Model::recenterToCenter< gl::VertexP3N3T2 >(std::vector<gl::VertexP3N3T2> &vertices);

	template void Model::recenterToCenterBottom< gl::VertexP3 >(std::vector<gl::VertexP3> &vertices);
	template void Model::recenterToCenterBottom< gl::VertexP3N3 >(std::vector<gl::VertexP3N3> &vertices);
	template void Model::recenterToCenterBottom< gl::VertexP3N3T2 >(std::vector<gl::VertexP3N3T2> &vertices);
}