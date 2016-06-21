#ifdef WIN32

#include <VrLib/drivers/HydraDriver.h>
#include <VrLib/Log.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <sixense_utils\controller_manager\controller_manager.hpp>

namespace vrlib
{

	bool HydraDeviceDriver::hydraInitialized = false;

	DeviceDriverAdaptor* HydraDeviceDriver::getAdaptor(std::string options)
	{
		HydraButton  selectedButton;
		HydraJoystick selectedJoystick;
		HydraNunchuk selectedNunchuk;
		int type = -1;	// 0 = digital, 1 = analog, 2 = 2D (joystick), 3 = positional

		if (options == "LeftButtonOne"){
			selectedButton = LeftButtonOne;
			type = 0;
		}
		else if (options == "LeftButtonTwo"){
			selectedButton = LeftButtonTwo;
			type = 0;
		}
		else if (options == "LeftButtonThree"){
			selectedButton = LeftButtonThree;
			type = 0;
		}
		else if (options == "LeftButtonFour"){
			selectedButton = LeftButtonFour;
			type = 0;
		}
		else if (options == "LeftStartButton"){
			selectedButton = LeftStartButton;
			type = 0;
		}
		else if (options == "LeftJoystickButton"){
			selectedButton = LeftJoystickButton;
			type = 0;
		}
		else if (options == "LeftBumper"){
			selectedButton = LeftBumper;
			type = 0;
		}

		else if (options == "RightButtonOne"){
			selectedButton = RightButtonOne;
			type = 0;
		}
		else if (options == "RightButtonTwo"){
			selectedButton = RightButtonTwo;
			type = 0;
		}
		else if (options == "RightButtonThree"){
			selectedButton = RightButtonThree;
			type = 0;
		}
		else if (options == "RightButtonFour"){
			selectedButton = RightButtonFour;
			type = 0;
		}
		else if (options == "RightStartButton"){
			selectedButton = RightStartButton;
			type = 0;
		}
		else if (options == "RightJoystickButton"){
			selectedButton = RightJoystickButton;
			type = 0;
		}
		else if (options == "RightBumper"){
			selectedButton = RightBumper;
			type = 0;
		}
		else if (options == "LeftTrigger"){
			selectedButton = LeftTrigger;
			type = 1;
		}
		else if (options == "RightTrigger"){
			selectedButton = RightTrigger;
			type = 1;
		}

		else if (options == "LeftJoystick"){
			selectedJoystick = LeftJoystick;
			type = 2;
		}
		else if (options == "RightJoystick"){
			selectedJoystick = RightJoystick;
			type = 2;
		}
		else if (options == "LeftNunchuk"){
			selectedNunchuk = LeftNunchuk;
			type = 3;
		}
		else if (options == "RightNunchuk"){
			selectedNunchuk = RightNunchuk;
			type = 3;
		}
		else
			logger << "Invalid  " + options << Log::newline;

		if (type == 0)
			return new HydraDigitalDeviceDriverAdaptor(this, selectedButton);
		if (type == 1)
			return new HydraAnalogDeviceDriverAdaptor(this, selectedButton);
		if (type == 2)
			return new HydraTwoDimensionDeviceDriverAdaptor(this, selectedJoystick);
		if (type == 3)
			return new HydraNunchukDeviceDriverAdaptor(this, selectedNunchuk);
		return NULL;
	}

	void controllerManagerSetupCallback(sixenseUtils::ControllerManager::setup_step step){
		if (sixenseUtils::getTheControllerManager()->isMenuVisible()){
			std::cout << "[Razer Hydra Setup] " << sixenseUtils::getTheControllerManager()->getStepString() << std::endl;
		}
	}

	HydraDeviceDriver::HydraDeviceDriver()
	{
		if (!hydraInitialized){
			sixenseInit();
			sixenseUtils::getTheControllerManager()->setGameType(sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER);
			sixenseSetActiveBase(0);
			sixenseUtils::getTheControllerManager()->registerSetupCallback(controllerManagerSetupCallback);
		}
	}

	HydraDeviceDriver::HydraDigitalDeviceDriverAdaptor::HydraDigitalDeviceDriverAdaptor(HydraDeviceDriver* driver, HydraButton button)
	{
		this->driver = driver;
		this->button = button;
	}

	DigitalState HydraDeviceDriver::HydraDigitalDeviceDriverAdaptor::getData()
	{
		if (!hydraInitialized){
			return OFF;
		}

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
			logger << "Unknown digital device values? The 2e verdieping is collapsing" << Log::newline;

		lastValue = b;
		return returnValue;
	}

	HydraDeviceDriver::HydraAnalogDeviceDriverAdaptor::HydraAnalogDeviceDriverAdaptor(HydraDeviceDriver* driver, HydraButton button)
	{
		this->driver = driver;
		this->button = button;
	}

	float HydraDeviceDriver::HydraAnalogDeviceDriverAdaptor::getData()
	{
		return driver->getAnalogValue(button);
	}

	HydraDeviceDriver::HydraTwoDimensionDeviceDriverAdaptor::HydraTwoDimensionDeviceDriverAdaptor(HydraDeviceDriver* driver, HydraJoystick joystick){
		this->driver = driver;
		this->joystick = joystick;
	}

