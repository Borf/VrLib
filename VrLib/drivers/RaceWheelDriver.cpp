#include <VrLib/drivers/RaceWheelDriver.h>
#include <VrLib/Log.h>

#include <windows.h>
#include "RaceWheelDriver.h"

#include <stdio.h>


namespace vrlib
{
#define SAFE_RELEASE(p)     { if(p) { (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }

	bool g_bFilterOutXinputDevices;

	LPDIRECTINPUT8          g_pDI;
	cRaceWheelDriver::XINPUT_DEVICE_NODE*  g_pXInputDeviceList;
	unsigned int            device_counter;
	LPDIRECTINPUTDEVICE8    g_pJoystick;



	DeviceDriverAdaptor* cRaceWheelDriver::getAdaptor(std::string options)
	{
		WheelButtons  pressedButton;

		if (options == "upperLeft")
			return new WheelDeviceDriverAdaptor(this, UpperLeft);
		else if (options == "upperRight")
			return new WheelDeviceDriverAdaptor(this, UpperRight);
		else if (options == "l1")
			return new WheelDeviceDriverAdaptor(this, L1);
		else if (options == "l3")
			return new WheelDeviceDriverAdaptor(this, L3);
		else if (options == "r1")
			return new WheelDeviceDriverAdaptor(this, R1);
		else if (options == "r3")
			return new WheelDeviceDriverAdaptor(this, R3);
		else if (options == "select")
			return new WheelDeviceDriverAdaptor(this, Select);
		else if (options == "start")
			return new WheelDeviceDriverAdaptor(this, Start);
		else if (options == "square")
			return new WheelDeviceDriverAdaptor(this, Square);
		else if (options == "triangle")
			return new WheelDeviceDriverAdaptor(this, Triangle);
		else if (options == "circle")
			return new WheelDeviceDriverAdaptor(this, Circle);
		else if (options == "x")
			return new WheelDeviceDriverAdaptor(this, X);
		else if (options == "xAxis")
			return new WheelAxisDeviceDriverAdaptor(this, Xaxis);
		else if (options == "yAxis")
			return new WheelAxisDeviceDriverAdaptor(this, Yaxis);
		else if (options == "dPad")
			return new WheelAxisDeviceDriverAdaptor(this, Dpad);
		else
			logger << "Invalid  " + options << Log::newline;

		return NULL;
	}

	void cRaceWheelDriver::wheelDown(WheelButtons button)
	{
		if (button == UpperLeft)
			upperLeft = true;
		else if (button == UpperRight)
			upperRight = true;
		else if (button == L1)
			l1 = true;
		else if (button == L3)
			l3 = true;
		else if (button == R1)
			r1 = true;
		else if (button == R3)
			r3 = true;
		else if (button == Select)
			select = true;
		else if (button == Start)
			start = true;
		else if (button == Square)
			square = true;
		else if (button == Triangle)
			triangle = true;
		else if (button == Circle)
			circle = true;
		else if (button == X)
			x = true;
	}


	void cRaceWheelDriver::wheelUp(WheelButtons button)
	{
		if (button == UpperLeft)
			upperLeft = false;
		else if (button == UpperRight)
			upperRight = false;
		else if (button == L1)
			l1 = false;
		else if (button == L3)
			l3 = false;
		else if (button == R1)
			r1 = false;
		else if (button == R3)
			r3 = false;
		else if (button == Select)
			select = false;
		else if (button == Start)
			start = false;
		else if (button == Square)
			square = false;
		else if (button == Triangle)
			triangle = false;
		else if (button == Circle)
			circle = false;
		else if (button == X)
			x = false;
	}

	bool cRaceWheelDriver::isPressed(WheelButtons button)
	{
		if (button == UpperLeft)
			return upperLeft;
		else if (button == UpperRight)
			return upperRight;
		else if (button == L1)
			return l1;
		else if (button == L3)
			return l3;
		else if (button == R1)
			return r1;
		else if (button == R3)
			return r3;
		else if (button == Select)
			return select;
		else if (button == Start)
			return start;
		else if (button == Square)
			return square;
		else if (button == Triangle)
			return triangle;
		else if (button == Circle)
			return circle;
		else if (button == X)
			return x;
		logger << "Unknown button" << Log::newline;
		return false;
	}

