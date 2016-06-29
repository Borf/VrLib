#pragma once
#ifdef WIN32

#include <VrLib/Device.h>

#include <map>
#include <string>

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <commctrl.h>
#include <basetsd.h>

#pragma warning(push)
#pragma warning(disable:6000 28251)
#include <dinput.h>
#pragma warning(pop)

#include <dinputd.h>
#include <assert.h>
#include <oleauto.h>
#include <shellapi.h>
#include <WbemCli.h>

//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by Joystick.rc
//
#define IDI_MAIN                        102
#define IDD_JOYST_IMM                   103
#define IDR_ACCELERATOR1                103
#define IDC_CLOSE                       1001
#define IDC_X_AXIS                      1010
#define IDC_Y_AXIS                      1011
#define IDC_Z_AXIS                      1012
#define IDC_X_AXIS_TEXT                 1013
#define IDC_Y_AXIS_TEXT                 1014
#define IDC_Z_AXIS_TEXT                 1015
#define IDC_X_ROT_TEXT                  1016
#define IDC_Y_ROT_TEXT                  1017
#define IDC_Z_ROT_TEXT                  1018
#define IDC_SLIDER0_TEXT                1019
#define IDC_X_ROT                       1020
#define IDC_Y_ROT                       1021
#define IDC_Z_ROT                       1022
#define IDC_SLIDER1_TEXT                1023
#define IDC_POV0_TEXT                   1024
#define IDC_POV1_TEXT                   1025
#define IDC_POV2_TEXT                   1026
#define IDC_POV3_TEXT                   1027
#define IDC_SLIDER0                     1030
#define IDC_SLIDER1                     1031
#define IDC_POV                         1040
#define IDC_POV0                        1040
#define IDC_BUTTONS                     1041
#define IDC_POV1                        1042
#define IDC_POV2                        1043
#define IDC_POV3                        1044

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        104
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1025
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif


namespace vrlib
{
	class RaceWheelDriver : public DeviceDriver
	{
	private:
		enum WheelButtons
		{//important, the order of the buttons below determine which buttons they are! (the number corresponds with the button number)
			Triangle, Circle, X, Square, L1, R1, UpperLeft, UpperRight, Select, Start, L3, R3,
		};
		enum WheelAxis
		{
			Xaxis, Yaxis, Dpad,
		};

		class WheelDeviceDriverAdaptor : public DigitalDeviceDriverAdaptor
		{
			RaceWheelDriver* driver;
			WheelButtons buttons;
			bool lastValue;
		public:
			WheelDeviceDriverAdaptor(RaceWheelDriver*, WheelButtons id);
			virtual DigitalState getData();
		};

		class WheelAxisDeviceDriverAdaptor : public AnalogDeviceDriverAdaptor
		{
			RaceWheelDriver* driver;
			WheelAxis axis;
		public:
			WheelAxisDeviceDriverAdaptor(RaceWheelDriver*, WheelAxis id);
			virtual float getData();
		};

	public:

		struct XINPUT_DEVICE_NODE
		{
			DWORD dwVidPid;
			XINPUT_DEVICE_NODE* pNext;
		};

		struct DI_ENUM_CONTEXT
		{
			DIJOYCONFIG* pPreferredJoyCfg;
			bool bPreferredJoyCfgValid;
		};

		LPDIRECTINPUTEFFECT     g_pEffect;
		DWORD                   g_dwNumForceFeedbackAxis;
		HWND					windowHandle;
		LPDIRECTINPUTDEVICE8    Joystick;

		RaceWheelDriver(HWND _windowHandle);
		~RaceWheelDriver();

		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
		//virtual void update();
		float data[10];
		float xAxis, yAxis, dPad;

		void setAxis(WheelAxis axis, float value);
		void wheelDown(WheelButtons button);
		void wheelUp(WheelButtons button);

		float getAxis(WheelAxis axis);
		bool isPressed(WheelButtons button);
		bool upperLeft, upperRight, l1, l3, r1, r3, select, start, square, triangle, circle, x;

		HRESULT deviceName(char* name);
		VOID FreeDirectInput();
		HRESULT UpdateInputState(HWND hDlg);
		/*BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
										 VOID* pContext );
										 BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
										 VOID* pContext );*/
		//bool IsXInputDevice( const GUID* pGuidProductFromDirectInput );
		void CleanupForIsXInputDevice();
		HRESULT SetupForIsXInputDevice();
		HRESULT InitDirectInput(HWND hDlg);
		HRESULT open();
		HRESULT close();

		HRESULT poll(DIJOYSTATE2 *js);
		HRESULT setEffect(int _magnitude);
		unsigned int            device_counter;
		//BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);

		// Device Querying
		static unsigned int deviceCount();
	};

	BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);
	BOOL CALLBACK countCallback(const DIDEVICEINSTANCE* instance, VOID* counter);
	/*
	BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
	VOID* pContext );
	BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
	VOID* pContext );
	*/
}

#endif