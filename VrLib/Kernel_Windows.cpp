#ifdef WIN32

#include <WinSock2.h>
#include <windowsx.h>
#include <GL/glew.h>
#include <GL/wglew.h>

#include <VrLib/Kernel_Windows.h>
#include <VrLib/Log.h>
#include <VrLib/drivers/MouseButton.h>
#include <VrLib/drivers/Keyboard.h>
#include <VrLib/drivers/SimPosition.h>
#include <VrLib\drivers\RaceWheelDriver.h>

#ifdef WIN32	

//void GLAPIENTRY onDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void GLAPIENTRY onDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
#else
void onDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
#endif
{

	if (type == 33361) //VBO setdata
		return;
	printf("%s\n", message);
}
namespace vrlib
{
	KernelWindows::KernelWindows()
	{
		WSADATA WinsockData;
		if (WSAStartup(MAKEWORD(2, 2), &WinsockData) != 0)
			logger << "Fatal Error: Winsock Startup failed!" << Log::newline;


	}


	Kernel* Kernel::getInstance()
	{
		static KernelWindows* t = new KernelWindows();
		return t;
	}



	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		return ((KernelWindows*)Kernel::getInstance())->wndProc(hWnd, message, wParam, lParam);
	}

	bool middleButtonDown = false;
	int lastX;
	int lastY;

	LRESULT CALLBACK KernelWindows::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message) {
		case WM_SIZE: // If our window is resizing
		{
			windowWidth = LOWORD(lParam);
			windowHeight = HIWORD(lParam);
			break;
		}
		case WM_DESTROY:
		{
			KillTimer(hWnd, 0);
			raceWheelDriver->FreeDirectInput();
			PostQuitMessage(0);
			break;
		}
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);

			if (keyboardDriver && ((lParam >> 30) & 1) == 0)
				keyboardDriver->keyDown(wParam);
			break;
		case WM_KEYUP:
			if (keyboardDriver)
				keyboardDriver->keyUp(wParam);
			break;
		case WM_LBUTTONDOWN:
			if (mouseDriver)
				mouseDriver->mouseDown(MouseButtonDeviceDriver::Left);
			break;
		case WM_LBUTTONUP:
			if (mouseDriver)
				mouseDriver->mouseUp(MouseButtonDeviceDriver::Left);
			break;
		case WM_RBUTTONDOWN:
			if (mouseDriver)
				mouseDriver->mouseDown(MouseButtonDeviceDriver::Right);
			break;
		case WM_RBUTTONUP:
			if (mouseDriver)
				mouseDriver->mouseUp(MouseButtonDeviceDriver::Right);
			break;
		case WM_MBUTTONDOWN:
			middleButtonDown = true;
			lastX = GET_X_LPARAM(lParam);
			lastY = GET_Y_LPARAM(lParam);
			if (mouseDriver)
				mouseDriver->mouseDown(MouseButtonDeviceDriver::Middle);
			break;
		case WM_MBUTTONUP:
			middleButtonDown = false;
			if (mouseDriver)
				mouseDriver->mouseUp(MouseButtonDeviceDriver::Middle);
			break;
		case WM_MOUSEMOVE:
			if (middleButtonDown && simPositionDriver)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				simPositionDriver->mouseMove(x - lastX, y - lastY);
				lastX = x;
				lastY = y;
			}
			if (mouseDriver)
				mouseDriver->mouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

			break;
		case WM_ACTIVATE:
			if (raceWheelDriver == NULL)
				break;
			raceWheelDriver->InitDirectInput(hWnd);
			/*if( FAILED( raceWheelDriver->InitDirectInput( hWnd ) ) )
			{//todo show log msg
			MessageBox( NULL, TEXT( "Error Initializing DirectInput" ),
			TEXT( "DirectInput Sample" ), MB_ICONERROR | MB_OK );
			EndDialog( hWnd, 0 );
			}*/

			// Set a timer to go off 30 times a second. At every timer message
			// the input device will be read
			if (raceWheelDriver->Joystick != NULL)
			{
				SetTimer(hWnd, 0, 1000 / 30, NULL);

				if (WA_INACTIVE != wParam && raceWheelDriver->Joystick)
				{
					// Make sure the device is acquired, if we are gaining focus.
					raceWheelDriver->Joystick->Acquire();
				}

				return TRUE;
			}
			return FALSE;

		case WM_TIMER:
			// Update the input device every timer message
			if (FAILED(raceWheelDriver->UpdateInputState(hWnd)))
			{
				KillTimer(hWnd, 0);
				MessageBox(NULL, TEXT("Error Reading Input State. ") \
					TEXT("The sample will now exit."), TEXT("DirectInput Sample"),
					MB_ICONERROR | MB_OK);
				EndDialog(hWnd, TRUE);
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDCANCEL:
				EndDialog(hWnd, 0);
				return TRUE;
			}
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}



	void KernelWindows::createWindow()
	{
		//Open Window
		const char* title = localConfig["window"]["title"].asString().c_str();
		windowWidth = localConfig["window"]["width"].asInt();
		windowHeight = localConfig["window"]["height"].asInt();

		WNDCLASS windowClass;
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

		HGLRC hrc; // Rendering context

		HINSTANCE hInstance = GetModuleHandle(NULL);

		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = (WNDPROC)WndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = hInstance;
		windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = title;

		if (!RegisterClass(&windowClass)) {
			return;
		}

		hWnd = CreateWindowEx(dwExStyle, title, title, localConfig["window"]["border"].asBool() ? WS_OVERLAPPEDWINDOW : (WS_OVERLAPPED | WS_POPUP),
			localConfig["window"]["x"].asInt(), localConfig["window"]["y"].asInt(), windowWidth, windowHeight, NULL, NULL, hInstance, NULL);


		hdc = GetDC(hWnd); // Get the device context for our window

		PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window
		pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels
		pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)
		pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
		pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

		int nPixelFormat = ChoosePixelFormat(hdc, &pfd); // Check if our PFD is valid and get a pixel format back
		if (nPixelFormat == 0) // If it fails
			return;

		BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD
		if (!bResult) // If it fails
			return;

		HGLRC tempOpenGLContext = wglCreateContext(hdc); // Create an OpenGL 2.1 context for our device context
		wglMakeCurrent(hdc, tempOpenGLContext); // Make the OpenGL 2.1 context current and active


		hrc = tempOpenGLContext;

		GLenum error = glewInit(); // Enable GLEW
		if (error != GLEW_OK) // If GLEW fails
			return;

		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3
			WGL_CONTEXT_MINOR_VERSION_ARB, 2, // Set the MINOR version of OpenGL to 2
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
			0
		};

		int glVersion[2] = { -1, -1 }; // Set some default values for the version
		glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
		glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using

		logger << "Using OpenGL " << (const char*)glGetString(GL_VERSION) << Log::newline;
		logger << "Using OpenGL " << glVersion[0] << "." << glVersion[1] << Log::newline; // Output which version of OpenGL we are using

		if(!localConfig["window"].isMember("cursor") || localConfig["window"]["cursor"].asBool() == false)
			ShowCursor(FALSE);
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);


