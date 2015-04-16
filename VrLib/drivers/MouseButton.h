#pragma once

#include <VrLib/Device.h>



namespace vrlib
{
	class MouseButtonDeviceDriver : public DeviceDriver
	{
	public:
		enum MouseButton
		{
			Left, Middle, Right
		};
	private:
		class MouseButtonDeviceDriverAdaptor : public DigitalDeviceDriverAdaptor
		{
			MouseButtonDeviceDriver* driver;
			MouseButton button;
			bool lastValue;
		public:
			MouseButtonDeviceDriverAdaptor(MouseButtonDeviceDriver*, MouseButton button);
			virtual DigitalState getData();
		};

		bool left;
		bool middle;
		bool right;

	public:
		MouseButtonDeviceDriver();
		void mouseDown(MouseButton button);
		void mouseUp(MouseButton button);


		bool isPressed(MouseButton button);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
	};
}