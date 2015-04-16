#pragma once

#include <VrLib/Device.h>

#include <glm/glm.hpp>


class vrpn_Tracker_Remote;



namespace vrlib
{
	class VrpnDeviceDriver : public DeviceDriver
	{
	public:
	private:
		class VrpnDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
			VrpnDeviceDriver* driver;
			int id;
		public:
			VrpnDeviceDriverAdaptor(VrpnDeviceDriver*, int id);
			virtual glm::mat4 getData();
		};

		vrpn_Tracker_Remote* tracker;


	public:
		glm::mat4 data[10];


		VrpnDeviceDriver();
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);

		virtual void update();

	};
}