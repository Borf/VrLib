#pragma once

#include <VrLib/Device.h>
#include <VrLib/drivers/Keyboard.h>

#include <OVR_CAPI.h>
#include <list>
#include <vrlib/json.h>

namespace vrlib
{
	class KeyboardDeviceDriver;

	class OculusDeviceDriver : public DeviceDriver
	{
		class OculusDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
		private:
			bool useTracker;
		public:
			ovrHmd hmd;
			ovrEyeRenderDesc* eye_rdesc;

			OculusDeviceDriverAdaptor(json::Value config);
			glm::vec3 getDirection();
			virtual glm::mat4 getData();
			void resetSensor();
			void increaseEyeDistance(float distanceToAdd);
			float getEyeDistance();
		};

		json::Value config;

		enum Action
		{
			RESET,
			INCREASE_EYE_DISTANCE,
			DECREASE_EYE_DISTANCE,
			TOGGLE_CONFIG,
		};

		typedef std::pair<Action, std::pair<KeyboardDeviceDriver::KeyboardButton, KeyboardDeviceDriver::KeyboardModifiers> > keyhandler;
		std::map<std::string, std::list<keyhandler>> keyHandlers;

	private:
		bool showConfigPanel;

	public:
		OculusDeviceDriver(json::Value config);
		void update(KeyboardDeviceDriver* keyboardDriver);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);

		ovrHmd* hmd;
		ovrEyeRenderDesc eye_rdesc[2];

		void resetSensor();
		void increaseEyeDistance(float distanceToAdd);
		void toggleConfigPanel(bool value);
		bool configPanelIsShown();
		float getEyeDistance();
		glm::vec3 getDirection();
		void beginFrame();
	};
}