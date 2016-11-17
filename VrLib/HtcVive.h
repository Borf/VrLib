#pragma once

#include <VrLib/Device.h>

namespace vrlib
{
	class Vive
	{
	public:
		class Controller
		{
		public:
			vrlib::PositionalDevice		transform;
			vrlib::DigitalDevice		applicationMenuButton;
			vrlib::DigitalDevice		gripButton;
			vrlib::DigitalDevice		touchButton;
			vrlib::DigitalDevice		triggerButton;

			vrlib::TwoDimensionDevice	thumbPad;
		} controllers[2];

		vrlib::PositionalDevice hmd;

		void init();
	};
}