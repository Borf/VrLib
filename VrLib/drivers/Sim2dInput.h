#pragma once

#include <VrLib/Device.h>
#include <VrLib/drivers/Keyboard.h>
#include <VrLib/json.hpp>
#include <map>
#include <list>

#include <glm/gtc/quaternion.hpp>

namespace vrlib
{
	class KeyboardDeviceDriver;


	class Sim2dInputDeviceDriver : public DeviceDriver
	{
		class SimPosData
		{
		public:
			glm::vec2 position;
			
			SimPosData()
			{
			}
			SimPosData(const SimPosData &other)
			{
				position = other.position;
			}

			glm::vec2 getPosition() { return position; }
		};

		class Sim2dInputDeviceDriverAdaptor : public TwoDimensionDeviceDriverAdaptor
		{
			Sim2dInputDeviceDriver* driver;
			std::string name;
		public:
			Sim2dInputDeviceDriverAdaptor(Sim2dInputDeviceDriver*, std::string name);
			virtual glm::vec2 getData();
		};

		enum Action
		{
			X_NEG,
			X_POS,
			Y_NEG,
			Y_POS
		};

		typedef std::pair<Action, std::pair<KeyboardDeviceDriver::KeyboardButton, KeyboardDeviceDriver::KeyboardModifiers> > keyhandler;

		std::map<std::string, std::list<keyhandler> > keyHandlers;

		std::map<std::string, SimPosData> data;
		friend class Kernel;

	public:
		Sim2dInputDeviceDriver(const json &config);
		void update(KeyboardDeviceDriver* keyboardDriver, double elapsedTime);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
	};
}