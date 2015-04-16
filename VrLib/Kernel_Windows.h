#pragma once
#ifdef WIN32

#include <VrLib/Kernel.h>
#include <Windows.h>

namespace vrlib
{
	class KernelWindows : public Kernel
	{
	protected:
		HDC hdc;
		HWND hWnd;

		virtual void createWindow();
		virtual void tick(double frameTime, double time);
		virtual void swapBuffer();
		virtual void windowMoveTo(int x, int y);

		LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		KernelWindows();
	};
}

#endif