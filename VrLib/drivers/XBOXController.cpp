#ifdef WIN32

#include "XBOXController.h"
#include <VrLib/Log.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

namespace vrlib
{
	XBOXDeviceDriver::XBOXDeviceDriver() {}

	DeviceDriverAdaptor* XBOXDeviceDriver::getAdaptor(std::string options) {
		XBOXData xboxData;
		DeviceType deviceType;

		if (options == "AButton") {
			xboxData = AButton;
			deviceType = Digital;
		}
		else if (options == "BButton") {
			xboxData = BButton;
			deviceType = Digital;
		}
		else if (options == "XButton") {
			xboxData = XButton;
			deviceType = Digital;
		}
		else if (options == "YButton") {
			xboxData = YButton;
			deviceType = Digital;
		}
		else if (options == "DPadUp") {
			xboxData = DPADUpButton;
			deviceType = Digital;
		}
		else if (options == "DPadDown") {
			xboxData = DPADDownButton;
			deviceType = Digital;
		}
		else if (options == "DPadLeft") {
			xboxData = DPADLeftButton;
			deviceType = Digital;
		}
		else if (options == "DPadRight") {
			xboxData = DPADRightButton;
			deviceType = Digital;
		}
		else if (options == "LeftShoulder") {
			xboxData = LeftShoulder;
			deviceType = Digital;
		}

		else if (options == "RightShoulder") {
			xboxData = RightShoulder;
			deviceType = Digital;
		}
		else if (options == "LeftTrigger") {
			xboxData = LeftTrigger;
			deviceType = Digital;
		}
		else if (options == "RightTrigger") {
			xboxData = RightTrigger;
			deviceType = Digital;
		}

		else if (options == "StartButton") {
			xboxData = StartButton;
			deviceType = Digital;
		}
		else if (options == "BackButton") {
			xboxData = BackButton;
			deviceType = Digital;
		}

		else if (options == "RightThumbStickAngle") {
			xboxData = RightThumbAngle;
			deviceType = Analog;
		}
		else if (options == "LeftThumbStickAngle") {
			xboxData = LeftThumbAngle;
			deviceType = Analog;
		}
		else if (options == "RightThumbStickMagnitude") {
			xboxData = RightThumbMagnitude;
			deviceType = Analog;
		}
		else if (options == "LeftThumbStickMagnitude") {
			xboxData = LeftThumbMagnitude;
			deviceType = Analog;
		}
		else if (options == "RightThumbPos") {
			xboxData = RightThumbPos;
			deviceType = TwoDimensional;
		}
		else if (options == "LeftThumbPos") {
			xboxData = LeftThumbPos;
			deviceType = TwoDimensional;
		}
		else {
			logger << "Wazmetdeez: " + options << Log::newline;
		}


		if (deviceType == Analog)
			return new XBOXAnalogDriverAdaptor(this, xboxData);
		if (deviceType == Digital)
			return new XBOXButtonDriverAdaptor(this, xboxData);
		if (deviceType == TwoDimensional)
			return new XBOXTwoDimensionAdaptor(this, xboxData);
		else
			throw "No adapter type found for this option: " + options;
	}

	XBOXDeviceDriver::XBOXAnalogDriverAdaptor::XBOXAnalogDriverAdaptor(XBOXDeviceDriver* driver, XBOXData dataType) : driver(driver), dataType(dataType) {

	}

	XBOXDeviceDriver::XBOXButtonDriverAdaptor::XBOXButtonDriverAdaptor(XBOXDeviceDriver* driver, XBOXData dataType) : driver(driver), dataType(dataType) {

	}

	XBOXDeviceDriver::XBOXTwoDimensionAdaptor::XBOXTwoDimensionAdaptor(XBOXDeviceDriver* driver, XBOXData dataType) : driver(driver), dataType(dataType) {

	}

	glm::vec2 XBOXDeviceDriver::XBOXTwoDimensionAdaptor::getData() {
		if (dataType == LeftThumbPos || dataType == RightThumbPos)
			return driver->getThumbPos(dataType);
		else
			return glm::vec2(12, 12);
	}


	float XBOXDeviceDriver::XBOXAnalogDriverAdaptor::getData() {
		if (dataType == RightThumbAngle || dataType == LeftThumbAngle)
			return driver->getThumbAngle(dataType);
		else if (dataType == RightThumbMagnitude || dataType == LeftThumbMagnitude)
			return driver->getThumbMagnitude(dataType);
		else
			return 0.0f;
	}