	float cRaceWheelDriver::getAxis(WheelAxis axis)
	{
		if (axis == Xaxis)
			return xAxis;
		else if (axis == Yaxis)
			return yAxis;
		else if (axis == Dpad)
			return dPad;
		logger << "Unknown axis" << Log::newline;
		return 0.0f;
	}

	void cRaceWheelDriver::setAxis(WheelAxis axis, float value)
	{
		if (axis == Xaxis)
			xAxis = value;
		else if (axis == Yaxis)
			yAxis = value;
		else if (axis == Dpad)
			dPad = value;
	}


	//-----------------------------------------------------------------------------
	// Enum each PNP device using WMI and check each device ID to see if it contains 
	// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it’s an XInput device
	// Unfortunately this information can not be found by just using DirectInput.
	// Checking against a VID/PID of 0x028E/0x045E won't find 3rd party or future 
	// XInput devices.
	//
	// This function stores the list of xinput devices in a linked list 
	// at g_pXInputDeviceList, and IsXInputDevice() searchs that linked list
	//-----------------------------------------------------------------------------
	HRESULT cRaceWheelDriver::SetupForIsXInputDevice()
	{
		IWbemServices* pIWbemServices = NULL;
		IEnumWbemClassObject* pEnumDevices = NULL;
		IWbemLocator* pIWbemLocator = NULL;
		IWbemClassObject* pDevices[20] = { 0 };
		BSTR bstrDeviceID = NULL;
		BSTR bstrClassName = NULL;
		BSTR bstrNamespace = NULL;
		DWORD uReturned = 0;
		bool bCleanupCOM = false;
		UINT iDevice = 0;
		VARIANT var;
		HRESULT hr;

		// CoInit if needed
		hr = CoInitialize(NULL);
		bCleanupCOM = SUCCEEDED(hr);

		// Create WMI
		hr = CoCreateInstance(__uuidof(WbemLocator),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(IWbemLocator),
			(LPVOID*)&pIWbemLocator);
		if (FAILED(hr) || pIWbemLocator == NULL)
			goto LCleanup;

		// Create BSTRs for WMI
		bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == NULL) goto LCleanup;
		bstrDeviceID = SysAllocString(L"DeviceID");           if (bstrDeviceID == NULL)  goto LCleanup;
		bstrClassName = SysAllocString(L"Win32_PNPEntity");    if (bstrClassName == NULL) goto LCleanup;

