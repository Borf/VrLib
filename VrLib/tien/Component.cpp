#include "Component.h"

namespace vrlib
{
	namespace tien
	{
		void Component::buildEditor(EditorBuilder * builder)
		{
			std::string componentName = typeid(*this).name();
			if (componentName.substr(0, 31) == "class vrlib::tien::components::") //TODO: a nicer way to do this
				componentName = componentName.substr(31);

			builder->addTitle("Generic Component: " + componentName);
		}
	}
}