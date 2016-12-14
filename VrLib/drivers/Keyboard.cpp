#include <VrLib/drivers/Keyboard.h>
#include <VrLib/Log.h>

#ifdef WIN32
#include <windows.h>
#else

enum Keys
{
	VK_PRIOR,
	VK_NEXT,
	VK_HOME,
	VK_END,
	VK_INSERT,
	VK_DELETE,
	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
	VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,
	VK_TAB,
	VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,
};

#endif

#include <vector>
#include <string>


namespace vrlib
{
	KeyboardDeviceDriver::keymap KeyboardDeviceDriver::keyMapping[] = {
		{ KeyboardButton::KEY_PAGEUP,			"pageup",		VK_PRIOR },
		{ KeyboardButton::KEY_PAGEDOWN,		"pagedown",		VK_NEXT },
		{ KeyboardButton::KEY_HOME,			"home",			VK_HOME },
		{ KeyboardButton::KEY_END,			"end",			VK_END },
		{ KeyboardButton::KEY_INSERT,			"insert",		VK_INSERT },
		{ KeyboardButton::KEY_DELETE,			"delete",		VK_DELETE },
		{ KeyboardButton::KEY_F1		,		"f1",			VK_F1 },
		{ KeyboardButton::KEY_F2		,		"f2",			VK_F2 },
		{ KeyboardButton::KEY_F3		,		"f3",			VK_F3 },
		{ KeyboardButton::KEY_F4		,		"f4",			VK_F4 },
		{ KeyboardButton::KEY_F5		,		"f5",			VK_F5 },
		{ KeyboardButton::KEY_F6		,		"f6",			VK_F6 },
		{ KeyboardButton::KEY_F7		,		"f7",			VK_F7 },
		{ KeyboardButton::KEY_F8		,		"f8",			VK_F8 },
		{ KeyboardButton::KEY_F9		,		"f9",			VK_F9 },
		{ KeyboardButton::KEY_F10		,		"f10",			VK_F10 },
		{ KeyboardButton::KEY_F11		,		"f11",			VK_F11 },
		{ KeyboardButton::KEY_F12		,		"f12",			VK_F12 },

		{ KeyboardButton::KEY_NUMPAD_0,		"numpad0",		VK_NUMPAD0 },
		{ KeyboardButton::KEY_NUMPAD_1,		"numpad1",		VK_NUMPAD1 },
		{ KeyboardButton::KEY_NUMPAD_2,		"numpad2",		VK_NUMPAD2 },
		{ KeyboardButton::KEY_NUMPAD_3,		"numpad3",		VK_NUMPAD3 },
		{ KeyboardButton::KEY_NUMPAD_4,		"numpad4",		VK_NUMPAD4 },
		{ KeyboardButton::KEY_NUMPAD_5,		"numpad5",		VK_NUMPAD5 },
		{ KeyboardButton::KEY_NUMPAD_6,		"numpad6",		VK_NUMPAD6 },
		{ KeyboardButton::KEY_NUMPAD_7,		"numpad7",		VK_NUMPAD7 },
		{ KeyboardButton::KEY_NUMPAD_8,		"numpad8",		VK_NUMPAD8 },
		{ KeyboardButton::KEY_NUMPAD_9,		"numpad9",		VK_NUMPAD9 },
		/*{ KeyboardDeviceDriver::KEY_PLUS,			"+",			VK_OEM_PLUS },
		{ KeyboardDeviceDriver::KEY_MINUS,			"-",			VK_OEM_MINUS },*/

		{ KeyboardButton::KEY_0		,		"0",			'0' },
		{ KeyboardButton::KEY_1		,		"1",			'1' },
		{ KeyboardButton::KEY_2		,		"2",			'2' },
		{ KeyboardButton::KEY_3		,		"3",			'3' },
		{ KeyboardButton::KEY_4		,		"4",			'4' },
		{ KeyboardButton::KEY_5		,		"5",			'5' },
		{ KeyboardButton::KEY_6		,		"6",			'6' },
		{ KeyboardButton::KEY_7		,		"7",			'7' },
		{ KeyboardButton::KEY_8		,		"8",			'8' },
		{ KeyboardButton::KEY_9		,		"9",			'9' },

		{ KeyboardButton::KEY_A		,		"a",			'A' },
		{ KeyboardButton::KEY_B		,		"b",			'B' },
		{ KeyboardButton::KEY_C		,		"c",			'C' },
		{ KeyboardButton::KEY_D		,		"d",			'D' },
		{ KeyboardButton::KEY_E		,		"e",			'E' },
		{ KeyboardButton::KEY_F		,		"f",			'F' },
		{ KeyboardButton::KEY_G		,		"g",			'G' },
		{ KeyboardButton::KEY_H		,		"h",			'H' },
		{ KeyboardButton::KEY_I		,		"i",			'I' },
		{ KeyboardButton::KEY_J		,		"j",			'J' },
		{ KeyboardButton::KEY_K		,		"k",			'K' },
		{ KeyboardButton::KEY_L		,		"l",			'L' },
		{ KeyboardButton::KEY_M		,		"m",			'M' },
		{ KeyboardButton::KEY_N		,		"n",			'N' },
		{ KeyboardButton::KEY_O		,		"o",			'O' },
		{ KeyboardButton::KEY_P		,		"p",			'P' },
		{ KeyboardButton::KEY_Q		,		"q",			'Q' },
		{ KeyboardButton::KEY_R		,		"r",			'R' },
		{ KeyboardButton::KEY_S		,		"s",			'S' },
		{ KeyboardButton::KEY_T		,		"t",			'T' },
		{ KeyboardButton::KEY_U		,		"u",			'U' },
		{ KeyboardButton::KEY_V		,		"v",			'V' },
		{ KeyboardButton::KEY_W		,		"w",			'W' },
		{ KeyboardButton::KEY_X		,		"x",			'X' },
		{ KeyboardButton::KEY_Y		,		"y",			'Y' },
		{ KeyboardButton::KEY_Z		,		"z",			'Z' },
		{ KeyboardButton::KEY_SPACEBAR,		"spacebar",		' ' },
		{ KeyboardButton::KEY_TAB		,		"tab",			VK_TAB },

		{ KeyboardButton::KEY_ARROW_UP,		"arrowup",		VK_UP },
		{ KeyboardButton::KEY_ARROW_DOWN,		"arrowdown",	VK_DOWN },
		{ KeyboardButton::KEY_ARROW_LEFT,		"arrowleft",	VK_LEFT },
		{ KeyboardButton::KEY_ARROW_RIGHT,	"arrowright",	VK_RIGHT },


	


		{ KeyboardButton::KEY_UNDEFINED,"UNDEFINED",0 }
	};



