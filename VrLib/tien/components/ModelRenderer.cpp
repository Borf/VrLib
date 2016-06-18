#include "ModelRenderer.h"
#include <vrlib/Model.h>
#include <vrlib/gl/Vertex.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			std::map<std::string, vrlib::Model*> ModelRenderer::cache;

			ModelRenderer::ModelRenderer(const std::string &fileName)
			{
				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2>(fileName);
				model = cache[fileName];
			}

			ModelRenderer::~ModelRenderer()
			{

			}
		}
	}
}