#pragma once

#include "../Component.h"
#include <string>
#include <map>
namespace vrlib
{
	class Model;
	namespace tien
	{
		namespace components
		{
			class ModelRenderer : public Component
			{
			private:
				static std::map<std::string, vrlib::Model*> cache;
			public:
				ModelRenderer(const std::string &fileName);
				~ModelRenderer();

				vrlib::Model* model;

				bool castShadow;
			};
		}
	}
}