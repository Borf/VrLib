#pragma once

#include <VrLib/Device.h>
#include <glm/glm.hpp>
#include <windows.h>
//#include <CaveLib\CaveLib\Components\Panel.h>
//#include <CaveLib\CaveLib\Components\Component.h>
#include <sixense.h>

namespace vrlib
{
	class HydraDeviceDriver : public DeviceDriver
	{
	private:
		enum HydraButton
		{
			LeftButtonOne, LeftButtonTwo, LeftButtonThree, LeftButtonFour,
			LeftTrigger, LeftBumper, LeftStartButton, LeftJoystickButton,
			RightButtonOne, RightButtonTwo, RightButtonThree, RightButtonFour,
			RightTrigger, RightBumper, RightStartButton, RightJoystickButton
		};

		enum HydraJoystick
		{
			LeftJoystick, RightJoystick
		};
		enum HydraNunchuk
		{
			LeftNunchuk, RightNunchuk
		};

		static bool hydraInitialized;

		int leftControllerIndex;
		int rightControllerIndex;
		sixenseControllerData controllerDataLeft;
		sixenseControllerData controllerDataRight;

		void updateInitialization();

		class HydraDigitalDeviceDriverAdaptor : public DigitalDeviceDriverAdaptor
		{
		private:
			HydraDeviceDriver* driver;
			HydraButton button;
			bool lastValue;

		public:
			HydraDigitalDeviceDriverAdaptor(HydraDeviceDriver*, HydraButton button);
			DigitalState getData();
		};

		class HydraAnalogDeviceDriverAdaptor : public AnalogDeviceDriverAdaptor
		{
		private:
			HydraDeviceDriver* driver;
			HydraButton button;

		public:
			HydraAnalogDeviceDriverAdaptor(HydraDeviceDriver*, HydraButton button);
			float getData();
		};

		class HydraTwoDimensionDeviceDriverAdaptor : public TwoDimensionDeviceDriverAdaptor{
		private:
			HydraDeviceDriver* driver;
			HydraJoystick joystick;

		public:
			HydraTwoDimensionDeviceDriverAdaptor(HydraDeviceDriver*, HydraJoystick joystick);
			glm::vec2 getData();
		};

		class HydraNunchukDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor{
		private:
			HydraDeviceDriver* driver;
			HydraNunchuk nunchuk;

		public:
			HydraNunchukDeviceDriverAdaptor(HydraDeviceDriver*, HydraNunchuk nunchuk);
			glm::mat4 getData();
		};

	public:
		HydraDeviceDriver();
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
		virtual void update();

		bool isPressed(HydraButton);
		float getAnalogValue(HydraButton);
		glm::vec2 getJoystickPosition(HydraJoystick joystick);
		glm::mat4 getNunchukData(HydraNunchuk nunchuk);
	};
}