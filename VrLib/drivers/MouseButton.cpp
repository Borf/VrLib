#include <VrLib/drivers/MouseButton.h>
#include <VrLib/Log.h>

namespace vrlib
{
	DeviceDriverAdaptor* MouseButtonDeviceDriver::getAdaptor(std::string options)
	{
		MouseButton btn;
		if (options == "left")
			btn = Left;
		else if (options == "right")
			btn = Right;
		else if (options == "middle")
			btn = Middle;
		else
			logger << "Unknown mousebutton: " << options << Log::newline;

		return new MouseButtonDeviceDriverAdaptor(this, btn);
	}


	void MouseButtonDeviceDriver::mouseDown(MouseButton button)
	{
		if (button == Left)
			left = true;
		else if (button == Right)
			right = true;
		else if (button == Middle)
			middle = true;
	}

	void MouseButtonDeviceDriver::mouseUp(MouseButton button)
	{
		if (button == Left)
			left = false;
		else if (button == Right)
			right = false;
		else if (button == Middle)
			middle = false;
	}

	bool MouseButtonDeviceDriver::isPressed(MouseButton button)
	{
		if (button == Left)
			return left;
		if (button == Middle)
			return middle;
		if (button == Right)
			return right;
		logger << "Unknown button" << Log::newline;
		return false;
	}

	MouseButtonDeviceDriver::MouseButtonDeviceDriver()
	{
		left = false;
		middle = false;
		right = false;
	}




	MouseButtonDeviceDriver::MouseButtonDeviceDriverAdaptor::MouseButtonDeviceDriverAdaptor(MouseButtonDeviceDriver* driver, MouseButton button)
	{
		this->driver = driver;
		this->button = button;
	}

	DigitalState MouseButtonDeviceDriver::MouseButtonDeviceDriverAdaptor::getData()
	{
		bool b = driver->isPressed(button);
		DigitalState returnValue;

		if (b && !lastValue)
			returnValue = TOGGLE_ON;
		else if (b && lastValue)
			returnValue = ON;
		else if (!b && !lastValue)
			returnValue = OFF;
		else if (!b && lastValue)
			returnValue = TOGGLE_OFF;
		else
			logger << "Unknown digital device values? The universe is collapsing" << Log::newline;
		lastValue = b;
		return returnValue;
	}
}