#include "HtcVive.h"

namespace vrlib
{
	void Vive::init()
	{
		hmd.init("MainUserHead");

		controllers[0].transform.init("WandPosition");
		controllers[1].transform.init("WandPositionLeft");

		controllers[0].applicationMenuButton.init("buttonRightMenu");
		controllers[0].gripButton.init("buttonRightGrip");
		controllers[0].touchButton.init("buttonRightTouch");
		controllers[0].triggerButton.init("buttonRightTrigger");

		controllers[1].applicationMenuButton.init("buttonLeftMenu");
		controllers[1].gripButton.init("buttonLeftGrip");
		controllers[1].touchButton.init("buttonLeftTouch");
		controllers[1].triggerButton.init("buttonLeftTrigger");

		controllers[1].thumbPad.init("LeftThumbPos");
		controllers[0].thumbPad.init("RightThumbPos");

		if (!controllers[0].applicationMenuButton.isInitialized())
		{//uhoh, we're not on the vive...
			controllers[0].triggerButton.init("LeftButton");
			controllers[0].touchButton.init("RightButton");

		}


	}
}