		// Connect to WMI 
		hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
			0L, NULL, NULL, &pIWbemServices);
		if (FAILED(hr) || pIWbemServices == NULL)
			goto LCleanup;

		// Switch security level to IMPERSONATE
		CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
			RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0);

		// Get list of Win32_PNPEntity devices
		hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
		if (FAILED(hr) || pEnumDevices == NULL)
			goto LCleanup;

		// Loop over all devices
		for (;;)
		{
			// Get 20 at a time
			hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
			if (FAILED(hr))
				goto LCleanup;
			if (uReturned == 0)
				break;

			for (iDevice = 0; iDevice < uReturned; iDevice++)
			{
				if (!pDevices[iDevice])
					continue;

				// For each device, get its device ID
				hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
				if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
				{
					// Check if the device ID contains "IG_".  If it does, then it’s an XInput device
					// Unfortunately this information can not be found by just using DirectInput 
					if (wcsstr(var.bstrVal, L"IG_"))
					{
						// If it does, then get the VID/PID from var.bstrVal
						DWORD dwPid = 0, dwVid = 0;
						WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
						if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
							dwVid = 0;
						WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
						if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
							dwPid = 0;

						DWORD dwVidPid = MAKELONG(dwVid, dwPid);

						// Add the VID/PID to a linked list
						XINPUT_DEVICE_NODE* pNewNode = new XINPUT_DEVICE_NODE;
						if (pNewNode)
						{
							pNewNode->dwVidPid = dwVidPid;
							pNewNode->pNext = g_pXInputDeviceList;
							g_pXInputDeviceList = pNewNode;
						}
					}
				}
				SAFE_RELEASE(pDevices[iDevice]);
			}
		}

	LCleanup:
		if (bstrNamespace)
			SysFreeString(bstrNamespace);
		if (bstrDeviceID)
			SysFreeString(bstrDeviceID);
		if (bstrClassName)
			SysFreeString(bstrClassName);
		for (iDevice = 0; iDevice < 20; iDevice++)
			SAFE_RELEASE(pDevices[iDevice]);
		SAFE_RELEASE(pEnumDevices);
		SAFE_RELEASE(pIWbemLocator);
		SAFE_RELEASE(pIWbemServices);

		return hr;
	}



	//-----------------------------------------------------------------------------
	// Returns true if the DirectInput device is also an XInput device.
	// Call SetupForIsXInputDevice() before, and CleanupForIsXInputDevice() after
	//-----------------------------------------------------------------------------
	bool IsXInputDevice(const GUID* pGuidProductFromDirectInput)
	{
		// Check each xinput device to see if this device's vid/pid matches
		cRaceWheelDriver::XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
		while (pNode)
		{
			if (pNode->dwVidPid == pGuidProductFromDirectInput->Data1)
				return true;
			pNode = pNode->pNext;
		}

		return false;
	}

	//-----------------------------------------------------------------------------
	// Name: EnumJoysticksCallback()
	// Desc: Called once for each enumerated joystick. If we find one, create a
	//       device interface on it so we can play with it.
	//-----------------------------------------------------------------------------
	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
		VOID* pContext)
	{
		cRaceWheelDriver::DI_ENUM_CONTEXT* pEnumContext = (cRaceWheelDriver::DI_ENUM_CONTEXT*)pContext;
		HRESULT hr;

		if (g_bFilterOutXinputDevices && IsXInputDevice(&pdidInstance->guidProduct))
			return DIENUM_CONTINUE;

		// Skip anything other than the perferred joystick device as defined by the control panel.  
		// Instead you could store all the enumerated joysticks and let the user pick.
		if (pEnumContext->bPreferredJoyCfgValid &&
			!IsEqualGUID(pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance))
			return DIENUM_CONTINUE;

		// Obtain an interface to the enumerated joystick.
		hr = g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, NULL);

		// If it failed, then we can't use this joystick. (Maybe the user unplugged
		// it while we were in the middle of enumerating it.)
		if (FAILED(hr))
			return DIENUM_CONTINUE;

		// Stop enumeration. Note: we're just taking the first joystick we get. You
		// could store all the enumerated joysticks and let the user pick.
		return DIENUM_STOP;
	}

	//-----------------------------------------------------------------------------
	// Name: EnumObjectsCallback()
	// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
	//       joystick. This function enables user interface elements for objects
	//       that are found to exist, and scales axes min/max values.
	//-----------------------------------------------------------------------------
	BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
		VOID* pContext)
	{
		HWND hDlg = (HWND)pContext;

		static int nSliderCount = 0;  // Number of returned slider controls
		static int nPOVCount = 0;     // Number of returned POV controls

		// For axes that are returned, set the DIPROP_RANGE property for the
		// enumerated axis in order to scale min/max values.
		if (pdidoi->dwType & DIDFT_AXIS)
		{
			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
			diprg.lMin = -1000;
			diprg.lMax = +1000;

			// Set the range for the axis
			if (FAILED(g_pJoystick->SetProperty(DIPROP_RANGE, &diprg.diph)))
				return DIENUM_STOP;

		}


		// Set the UI to reflect what objects the joystick supports
		if (pdidoi->guidType == GUID_XAxis)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_X_AXIS), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_X_AXIS_TEXT), TRUE);
		}
		if (pdidoi->guidType == GUID_YAxis)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_Y_AXIS), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_Y_AXIS_TEXT), TRUE);
		}
		if (pdidoi->guidType == GUID_ZAxis)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_Z_AXIS), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_Z_AXIS_TEXT), TRUE);
		}
		if (pdidoi->guidType == GUID_RxAxis)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_X_ROT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_X_ROT_TEXT), TRUE);
		}
		if (pdidoi->guidType == GUID_RyAxis)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_Y_ROT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_Y_ROT_TEXT), TRUE);
		}
		if (pdidoi->guidType == GUID_RzAxis)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_Z_ROT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_Z_ROT_TEXT), TRUE);
		}
		if (pdidoi->guidType == GUID_Slider)
		{
			switch (nSliderCount++)
			{
			case 0:
				EnableWindow(GetDlgItem(hDlg, IDC_SLIDER0), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_SLIDER0_TEXT), TRUE);
				break;

			case 1:
				EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1_TEXT), TRUE);
				break;
			}
		}
		if (pdidoi->guidType == GUID_POV)
		{
			switch (nPOVCount++)
			{
			case 0:
				EnableWindow(GetDlgItem(hDlg, IDC_POV0), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_POV0_TEXT), TRUE);
				break;

			case 1:
				EnableWindow(GetDlgItem(hDlg, IDC_POV1), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_POV1_TEXT), TRUE);
				break;

			case 2:
				EnableWindow(GetDlgItem(hDlg, IDC_POV2), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_POV2_TEXT), TRUE);
				break;

			case 3:
				EnableWindow(GetDlgItem(hDlg, IDC_POV3), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_POV3_TEXT), TRUE);
				break;
			}
		}

		return DIENUM_CONTINUE;
	}

	//-----------------------------------------------------------------------------
	// Name: UpdateInputState()
	// Desc: Get the input device's state and display it.
	//-----------------------------------------------------------------------------
	HRESULT cRaceWheelDriver::UpdateInputState(HWND hDlg)
	{
		HRESULT hr;
		TCHAR strText[512] = { 0 }; // Device state text
		DIJOYSTATE2 js;           // DInput joystick state 

		if (NULL == g_pJoystick)
			return S_OK;

		// Poll the device to read the current state
		hr = g_pJoystick->Poll();
		if (FAILED(hr))
		{
			// DInput is telling us that the input stream has been
			// interrupted. We aren't tracking any state between polls, so
			// we don't have any special reset that needs to be done. We
			// just re-acquire and try again.
			hr = g_pJoystick->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = g_pJoystick->Acquire();

			// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
			// may occur when the app is minimized or in the process of 
			// switching, so just try again later 
			return S_OK;
		}

		// Get the input's device state
		if (FAILED(hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
			return hr; // The device should have been acquired during the Poll()
		/*
		// Display joystick state to dialog

		// Axes
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.lX );
		SetWindowText( GetDlgItem( hDlg, IDC_X_AXIS ), strText );
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.lY );
		SetWindowText( GetDlgItem( hDlg, IDC_Y_AXIS ), strText );
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.lZ );
		SetWindowText( GetDlgItem( hDlg, IDC_Z_AXIS ), strText );
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.lRx );
		SetWindowText( GetDlgItem( hDlg, IDC_X_ROT ), strText );
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.lRy );
		SetWindowText( GetDlgItem( hDlg, IDC_Y_ROT ), strText );
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.lRz );
		SetWindowText( GetDlgItem( hDlg, IDC_Z_ROT ), strText );

		// Slider controls
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.rglSlider[0] );
		SetWindowText( GetDlgItem( hDlg, IDC_SLIDER0 ), strText );
		_stprintf_s( strText, 512, TEXT( "%ld" ), js.rglSlider[1] );
		SetWindowText( GetDlgItem( hDlg, IDC_SLIDER1 ), strText );

		// Points of view
		_stprintf_s( strText, 512, TEXT( "%lu" ), js.rgdwPOV[0] );
		SetWindowText( GetDlgItem( hDlg, IDC_POV0 ), strText );
		_stprintf_s( strText, 512, TEXT( "%lu" ), js.rgdwPOV[1] );
		SetWindowText( GetDlgItem( hDlg, IDC_POV1 ), strText );
		_stprintf_s( strText, 512, TEXT( "%lu" ), js.rgdwPOV[2] );
		SetWindowText( GetDlgItem( hDlg, IDC_POV2 ), strText );
		_stprintf_s( strText, 512, TEXT( "%lu" ), js.rgdwPOV[3] );
		SetWindowText( GetDlgItem( hDlg, IDC_POV3 ), strText );
		*/
		setAxis(Xaxis, js.lX);
		setAxis(Yaxis, js.lY);
		setAxis(Dpad, js.rgdwPOV[0]);

		// Fill up text with which buttons are pressed
		_tcscpy_s(strText, 512, TEXT(""));
		for (int i = 0; i < 128; i++)
		{
			if (js.rgbButtons[i] & 0x80)
				wheelDown((cRaceWheelDriver::WheelButtons) i);
			else
				wheelUp((cRaceWheelDriver::WheelButtons) i);
		}

		//SetWindowText( GetDlgItem( windowHandle, IDC_BUTTONS ), strText );

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: FreeDirectInput()
	// Desc: Initialize the DirectInput variables.
	//-----------------------------------------------------------------------------
	VOID cRaceWheelDriver::FreeDirectInput()
	{
		// Unacquire the device one last time just in case 
		// the app tried to exit while the device is still acquired.
		if (g_pJoystick)
			g_pJoystick->Unacquire();

		// Release any DirectInput objects.
		SAFE_RELEASE(g_pJoystick);
		SAFE_RELEASE(g_pDI);
	}

	//-----------------------------------------------------------------------------
	// Name: InitDirectInput()
	// Desc: Initialize the DirectInput variables.
	//-----------------------------------------------------------------------------
	HRESULT cRaceWheelDriver::InitDirectInput(HWND hDlg)
	{
		HRESULT hr;

		// Register with the DirectInput subsystem and get a pointer
		// to a IDirectInput interface we can use.
		// Create a DInput object
		if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
			IID_IDirectInput8, (VOID**)&g_pDI, NULL)))
			return hr;


		if (g_bFilterOutXinputDevices)
			SetupForIsXInputDevice();

		DIJOYCONFIG PreferredJoyCfg = { 0 };
		DI_ENUM_CONTEXT enumContext;
		enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
		enumContext.bPreferredJoyCfgValid = false;

		IDirectInputJoyConfig8* pJoyConfig = NULL;
		if (FAILED(hr = g_pDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
			return hr;

		PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);
		if (SUCCEEDED(pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE))) // This function is expected to fail if no joystick is attached
			enumContext.bPreferredJoyCfgValid = true;
		SAFE_RELEASE(pJoyConfig);

		// Look for a simple joystick we can use for this sample program.
		if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
			EnumJoysticksCallback,
			&enumContext, DIEDFL_ATTACHEDONLY)))
			return hr;

		if (g_bFilterOutXinputDevices)
			CleanupForIsXInputDevice();

		// Make sure we got a joystick

		if (NULL == g_pJoystick)
		{
			return NULL;
		}
		/*
		MessageBox( NULL, TEXT( "Joystick not found. The sample will now exit." ),
		TEXT( "DirectInput Sample" ),
		MB_ICONERROR | MB_OK );
		EndDialog( hDlg, 0 );
		return S_OK;
		}*/

		// Set the data format to "simple joystick" - a predefined data format 
		//
		// A data format specifies which controls on a device we are interested in,
		// and how they should be reported. This tells DInput that we will be
		// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		if (FAILED(hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2)))
			return hr;

		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.

		if (FAILED(hr = g_pJoystick->SetCooperativeLevel(hDlg, DISCL_EXCLUSIVE |
			DISCL_FOREGROUND)))
			return hr;

		// Enumerate the joystick objects. The callback function enabled user
		// interface elements for objects that are found, and sets the min/max
		// values property for discovered axes.
		if (FAILED(hr = g_pJoystick->EnumObjects(EnumObjectsCallback,
			(VOID*)hDlg, DIDFT_ALL)))
			return hr;

		Joystick = g_pJoystick;

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Cleanup needed for IsXInputDevice()
	//-----------------------------------------------------------------------------
	void cRaceWheelDriver::CleanupForIsXInputDevice()
	{
		// Cleanup linked list
		XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
		while (pNode)
		{
			XINPUT_DEVICE_NODE* pDelete = pNode;
			pNode = pNode->pNext;
			SAFE_DELETE(pDelete);
		}
	}

	cRaceWheelDriver::cRaceWheelDriver(HWND _windowHandle)
	{
		device_counter = 0;
		g_pEffect = NULL;
		g_pDI = NULL;
		g_pJoystick = NULL;
		Joystick = NULL;
		g_bFilterOutXinputDevices = false;
		g_pXInputDeviceList = NULL;
		windowHandle = _windowHandle;

		xAxis = 0.0f;
		yAxis = 0.0f;
		dPad = 0.0f;
		upperLeft = false;
		upperRight = false;
		l1 = false;
		l3 = false;
		r1 = false;
		r3 = false;
		select = false;
		start = false;
		square = false;
		triangle = false;
		circle = false;
		x = false;
	}

	cRaceWheelDriver::~cRaceWheelDriver()
	{
		close();
	}

	HRESULT
		cRaceWheelDriver::deviceName(char* name)
	{
		HRESULT hr;
		DIDEVICEINSTANCE device;

		ZeroMemory(&device, sizeof(device));
		device.dwSize = sizeof(device);

		if (!g_pDI || !g_pJoystick) {
			return E_INVALIDARG;
		}

		if (FAILED(hr = g_pJoystick->GetDeviceInfo(&device))) {
			return hr;
		}

		strncpy(name, device.tszProductName, MAX_PATH);

		return hr;
	}

	//-----------------------------------------------------------------------------
	// Name: EnumAxesCallback()
	// Desc: Callback function for enumerating the axes on a joystick and counting
	//       each force feedback enabled axis
	//-----------------------------------------------------------------------------
	BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
		VOID* pContext)
	{
		DWORD* pdwNumForceFeedbackAxis = (DWORD*)pContext;

		if ((pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0)
			(*pdwNumForceFeedbackAxis)++;

		return DIENUM_CONTINUE;
	}

	HRESULT
		cRaceWheelDriver::open()
	{

		HRESULT hr;

		// Create a DirectInput device
		if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(VOID**)&g_pDI, NULL))) {
			return hr;
		}

		// Look for the first simple joystick we can find.
		if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, vrlib::enumCallback,
			(LPVOID)this, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK))) {
			return hr;
		}

		// Make sure we got a joystick
		if (g_pJoystick == NULL) {
			return E_FAIL;
		}

		// Set the data format to "simple joystick" - a predefined data format 
		//
		// A data format specifies which controls on a device we are interested in,
		// and how they should be reported. This tells DInput that we will be
		// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		if (FAILED(hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2))) {
			return hr;
		}

		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.
		if (FAILED(hr = g_pJoystick->SetCooperativeLevel(windowHandle, DISCL_EXCLUSIVE | DISCL_FOREGROUND))) {
			return hr;
		}

		DIPROPDWORD dipdw;
		// Since we will be playing force feedback effects, we should disable the
		// auto-centering spring.
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = FALSE;

		if (FAILED(hr = g_pJoystick->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph)))
			return hr;

		// Enumerate and count the axes of the joystick 
		if (FAILED(hr = g_pJoystick->EnumObjects(EnumAxesCallback,
			(VOID*)&g_dwNumForceFeedbackAxis, DIDFT_AXIS)))
			return hr;

		// This simple sample only supports one or two axis joysticks
		if (g_dwNumForceFeedbackAxis > 2)
			g_dwNumForceFeedbackAxis = 2;

		DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
		LONG rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf = { 0 };
		cf.lMagnitude = 0;

		DIEFFECT eff;
		ZeroMemory(&eff, sizeof(eff));
		eff.dwSize = sizeof(DIEFFECT);
		eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration = INFINITE;
		eff.dwSamplePeriod = 0;
		eff.dwGain = DI_FFNOMINALMAX;
		eff.dwTriggerButton = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes = g_dwNumForceFeedbackAxis;
		eff.rgdwAxes = rgdwAxes;
		eff.rglDirection = rglDirection;
		eff.lpEnvelope = 0;
		eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;
		eff.dwStartDelay = 0;

		// Create the prepared effect
		if (FAILED(hr = g_pJoystick->CreateEffect(GUID_ConstantForce,
			&eff, &g_pEffect, NULL)))
		{
			return hr;
		}

		if (NULL == g_pEffect)
			printf("Fuck effect null!!!!!");

		return hr;
	}

	HRESULT cRaceWheelDriver::setEffect(int _magnitude)
	{

		if (NULL == g_pEffect)
			return E_FAIL;

		//LONG rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf;

		//rglDirection[0] = -7500;
		//rglDirection[1] = 1500;
		cf.lMagnitude = _magnitude;

		DIEFFECT eff;
		ZeroMemory(&eff, sizeof(eff));
		eff.dwSize = sizeof(DIEFFECT);
		eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.cAxes = g_dwNumForceFeedbackAxis;
		//eff.rglDirection = rglDirection;
		eff.lpEnvelope = 0;
		eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;
		eff.dwStartDelay = 0;

		// Now set the new parameters and start the effect immediately.
		return g_pEffect->SetParameters(&eff, DIEP_DIRECTION |
			DIEP_TYPESPECIFICPARAMS |
			DIEP_START);

		//return S_OK;
	}

	HRESULT
		cRaceWheelDriver::close()
	{
		if (g_pJoystick) {
			g_pJoystick->Unacquire();
		}

		SAFE_RELEASE(g_pJoystick);
		SAFE_RELEASE(g_pDI);

		return S_OK;
	}

	HRESULT
		cRaceWheelDriver::poll(DIJOYSTATE2 *js)
	{
		HRESULT hr;

		if (g_pJoystick == NULL) {
			return S_OK;
		}

		// Poll the device to read the current state
		hr = g_pJoystick->Poll();
		if (FAILED(hr)) {

			// DirectInput is telling us that the input stream has been
			// interrupted.  We aren't tracking any state between polls, so we
			// don't have any special reset that needs to be done.  We just
			// re-acquire and try again.
			hr = g_pJoystick->Acquire();
			while (hr == DIERR_INPUTLOST) {
				hr = g_pJoystick->Acquire();
			}

			// If we encounter a fatal error, return failure.
			if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
				return E_FAIL;
			}

			// If another application has control of this device, return success.
			// We'll just have to wait our turn to use the joystick.
			if (hr == DIERR_OTHERAPPHASPRIO) {
				return S_OK;
			}
		}

		// Get the input's device state
		if (FAILED(hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), js))) {
			return hr;
		}

		return S_OK;
	}

	BOOL CALLBACK
		enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
	{
		// If this is the requested device ID ...
		// set to 0 now because we only use 1 joystick
		if (device_counter == 0) {

			// Obtain an interface to the enumerated joystick.  Stop the enumeration
			// if the requested device was created successfully.
			if (SUCCEEDED(g_pDI->CreateDevice(instance->guidInstance, &g_pJoystick, NULL))) {
				return DIENUM_STOP;
			}
		}

		// Otherwise, increment the device counter and continue with
		// the device enumeration.
		device_counter++;

		return DIENUM_CONTINUE;
	}
	/*
	BOOL CALLBACK
	enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
	{
	if (context != NULL) {
	return ((cRaceWheelDriver *)context)->enumCallback(instance, context);
	} else {
	return DIENUM_STOP;
	}
	}*/

	unsigned int
		cRaceWheelDriver::deviceCount()
	{
		unsigned int counter = 0;
		LPDIRECTINPUT8 di = NULL;
		HRESULT hr;

		if (SUCCEEDED(hr = DirectInput8Create(GetModuleHandle(NULL),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(VOID**)&di, NULL))) {
			di->EnumDevices(DI8DEVCLASS_GAMECTRL, vrlib::countCallback,
				&counter, DIEDFL_ATTACHEDONLY);
		}

		return counter;
	}

	BOOL CALLBACK
		countCallback(const DIDEVICEINSTANCE* instance, VOID* counter)
	{
		if (counter != NULL) {
			unsigned int *tmpCounter = (unsigned int *)counter;
			(*tmpCounter)++;
			counter = tmpCounter;
		}

		return DIENUM_CONTINUE;
	}

	cRaceWheelDriver::WheelAxisDeviceDriverAdaptor::WheelAxisDeviceDriverAdaptor(cRaceWheelDriver* driver, WheelAxis id)
	{
		this->driver = driver;
		this->axis = id;
	}

	float cRaceWheelDriver::WheelAxisDeviceDriverAdaptor::getData()
	{
		return driver->getAxis(axis);
	}

	cRaceWheelDriver::WheelDeviceDriverAdaptor::WheelDeviceDriverAdaptor(cRaceWheelDriver* driver, WheelButtons id)
	{
		this->driver = driver;
		this->buttons = id;
	}

	DigitalState cRaceWheelDriver::WheelDeviceDriverAdaptor::getData()
	{
		bool b = driver->isPressed(buttons);
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
		//return driver->data[(int)buttons];
	}
}