	DeviceDriverAdaptor* KeyboardDeviceDriver::getAdaptor( std::string options )
	{
		for(int i = 0; i < sizeof(keyMapping) / sizeof(keymap); i++)
		{
			if(keyMapping[i].configValue == options)
				return new KeyboardDeviceDriverAdaptor(this, keyMapping[i].btn);
		}
		return NULL;
	}


	void KeyboardDeviceDriver::keyDown( int button )
	{
		if(buttonLookup.find(button) != buttonLookup.end())
			buttonsPressed[(int)buttonLookup[button]] = true;
	}

	void KeyboardDeviceDriver::keyUp( int button )
	{
		if(buttonLookup.find(button) != buttonLookup.end())
			buttonsPressed[(int)buttonLookup[button]] = false;
	}

	void KeyboardDeviceDriver::keyChar(char character)
	{
	}

	bool KeyboardDeviceDriver::isPressed( KeyboardButton button )
	{
		return buttonsPressed[(int)button];
	}

	KeyboardDeviceDriver::KeyboardDeviceDriver()
	{
		for(int i = 0; i < sizeof(keyMapping) / sizeof(keymap); i++)
			buttonLookup[keyMapping[i].vk] = keyMapping[i].btn;
		for(int i = 0; i < (int)KeyboardButton::KEY_UNDEFINED; i++)
			buttonsPressed[i] = false;
	}