	bool XBOXDeviceDriver::isPressed(XBOXData button) {
		if (button == AButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
		if (button == BButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
		if (button == XButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
		if (button == YButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
		if (button == DPADUpButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		if (button == DPADDownButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		if (button == DPADRightButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
		if (button == DPADLeftButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		if (button == LeftShoulder)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		if (button == RightShoulder)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		if (button == LeftTrigger)
			return this->getState().Gamepad.bLeftTrigger != 0;
		if (button == RightTrigger)
			return this->getState().Gamepad.bRightTrigger != 0;
		if (button == StartButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
		if (button == BackButton)
			return (this->getState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
		else
			logger << "I can't check this button for presses:" << Log::newline;


		return false;
	}

	glm::vec2 XBOXDeviceDriver::getThumbPos(XBOXData thumbStick) {

		float thumbY, thumbX;
		int thumbDeadzone;

		if (thumbStick == LeftThumbPos) {
			thumbX = this->getState().Gamepad.sThumbLX;
			thumbY = this->getState().Gamepad.sThumbLY;
			thumbDeadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		}
		else if (thumbStick == RightThumbPos) {
			thumbX = this->getState().Gamepad.sThumbRX;
			thumbY = this->getState().Gamepad.sThumbRY;
			thumbDeadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		}
		else {
			return glm::vec2(0.0f, 0.0f);
		}

		if ((thumbX < thumbDeadzone &&thumbX > -thumbDeadzone) && (thumbY  < thumbDeadzone && thumbY  > -thumbDeadzone)) {
			return glm::vec2(0.0f, 0.0f);
		}

		if (thumbY > 0.0f)
			thumbY /= 32767.0f;
		else
			thumbY /= 32768.0f;

		if (thumbX > 0.0f)
			thumbX /= 32767.0f;
		else
			thumbX /= 32768.0f;

		return glm::vec2(thumbX, thumbY);
	}

	float XBOXDeviceDriver::getThumbAngle(XBOXData thumbStick) {
		float thumbX;
		float thumbY;

		if (thumbStick == LeftThumbAngle) {
			thumbX = this->getState().Gamepad.sThumbLX;
			thumbY = this->getState().Gamepad.sThumbLY;

		}
		else if (thumbStick = RightThumbAngle) {
			thumbX = this->getState().Gamepad.sThumbRX;
			thumbY = this->getState().Gamepad.sThumbRY;

		}

		if ((thumbX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && thumbX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(thumbY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && thumbY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
			return 0.0f;
		}

		float leftThumbY = this->getState().Gamepad.sThumbRY;
		float leftThumbX = this->getState().Gamepad.sThumbRX;

		return normalizedAngle(thumbX, thumbY);
	}

	float XBOXDeviceDriver::getThumbMagnitude(XBOXData thumbStick) {
		float thumbX;
		float thumbY;
		float normalizedMagnitude;
		int thumbDeadzone;

		if (thumbStick == LeftThumbMagnitude) {
			thumbX = this->getState().Gamepad.sThumbLX;
			thumbY = this->getState().Gamepad.sThumbLY;
			thumbDeadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		}
		else if (thumbStick = RightThumbMagnitude) {
			thumbX = this->getState().Gamepad.sThumbRX;
			thumbY = this->getState().Gamepad.sThumbRY;
			thumbDeadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		}
		else {
			return 0.0f;
		}

		if ((thumbX < thumbDeadzone && thumbX > -thumbDeadzone) && (thumbY < thumbDeadzone && thumbY > -thumbDeadzone)) {
			return 0.0f;
		}

		float magnitude = sqrt(thumbX*thumbX + thumbY*thumbY);

		if (magnitude > thumbDeadzone) {
			if (magnitude > 32767) {
				magnitude = 32767;
			}
			magnitude -= thumbDeadzone;
			normalizedMagnitude = magnitude / (32767 - thumbDeadzone);
		}
		else {
			normalizedMagnitude = 0.0f;
		}

		return normalizedMagnitude;
	}

	float XBOXDeviceDriver::normalizedAngle(float thumbY, float thumbX) {
		float fReturnAngle = 0.0f;

		float magnitude = ((float)sqrt(thumbX*thumbX + thumbY*thumbY));
		float fRadians = (float)std::atan2(thumbY, thumbX) - (float)(0.5f * M_PI);
		fReturnAngle = ((float)((fRadians / M_PI) * 180) + (fRadians > 0 ? 0 : 360));

		//Translate from deadzone. Angle 270 means we are actually death
		//center and not moving the controller
		if (fReturnAngle == 270)
			return 0;
		else
			return fReturnAngle / 360.0f;
	}

	XINPUT_STATE XBOXDeviceDriver::getState() {
		return this->controllerState;
	}

	void XBOXDeviceDriver::update() {
		ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
		XInputGetState(0, &controllerState);
	}

	DigitalState XBOXDeviceDriver::XBOXButtonDriverAdaptor::getData() {
		bool b = driver->isPressed(dataType);
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
			logger << "Unknown digital device values? The 2e verdieping is collapsing" << Log::newline;
		lastValue = b;
		return returnValue;
	}
}


#endif