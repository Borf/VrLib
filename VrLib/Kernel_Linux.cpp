#ifndef WIN32
#include "Kernel_Linux.h"


namespace vrlib
{
	Kernel* Kernel::getInstance()
	{
		static Kernel_Linux* t = new Kernel_Linux();
		return t;
	}




}




#endif