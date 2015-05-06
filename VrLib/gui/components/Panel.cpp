#include "Panel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <fstream>

#include <VrLib/gui/Window.h>
#include <VrLib/gl/shader.h>
#include <VrLib/Model.h>
#include <VrLib/json.h>
#include <VrLib/Log.h>

#include <VrLib/gui/components/Button.h>
#include <VrLib/gui/components/CheckBox.h>
#include <VrLib/gui/components/Slider.h>
#include <VrLib/gui/components/Image.h>
#include <VrLib/gui/components/Label.h>


namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			Panel::Panel()
			{

			}

			Panel::Panel(const std::string &jsonFileName)
			{
				json::Value config = json::readJson(std::ifstream(jsonFileName.c_str()));
				loadJson(config);
			}


			void Panel::loadJson(const json::Value &config)
			{
				for (const json::Value &element : config)
				{
					Component* component = NULL;
					if (element["type"] == "button")
						component = new Button(element["value"].asString());
					else if (element["type"] == "checkbox")
					{
						component = new CheckBox();
						if (element.isMember("value"))
							((CheckBox*)component)->value = element["value"];
					}
					else if (element["type"] == "panel")
					{
						component = new Panel();
						if (element.isMember("components"))
							((Panel*)component)->loadJson(element["components"]);

					}
					else
						logger << "Unknown panel element type: " << element["type"].asString() << Log::newline;


					if (component)
					{
						if (element.isMember("position"))
							component->position = glm::vec2(element["position"][0], element["position"][1]);
						if (element.isMember("size"))
							component->size = glm::vec2(element["size"][0], element["size"][1]);
						if (element.isMember("name"))
							component->name = element["name"];

						push_back(component);
					}
				}
			}


			void Panel::draw(const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position, 0));
				matrix = glm::scale(matrix, glm::vec3(((Component*)this)->size, 1));


			/*	vrlib::gui::Window::panelModel->draw([this, &matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix * mat);
				});*/


				ContainerComponent::draw(glm::translate(parentMatrix, glm::vec3(position, Window::thickness)));
			}

		}
	}
}
