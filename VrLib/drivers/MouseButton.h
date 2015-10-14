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
	protected:
		bool left;
		bool middle;
		bool right;

	public:
		MouseButtonDeviceDriver();
		virtual void mouseDown(MouseButton button);
		virtual void mouseUp(MouseButton button);
		virtual void mouseMove(int x, int y) { }

		bool isPressed(MouseButton button);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
	};
}