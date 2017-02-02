#include "Panel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <fstream>

#include <VrLib/gui/Window.h>
#include <VrLib/gl/shader.h>
#include <VrLib/Model.h>
#include <VrLib/json.hpp>
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
				std::ifstream file(jsonFileName.c_str());
				json config;
				file>>config;
				loadJson(config);
			}


			void Panel::loadJson(const json &config)
			{
				for (const json &element : config)
				{
					Component* component = NULL;
					if (element["type"] == "button")
						component = new Button(element["value"].get<std::string>());
					else if (element["type"] == "checkbox")
					{
						component = new CheckBox();
						if (element.find("value") != element.end())
							((CheckBox*)component)->value = element["value"];
					}
					else if (element["type"] == "panel")
					{
						component = new Panel();
						if (element.find("components") != element.end())
							((Panel*)component)->loadJson(element["components"]);
					}
					else if (element["type"] == "label")
					{
						component = new Label(element["value"].get<std::string>());
					}
					else if (element["type"] == "slider")
						component = new Slider(element["min"], element["max"], element["value"]);
					else
						logger << "Unknown panel element type: " << element["type"] << Log::newline;


					if (component)
					{
						if (element.find("position") != element.end())
							component->position = glm::vec2(element["position"][0], element["position"][1]);
						if (element.find("size") != element.end())
							component->size = glm::vec2(element["size"][0], element["size"][1]);
						if (element.find("name") != element.end())
							component->name = element["name"].get<std::string>();

						push_back(component);
					}
				}
			}


			void Panel::draw(const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position, 0));
				matrix = glm::scale(matrix, glm::vec3(((Component*)this)->size, 1));


				vrlib::gui::Window::panelModel->draw([this, &matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix * mat);
				});


				ContainerComponent::draw(glm::translate(parentMatrix, glm::vec3(0,0, Window::thickness)));
			}

			void Panel::foreachWithMatrix(const std::function<void(const glm::mat4 &matrix, Component*) > &callback, const glm::mat4 &parentMatrix /*= glm::mat4()*/)
			{
				ContainerComponent::foreachWithMatrix(callback, glm::translate(parentMatrix, glm::vec3(0, 0, Window::thickness)));
			}
		}
	}
}