#ifdef __GLEW_H__
#ifdef GL_DEBUG_OUTPUT
#ifdef _DEBUG
		if (GLEW_ARB_debug_output)
		{
			glDebugMessageCallback(&onDebug, NULL);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
			glEnable(GL_DEBUG_OUTPUT);
		}
#endif
#endif
#endif
		logger << "Using Renderer: " << std::string((char*)glGetString(GL_RENDERER)) << Log::newline;


		if (localConfig["window"].isMember("vsync"))
		{
			logger << "SwapInterval: " << (localConfig["window"]["vsync"].asBool() ? 1 : 0) << Log::newline;
			wglSwapIntervalEXT(localConfig["window"]["vsync"].asBool() ? 1 : 0);
		}
		else
			wglSwapIntervalEXT(0);
	}


	void KernelWindows::swapBuffer()
	{
		SwapBuffers(hdc);
	}


	void KernelWindows::tick(double frameTime, double time)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // If we have a message to process, process it
			if (msg.message == WM_QUIT) {
				running = false; // Set running to false if we have a message to quit
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		Kernel::tick(frameTime, time);
	}

	void KernelWindows::windowMoveTo(int x, int y)
	{
		SetWindowPos(hWnd, HWND_TOP, x, y, windowWidth, windowHeight, SWP_NOSIZE);
	}
}

	#endif