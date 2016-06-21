#pragma once
#ifdef WIN32
#include <VrLib/Device.h>

#include <glm/glm.hpp>

#include <windows.h>
#include <fglove.h>



namespace vrlib
{
	class GloveDeviceDriver : public DeviceDriver
	{
	public:
	private:
		enum GloveFinger
		{
			LeftLittle, LeftRing, LeftMiddle, LeftIndex, LeftThumb,
			RightLittle, RightRing, RightMiddle, RightIndex, RightThumb,
		};
		class GloveDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
			GloveDeviceDriver* driver;
			GloveFinger finger;
		public:
			GloveDeviceDriverAdaptor(GloveDeviceDriver*, GloveFinger id);
			virtual glm::mat4 getData();
		};
		virtual glm::mat4 calcFinger(const glm::mat4 &mat, float rot, float T1x, float T1y, float T2y, bool thumb);


	public:
		fdGlove *pGloveLeft;
		fdGlove *pGloveRight;
		fdGlove *pGlove;
		int usbGloveLeft;
		int usbGloveRight;


		float data[10];
		glm::mat4 fingerpos[10];


		GloveDeviceDriver();
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);

		virtual void update();

	};
}

#endif