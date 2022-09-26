#include <VrLib/drivers/Sim2dInput.h>
#include <VrLib/drivers/Keyboard.h>
#include <VrLib/Log.h>
#include <VrLib/json.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace vrlib
{
	DeviceDriverAdaptor* Sim2dInputDeviceDriver::getAdaptor(std::string options)
	{
		//options contains the name. Actual configuration of what keys do what should isn't done here
		return new Sim2dInputDeviceDriverAdaptor(this, options);
	}


	Sim2dInputDeviceDriver::Sim2dInputDeviceDriver(const nlohmann::json &config)
	{


		for (nlohmann::json::const_iterator it = config.cbegin(); it != config.cend(); it++)
		{
			std::string key = it.key();
			const nlohmann::json &value = it.value();

			static struct ActionMapping { std::string str; Action action; } actionMapping[] = {
				{ "xneg", X_NEG },
				{ "xpos", X_POS },
				{ "yneg", Y_NEG },
				{ "ypos", Y_POS }
			};
			for (int ii = 0; ii < sizeof(actionMapping) / sizeof(ActionMapping); ii++)
				if (value.find(actionMapping[ii].str) != value.end())
					keyHandlers[key].push_back(keyhandler(actionMapping[ii].action, KeyboardDeviceDriver::parseString(value[actionMapping[ii].str])));
		}
	}


	void Sim2dInputDeviceDriver::update(KeyboardDeviceDriver* keyboardDriver, double elapsedTime)
	{
		float speed = (float)(0.1 * elapsedTime);
		for (std::map<std::string, std::list<keyhandler> >::iterator it = keyHandlers.begin(); it != keyHandlers.end(); it++)
		{
			data[it->first].position = glm::vec2(0, 0);
			for (std::list<keyhandler>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				keyhandler& kh = *it2;

				if (keyboardDriver->isPressed(kh.second.first) && keyboardDriver->isModPressed(kh.second.second))
				{
					switch (kh.first)
					{
					case X_NEG: data[it->first].position.x = -1;  break;
					case X_POS: data[it->first].position.x = 1; break;
					case Y_NEG: data[it->first].position.y = -1; break;
					case Y_POS: data[it->first].position.y = 1; break;
					}
				}

			}
		}



	}


	Sim2dInputDeviceDriver::Sim2dInputDeviceDriverAdaptor::Sim2dInputDeviceDriverAdaptor(Sim2dInputDeviceDriver* driver, std::string name)
	{
		this->driver = driver;
		this->name = name;
		if (driver->data.find(name) == driver->data.end())
		{
			driver->data[name] = SimPosData();
		}
	}

	glm::vec2 Sim2dInputDeviceDriver::Sim2dInputDeviceDriverAdaptor::getData()
	{
		return driver->data[name].position;
	}

}