#include "Component.h"

#include <VrLib/json.hpp>
#include "Node.h"
#include <sstream>

namespace vrlib
{
	namespace tien
	{
		nlohmann::json Component::toJson(nlohmann::json &meshes) const
		{ 
			throw "Cannot serialize..."; 
		};


		void Component::buildEditor(EditorBuilder * builder, bool folded)
		{
			std::string componentName = typeid(*this).name();
			if (componentName.substr(0, 31) == "class vrlib::tien::components::") //TODO: a nicer way to do this
				componentName = componentName.substr(31);

			builder->addTitle("Generic Component: " + componentName);
		}

		std::string EditorBuilder::toString(float value) const
		{
			std::ostringstream ss;
			ss << value;
			return ss.str();
		}


	}
}