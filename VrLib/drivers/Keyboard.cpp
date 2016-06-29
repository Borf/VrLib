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
		{ KeyboardDeviceDriver::KEY_PAGEUP,			"pageup",		VK_PRIOR },
		{ KeyboardDeviceDriver::KEY_PAGEDOWN,		"pagedown",		VK_NEXT },
		{ KeyboardDeviceDriver::KEY_HOME,			"home",			VK_HOME },
		{ KeyboardDeviceDriver::KEY_END,			"end",			VK_END },
		{ KeyboardDeviceDriver::KEY_INSERT,			"insert",		VK_INSERT },
		{ KeyboardDeviceDriver::KEY_DELETE,			"delete",		VK_DELETE },
		{ KeyboardDeviceDriver::KEY_F1		,		"f1",			VK_F1 },
		{ KeyboardDeviceDriver::KEY_F2		,		"f2",			VK_F2 },
		{ KeyboardDeviceDriver::KEY_F3		,		"f3",			VK_F3 },
		{ KeyboardDeviceDriver::KEY_F4		,		"f4",			VK_F4 },
		{ KeyboardDeviceDriver::KEY_F5		,		"f5",			VK_F5 },
		{ KeyboardDeviceDriver::KEY_F6		,		"f6",			VK_F6 },
		{ KeyboardDeviceDriver::KEY_F7		,		"f7",			VK_F7 },
		{ KeyboardDeviceDriver::KEY_F8		,		"f8",			VK_F8 },
		{ KeyboardDeviceDriver::KEY_F9		,		"f9",			VK_F9 },
		{ KeyboardDeviceDriver::KEY_F10		,		"f10",			VK_F10 },
		{ KeyboardDeviceDriver::KEY_F11		,		"f11",			VK_F11 },
		{ KeyboardDeviceDriver::KEY_F12		,		"f12",			VK_F12 },

		{ KeyboardDeviceDriver::KEY_NUMPAD_0,		"numpad0",		VK_NUMPAD0 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_1,		"numpad1",		VK_NUMPAD1 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_2,		"numpad2",		VK_NUMPAD2 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_3,		"numpad3",		VK_NUMPAD3 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_4,		"numpad4",		VK_NUMPAD4 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_5,		"numpad5",		VK_NUMPAD5 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_6,		"numpad6",		VK_NUMPAD6 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_7,		"numpad7",		VK_NUMPAD7 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_8,		"numpad8",		VK_NUMPAD8 },
		{ KeyboardDeviceDriver::KEY_NUMPAD_9,		"numpad9",		VK_NUMPAD9 },
		/*{ KeyboardDeviceDriver::KEY_PLUS,			"+",			VK_OEM_PLUS },
		{ KeyboardDeviceDriver::KEY_MINUS,			"-",			VK_OEM_MINUS },*/

		{ KeyboardDeviceDriver::KEY_0		,		"0",			'0' },
		{ KeyboardDeviceDriver::KEY_1		,		"1",			'1' },
		{ KeyboardDeviceDriver::KEY_2		,		"2",			'2' },
		{ KeyboardDeviceDriver::KEY_3		,		"3",			'3' },
		{ KeyboardDeviceDriver::KEY_4		,		"4",			'4' },
		{ KeyboardDeviceDriver::KEY_5		,		"5",			'5' },
		{ KeyboardDeviceDriver::KEY_6		,		"6",			'6' },
		{ KeyboardDeviceDriver::KEY_7		,		"7",			'7' },
		{ KeyboardDeviceDriver::KEY_8		,		"8",			'8' },
		{ KeyboardDeviceDriver::KEY_9		,		"9",			'9' },

		{ KeyboardDeviceDriver::KEY_A		,		"a",			'A' },
		{ KeyboardDeviceDriver::KEY_B		,		"b",			'B' },
		{ KeyboardDeviceDriver::KEY_C		,		"c",			'C' },
		{ KeyboardDeviceDriver::KEY_D		,		"d",			'D' },
		{ KeyboardDeviceDriver::KEY_E		,		"e",			'E' },
		{ KeyboardDeviceDriver::KEY_F		,		"f",			'F' },
		{ KeyboardDeviceDriver::KEY_G		,		"g",			'G' },
		{ KeyboardDeviceDriver::KEY_H		,		"h",			'H' },
		{ KeyboardDeviceDriver::KEY_I		,		"i",			'I' },
		{ KeyboardDeviceDriver::KEY_J		,		"j",			'J' },
		{ KeyboardDeviceDriver::KEY_K		,		"k",			'K' },
		{ KeyboardDeviceDriver::KEY_L		,		"l",			'L' },
		{ KeyboardDeviceDriver::KEY_M		,		"m",			'M' },
		{ KeyboardDeviceDriver::KEY_N		,		"n",			'N' },
		{ KeyboardDeviceDriver::KEY_O		,		"o",			'O' },
		{ KeyboardDeviceDriver::KEY_P		,		"p",			'P' },
		{ KeyboardDeviceDriver::KEY_Q		,		"q",			'Q' },
		{ KeyboardDeviceDriver::KEY_R		,		"r",			'R' },
		{ KeyboardDeviceDriver::KEY_S		,		"s",			'S' },
		{ KeyboardDeviceDriver::KEY_T		,		"t",			'T' },
		{ KeyboardDeviceDriver::KEY_U		,		"u",			'U' },
		{ KeyboardDeviceDriver::KEY_V		,		"v",			'V' },
		{ KeyboardDeviceDriver::KEY_W		,		"w",			'W' },
		{ KeyboardDeviceDriver::KEY_X		,		"x",			'X' },
		{ KeyboardDeviceDriver::KEY_Y		,		"y",			'Y' },
		{ KeyboardDeviceDriver::KEY_Z		,		"z",			'Z' },
		{ KeyboardDeviceDriver::KEY_SPACEBAR,		"spacebar",		' ' },
		{ KeyboardDeviceDriver::KEY_TAB		,		"tab",			VK_TAB },

		{ KeyboardDeviceDriver::KEY_ARROW_UP,		"arrowup",		VK_UP },
		{ KeyboardDeviceDriver::KEY_ARROW_DOWN,		"arrowdown",	VK_DOWN },
		{ KeyboardDeviceDriver::KEY_ARROW_LEFT,		"arrowleft",	VK_LEFT },
		{ KeyboardDeviceDriver::KEY_ARROW_RIGHT,	"arrowright",	VK_RIGHT },


	


		{ KeyboardDeviceDriver::KEY_UNDEFINED,"UNDEFINED",0 }
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
			buttonsPressed[buttonLookup[button]] = true;
	}

	void KeyboardDeviceDriver::keyUp( int button )
	{
		if(buttonLookup.find(button) != buttonLookup.end())
			buttonsPressed[buttonLookup[button]] = false;
	}

	bool KeyboardDeviceDriver::isPressed( KeyboardButton button )
	{
		return buttonsPressed[button];
	}

	KeyboardDeviceDriver::KeyboardDeviceDriver()
	{
		for(int i = 0; i < sizeof(keyMapping) / sizeof(keymap); i++)
			buttonLookup[keyMapping[i].vk] = keyMapping[i].btn;
		for(int i = 0; i < KEY_UNDEFINED; i++)
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

		KeyboardButton btn = KEY_UNDEFINED;
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