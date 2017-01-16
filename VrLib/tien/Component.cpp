#include "Component.h"

#include <VrLib/json.h>
#include "Node.h"
#include <sstream>

namespace vrlib
{
	namespace tien
	{
		json::Value Component::toJson(json::Value &meshes) const
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