#pragma once

#include <VrLib/Device.h>

#include <map>
#include <string>

namespace vrlib
{
	class SimPositionDeviceDriver;
	class OculusDeviceDriver;

	class KeyboardDeviceDriver : public DeviceDriver
	{
		friend class SimPositionDeviceDriver;
		friend class OculusDeviceDriver;
	private:
		enum KeyboardButton
		{
			KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
			KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
			KEY_SPACEBAR,
			KEY_PAGEUP, KEY_PAGEDOWN, KEY_HOME, KEY_END, KEY_INSERT, KEY_DELETE,
			KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
			KEY_RETURN,

			KEY_NUMPAD_1, KEY_NUMPAD_2, KEY_NUMPAD_3, KEY_NUMPAD_4, KEY_NUMPAD_5, KEY_NUMPAD_6, KEY_NUMPAD_7, KEY_NUMPAD_8, KEY_NUMPAD_9, KEY_NUMPAD_0,/*KEY_PLUS,KEY_MINUS,*/


			KEY_UNDEFINED,
		};

		enum KeyboardModifiers
		{
			KEYMOD_NONE = 0,
			KEYMOD_CTRL = 1,
			KEYMOD_ALT = 2,
			KEYMOD_SHIFT = 4,
			KEYMOD_CTRLALT = 1 | 2,
			KEYMOD_CTRLSHIFT = 1 | 4,
			KEYMOD_ALTSHIFT = 2 | 4,
			KEYMOD_ALTSHIFTCTRL = 1 | 2 | 4,
		};

		class KeyboardDeviceDriverAdaptor : public DigitalDeviceDriverAdaptor
		{
			KeyboardDeviceDriver* driver;
			KeyboardButton button;
			bool lastValue;
		public:
			KeyboardDeviceDriverAdaptor(KeyboardDeviceDriver*, KeyboardButton button);
			virtual DigitalState getData();
		};

		bool buttonsPressed[KEY_UNDEFINED];

		std::map<int, KeyboardDeviceDriver::KeyboardButton> buttonLookup;
		struct keymap { KeyboardButton btn; std::string configValue; int vk; };
		static keymap keyMapping[];
	public:


		KeyboardDeviceDriver();
		void keyDown(int button);
		void keyUp(int button);

		bool isPressed(KeyboardButton button);
		bool isModPressed(KeyboardModifiers mods);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);

		static std::pair<KeyboardButton, KeyboardModifiers> parseString(std::string);

	};
}