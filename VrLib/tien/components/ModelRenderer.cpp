#include "ModelRenderer.h"
#include <vrlib/Model.h>
#include <vrlib/gl/Vertex.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			ModelRenderer::ModelRenderer(const std::string &fileName)
			{
				model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T3T2>(fileName);
			}

			ModelRenderer::~ModelRenderer()
			{

			}
		}
	}
}