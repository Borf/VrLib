#pragma once

#include <VrLib/Kernel.h>

namespace vrlib
{

	class Kernel_Linux : public Kernel
	{
	public:
		virtual void createWindow() override {}
		virtual void swapBuffer() override {}
	
	};


}