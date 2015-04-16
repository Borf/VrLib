#include <VrLib/Device.h>
#include <glm/glm.hpp>
#include <windows.h>
#include <XInput.h>

#pragma comment(lib, "XInput9_1_0.lib")

namespace vrlib
{
	class XBOXDeviceDriver : public DeviceDriver
	{
	public:
		XBOXDeviceDriver();
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
	private:
		enum DeviceType
		{
			Analog, Digital, TwoDimensional
		};

		enum XBOXData
		{
			LeftThumbMagnitude, RightThumbMagnitude, LeftThumbAngle, RightThumbAngle,
			AButton, BButton, XButton, YButton,
			DPADLeftButton, DPADRightButton, DPADUpButton, DPADDownButton,
			LeftShoulder, RightShoulder, LeftTrigger, RightTrigger,
			StartButton, BackButton,
			RightThumbPos, LeftThumbPos
		};

		class XBOXButtonDriverAdaptor : public DigitalDeviceDriverAdaptor
		{
			XBOXDeviceDriver* driver;
			XBOXData dataType;
			bool lastValue;

		public:
			XBOXButtonDriverAdaptor(XBOXDeviceDriver*, XBOXData dataType);
			DigitalState getData();
		};

		class XBOXTwoDimensionAdaptor : public TwoDimensionDeviceDriverAdaptor {
			XBOXDeviceDriver* driver;
			XBOXData dataType;

		public:
			XBOXTwoDimensionAdaptor(XBOXDeviceDriver*, XBOXData dataType);
			glm::vec2 getData();
		};

		class XBOXAnalogDriverAdaptor : public AnalogDeviceDriverAdaptor
		{
			XBOXDeviceDriver* driver;
			XBOXData dataType;
		public:
			XBOXAnalogDriverAdaptor(XBOXDeviceDriver*, XBOXData dataType);
			float getData();
		};

		float normalizedAngle(float, float);
		float getThumbAngle(XBOXData);
		float getThumbMagnitude(XBOXData);
		glm::vec2 getThumbPos(XBOXData);

		XINPUT_STATE getState();
		XINPUT_STATE controllerState;

		bool isPressed(XBOXData);
		void update();

	};

}