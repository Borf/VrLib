#pragma once

#include "ContainerComponent.h"
#include <VrLib/json.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Panel : public ContainerComponent
			{
			public:
				Panel();
				Panel(const std::string &jsonFileName);

				virtual void draw(const glm::mat4 &parentMatrix) override;
				void loadJson(const nlohmann::json &config);

				virtual void foreachWithMatrix(const std::function<void(const glm::mat4 &matrix, Component*) > &callback, const glm::mat4 &parentMatrix = glm::mat4()) override;


			};
		}
	}
}