#pragma once

#include <VrLib/Device.h>
#include <VrLib/drivers/Keyboard.h>
#include <map>
#include <list>

#include <glm/gtc/quaternion.hpp>

namespace vrlib
{
	namespace json
	{
		class Value;
	}

	class KeyboardDeviceDriver;


	class SimPositionDeviceDriver : public DeviceDriver
	{
		class SimPosData
		{
		public:
			glm::vec3 position;
			glm::quat rotation;
			bool isCamera;

			SimPosData()
			{
				isCamera = false;
			}
			SimPosData(const SimPosData &other)
			{
				isCamera = other.isCamera;
				rotation = other.rotation;
				position = other.position;
			}

			glm::mat4 getMatrix();
		};

		class SimPositionDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
			SimPositionDeviceDriver* driver;
			std::string name;
		public:
			SimPositionDeviceDriverAdaptor(SimPositionDeviceDriver*, std::string name);
			virtual glm::mat4 getData();
		};

		enum Action
		{
			TRANS_X_NEG,
			TRANS_X_POS,
			TRANS_Y_NEG,
			TRANS_Y_POS,
			TRANS_Z_NEG,
			TRANS_Z_POS,

			ROT_X_NEG,
			ROT_X_POS,
			ROT_Y_NEG,
			ROT_Y_POS,
			ROT_Z_NEG,
			ROT_Z_POS,
		};

		typedef std::pair<Action, std::pair<KeyboardDeviceDriver::KeyboardButton, KeyboardDeviceDriver::KeyboardModifiers> > keyhandler;

		std::map<std::string, std::list<keyhandler> > keyHandlers;

		std::map<std::string, SimPosData> data;
		friend class Kernel;

		int mouseOffsetX;
		int mouseOffsetY;

	public:
		SimPositionDeviceDriver(json::Value config);
		void update(KeyboardDeviceDriver* keyboardDriver);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
		void mouseMove(int xOffset, int yOffset);
	};
}