	glm::vec2 HydraDeviceDriver::HydraTwoDimensionDeviceDriverAdaptor::getData()
	{
		return driver->getJoystickPosition(joystick);
	}

	glm::vec2 HydraDeviceDriver::getJoystickPosition(HydraJoystick joystick)
	{
		if (sixenseUtils::getTheControllerManager()->isMenuVisible())
			return glm::vec2(-999, -999);
		if (joystick == LeftJoystick)
			return glm::vec2(controllerDataLeft.joystick_x, controllerDataLeft.joystick_y);
		if (joystick == RightJoystick)
			return glm::vec2(controllerDataRight.joystick_x, controllerDataRight.joystick_y);
		return glm::vec2(0, 0);
	}

	HydraDeviceDriver::HydraNunchukDeviceDriverAdaptor::HydraNunchukDeviceDriverAdaptor(HydraDeviceDriver* driver, HydraNunchuk nunchuk){
		this->driver = driver;
		this->nunchuk = nunchuk;
	}

	glm::mat4 HydraDeviceDriver::HydraNunchukDeviceDriverAdaptor::getData(){
		return driver->getNunchukData(nunchuk);
	}

	glm::mat4 HydraDeviceDriver::getNunchukData(HydraNunchuk nunchuk){
		sixenseControllerData* controllerData;

		if (nunchuk == LeftNunchuk)
			controllerData = &controllerDataLeft;
		if (nunchuk == RightNunchuk)
			controllerData = &controllerDataRight;

		glm::vec3 position(controllerData->pos[0], controllerData->pos[1], controllerData->pos[2]);
		glm::quat rotation(controllerData->rot_quat[3], controllerData->rot_quat[0], controllerData->rot_quat[1], controllerData->rot_quat[2]);

		// Adjust height (default height places ray lower than expected)
		position.z -= 200;
		position *= 0.01;
		position /= 3.0f;

		glm::mat4 returnMatrix;
		returnMatrix = glm::translate(returnMatrix, position);

		returnMatrix = returnMatrix * glm::toMat4(rotation);

		return returnMatrix;
	}

	float HydraDeviceDriver::getAnalogValue(HydraButton button)
	{
		if (button == LeftTrigger)
			return controllerDataLeft.trigger;

		if (button == RightTrigger)
			return controllerDataRight.trigger;

		return 0;
	}

	bool HydraDeviceDriver::isPressed(HydraButton button)
	{
		unsigned int leftButtons = controllerDataLeft.buttons;
		unsigned int rightButtons = controllerDataRight.buttons;

		if (button == LeftButtonOne){
			return (leftButtons & SIXENSE_BUTTON_1) != 0;
		}
		if (button == LeftButtonTwo){
			return (leftButtons & SIXENSE_BUTTON_2) != 0;
		}
		if (button == LeftButtonThree){
			return (leftButtons & SIXENSE_BUTTON_3) != 0;
		}
		if (button == LeftButtonFour){
			return (leftButtons & SIXENSE_BUTTON_4) != 0;
		}
		if (button == LeftStartButton){
			return (leftButtons & SIXENSE_BUTTON_START) != 0;
		}
		if (button == LeftTrigger){
			return controllerDataLeft.trigger > 0.0f;
		}
		if (button == LeftBumper){
			return (leftButtons & SIXENSE_BUTTON_BUMPER) != 0;
		}
		if (button == LeftJoystickButton){
			return (leftButtons & SIXENSE_BUTTON_JOYSTICK) != 0;
		}

		if (button == RightButtonOne){
			return (rightButtons & SIXENSE_BUTTON_1) != 0;
		}
		if (button == RightButtonTwo){
			return (rightButtons & SIXENSE_BUTTON_2) != 0;
		}
		if (button == RightButtonThree){
			return (rightButtons & SIXENSE_BUTTON_3) != 0;
		}
		if (button == RightButtonFour){
			return (rightButtons & SIXENSE_BUTTON_4) != 0;
		}
		if (button == RightStartButton){
			return (rightButtons & SIXENSE_BUTTON_START) != 0;
		}
		if (button == RightTrigger){
			return controllerDataRight.trigger > 0.0f;
		}
		if (button == RightBumper){
			return (rightButtons & SIXENSE_BUTTON_BUMPER) != 0;
		}
		if (button == RightJoystickButton){
			return (rightButtons & SIXENSE_BUTTON_JOYSTICK) != 0;
		}
		return false;
	}

	void HydraDeviceDriver::updateInitialization(){
		if (!hydraInitialized){
			sixenseAllControllerData acd;
			sixenseGetAllNewestData(&acd);
			sixenseUtils::getTheControllerManager()->update(&acd);

			hydraInitialized = !sixenseUtils::getTheControllerManager()->isMenuVisible();
		}

		if (hydraInitialized){
			leftControllerIndex = sixenseUtils::getTheControllerManager()->getIndex(sixenseUtils::ControllerManager::P1L);
			rightControllerIndex = sixenseUtils::getTheControllerManager()->getIndex(sixenseUtils::ControllerManager::P1R);
		}
	}

	void HydraDeviceDriver::update(){
		if (!hydraInitialized){
			updateInitialization();
		}
		else{
			sixenseGetNewestData(leftControllerIndex, &controllerDataLeft);
			sixenseGetNewestData(rightControllerIndex, &controllerDataRight);
		}
	}
}


#endif