	std::pair<KeyboardDeviceDriver::KeyboardButton, KeyboardDeviceDriver::KeyboardModifiers> KeyboardDeviceDriver::parseString( std::string str)
	{
		std::vector<std::string> splitted;
		while(str.find("+") != std::string::npos)
		{
			splitted.push_back(str.substr(0, str.find("+")));
			str = str.substr(str.find("+")+1);
		}
		splitted.push_back(str);

		KeyboardButton btn = KeyboardButton::KEY_UNDEFINED;
		KeyboardModifiers mod = KEYMOD_NONE;

		for(size_t i = 0; i < splitted.size(); i++)
		{
			if(splitted[i] == "ctrl")
				mod = (KeyboardModifiers)((int)KEYMOD_CTRL|mod);
			else if(splitted[i] == "alt")
				mod = (KeyboardModifiers)((int)KEYMOD_ALT|mod);
			else if(splitted[i] == "shift")
				mod = (KeyboardModifiers)((int)KEYMOD_SHIFT|mod);
			else
			{
				for(int ii = 0; ii < sizeof(keyMapping) / sizeof(keymap); ii++)
				{
					if(keyMapping[ii].configValue == splitted[i])
						btn = keyMapping[ii].btn;
				}
			}
		}

	
		return std::pair<KeyboardDeviceDriver::KeyboardButton, KeyboardDeviceDriver::KeyboardModifiers>(btn,mod);
	}

	bool KeyboardDeviceDriver::isModPressed( KeyboardModifiers mods )
	{
#ifdef WIN32
		bool shift = ((GetKeyState(VK_LSHIFT) | GetKeyState(VK_RSHIFT))&0x80) != 0;
		bool ctrl = ((GetKeyState(VK_LCONTROL) | GetKeyState(VK_RCONTROL))&0x80) != 0;
		bool alt = ((GetKeyState(VK_LMENU) | GetKeyState(VK_RMENU))&0x80) != 0;
#else
		bool shift = false, ctrl = false, alt = false;
#endif
		switch(mods)
		{
		case KEYMOD_ALT:			return alt && !shift && !ctrl;
		case KEYMOD_SHIFT:			return !alt && shift && !ctrl;
		case KEYMOD_CTRL:			return !alt && !shift && ctrl;
		case KEYMOD_ALTSHIFT:		return alt && shift && !ctrl;
		case KEYMOD_CTRLALT:		return alt && !shift && ctrl;
		case KEYMOD_CTRLSHIFT:		return !alt && shift && ctrl;
		case KEYMOD_ALTSHIFTCTRL:	return alt && shift && ctrl;
		case KEYMOD_NONE:			return !alt && !shift && !ctrl;
		}
		return false;
	}




	KeyboardDeviceDriver::KeyboardDeviceDriverAdaptor::KeyboardDeviceDriverAdaptor( KeyboardDeviceDriver* driver, KeyboardButton button )
	{
		this->driver = driver;
		this->button = button;
		this->lastValue = false;
	}

	DigitalState KeyboardDeviceDriver::KeyboardDeviceDriverAdaptor::getData()
	{
		bool b = driver->isPressed(button);
		DigitalState returnValue;

		if(b && !lastValue)
			returnValue = TOGGLE_ON;
		else if(b && lastValue)
			returnValue = ON;
		else if(!b && !lastValue)
			returnValue = OFF;
		else if(!b && lastValue)
			returnValue = TOGGLE_OFF;
		else
			logger<<"Unknown digital device values? The universe is collapsing"<<Log::newline;
		lastValue = b;
		return returnValue;
	}
}