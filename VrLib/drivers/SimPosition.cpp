#include <VrLib/drivers/SimPosition.h>
#include <VrLib/drivers/Keyboard.h>
#include <VrLib/Log.h>
#include <VrLib/json.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace vrlib
{
	DeviceDriverAdaptor* SimPositionDeviceDriver::getAdaptor(std::string options)
	{
		//options contains the name. Actual configuration of what keys do what should isn't done here
		return new SimPositionDeviceDriverAdaptor(this, options);
	}


	SimPositionDeviceDriver::SimPositionDeviceDriver(const nlohmann::json &config)
	{
		mouseOffsetX = 0;
		mouseOffsetY = 0;


		for (nlohmann::json::const_iterator it = config.cbegin(); it != config.cend(); it++)
		{
			std::string key = it.key();
			const nlohmann::json &value = it.value();

			if (value.find("initial") != value.end())
				data[key].position = glm::vec3(value["initial"][0], value["initial"][1], value["initial"][2]);

			if (value.find("camera") != value.end())
				if (value["camera"].get<bool>())
					data[key].isCamera = true;


			static struct ActionMapping { std::string str; Action action; } actionMapping[] = {
				{ "xneg", TRANS_X_NEG },
				{ "xpos", TRANS_X_POS },
				{ "yneg", TRANS_Y_NEG },
				{ "ypos", TRANS_Y_POS },
				{ "zneg", TRANS_Z_NEG },
				{ "zpos", TRANS_Z_POS },

				{ "rotxneg", ROT_X_NEG },
				{ "rotxpos", ROT_X_POS },
				{ "rotyneg", ROT_Y_NEG },
				{ "rotypos", ROT_Y_POS },
				{ "rotzneg", ROT_Z_NEG },
				{ "rotzpos", ROT_Z_POS },
			};
			for (int ii = 0; ii < sizeof(actionMapping) / sizeof(ActionMapping); ii++)
				if (value.find(actionMapping[ii].str) != value.end())
					keyHandlers[key].push_back(keyhandler(actionMapping[ii].action, KeyboardDeviceDriver::parseString(value[actionMapping[ii].str])));
		}
	}


	void SimPositionDeviceDriver::update(KeyboardDeviceDriver* keyboardDriver, double elapsedTime)
	{
		if (mouseOffsetX != 0 || mouseOffsetY != 0)
		{
			for (std::map<std::string, SimPosData>::iterator it = data.begin(); it != data.end(); it++)
			{
				if (it->second.isCamera)
				{
					data[it->first].rotation = it->second.rotation * glm::quat(glm::vec3(0, .01f * mouseOffsetX, 0));
					data[it->first].rotation = glm::quat(glm::vec3(.01f * mouseOffsetY, 0, 0)) * it->second.rotation;

				}
			}
			mouseOffsetX = 0;
			mouseOffsetY = 0;
		}

		for (std::map<std::string, std::list<keyhandler> >::iterator it = keyHandlers.begin(); it != keyHandlers.end(); it++)
		{
			float speed = (float)(0.05 * elapsedTime);
			if (it->first == "Wand" || it->first == "Wand2")
				speed *= .333f;

			for (std::list<keyhandler>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				keyhandler& kh = *it2;

				if (keyboardDriver->isPressed(kh.second.first) && keyboardDriver->isModPressed(kh.second.second))
				{
					switch (kh.first)
					{
					case TRANS_X_NEG: data[it->first].position += speed * (data[it->first].isCamera ? (glm::vec3(-0.1f, 0, 0) * data[it->first].rotation) : glm::vec3(-0.1f, 0, 0)); break;
					case TRANS_X_POS: data[it->first].position += speed * (data[it->first].isCamera ? (glm::vec3(0.1f, 0, 0) * data[it->first].rotation) : glm::vec3(0.1f, 0, 0)); break;
					case TRANS_Y_NEG: data[it->first].position += speed * (data[it->first].isCamera ? (glm::vec3(0, -0.1f, 0) * data[it->first].rotation) : glm::vec3(0, -0.1f, 0)); break;
					case TRANS_Y_POS: data[it->first].position += speed * (data[it->first].isCamera ? (glm::vec3(0, 0.1f, 0) * data[it->first].rotation) : glm::vec3(0, 0.1f, 0)); break;
					case TRANS_Z_NEG: data[it->first].position += speed * (data[it->first].isCamera ? (glm::vec3(0, 0, -0.1f) * data[it->first].rotation) : glm::vec3(0, 0, -0.1f)); break;
					case TRANS_Z_POS: data[it->first].position += speed * (data[it->first].isCamera ? (glm::vec3(0, 0, 0.1f) * data[it->first].rotation) : glm::vec3(0, 0, 0.1f)); break;

					case ROT_X_NEG: data[it->first].rotation = (data[it->first].rotation * glm::quat(glm::vec3(-.02f * speed, 0, 0))); break;
					case ROT_X_POS: data[it->first].rotation = (data[it->first].rotation * glm::quat(glm::vec3(.02f * speed, 0, 0))); break;
					case ROT_Y_NEG: data[it->first].rotation = (data[it->first].rotation * glm::quat(glm::vec3(0, -.02f * speed, 0))); break;
					case ROT_Y_POS: data[it->first].rotation = (data[it->first].rotation * glm::quat(glm::vec3(0, .02f * speed, 0))); break;
					case ROT_Z_NEG: data[it->first].rotation = (data[it->first].rotation * glm::quat(glm::vec3(0, 0, -.02f * speed))); break;
					case ROT_Z_POS: data[it->first].rotation = (data[it->first].rotation * glm::quat(glm::vec3(0, 0, .02f * speed))); break;
					}
				}

			}
		}



	}

	void SimPositionDeviceDriver::mouseMove(int xOffset, int yOffset)
	{
		mouseOffsetX += xOffset;
		mouseOffsetY += yOffset;
	}



	SimPositionDeviceDriver::SimPositionDeviceDriverAdaptor::SimPositionDeviceDriverAdaptor(SimPositionDeviceDriver* driver, std::string name)
	{
		this->driver = driver;
		this->name = name;
		if (driver->data.find(name) == driver->data.end())
		{
			driver->data[name] = SimPosData();
		}
	}

	glm::mat4 SimPositionDeviceDriver::SimPositionDeviceDriverAdaptor::getData()
	{
		return driver->data[name].getMatrix();
	}



	glm::mat4 SimPositionDeviceDriver::SimPosData::getMatrix()
	{
		if (isCamera)
			return glm::translate(glm::toMat4(rotation), -position);
		else
			return glm::translate(glm::mat4(), position) * glm::toMat4(rotation);
	}

}