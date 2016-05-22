#pragma once

#include "../Component.h"
#include <string>

namespace vrlib
{
	class Model;
	namespace tien
	{
		namespace components
		{
			class ModelRenderer : public Component
			{
			public:
				ModelRenderer(const std::string &fileName);
				~ModelRenderer();

				vrlib::Model* model;

				bool castShadow;
			};
		}
	}
}