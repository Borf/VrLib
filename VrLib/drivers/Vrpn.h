#pragma once

#ifdef WIN32

#include <VrLib/Device.h>
#include <VrLib/json.hpp>
#include <glm/glm.hpp>
#include <map>

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


		std::map<int, glm::mat4> postTransforms;

		VrpnDeviceDriver(const nlohmann::json &json);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);

		virtual void update();

	};
}


#endif