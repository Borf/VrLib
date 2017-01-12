#ifdef WIN32

#include <VrLib/drivers/Vrpn.h>
#include <VrLib/Log.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vrpn/vrpn_Tracker.h>

#include <VrLib/json.h>



namespace vrlib
{
	DeviceDriverAdaptor* VrpnDeviceDriver::getAdaptor(std::string options)
	{
		return new VrpnDeviceDriverAdaptor(this, atoi(options.c_str()));
	}

	void VRPN_CALLBACK handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t)
	{
		VrpnDeviceDriver* driver = (VrpnDeviceDriver*)userdata;

		glm::mat4 customRot = glm::rotate(glm::mat4(), glm::radians(180.0f), glm::vec3(0, 1, 0));

		glm::mat4 rot;
		
		rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(0, 1, 0));

		rot = rot * glm::toMat4(glm::quat(
			(float)t.quat[0],
			(float)t.quat[1],
			(float)t.quat[2],
			(float)t.quat[3]));
		rot = glm::rotate(rot, glm::radians(-90.0f), glm::vec3(0, 1, 0));

		auto it = driver->postTransforms.find(t.sensor);
		if (it != driver->postTransforms.end())
			rot *= it->second;
		
		//rot = glm::scale(glm::mat4(), glm::vec3(1, -1, 1)) * rot;



		rot = glm::inverse(rot);

		driver->data[t.sensor] = glm::translate(glm::mat4(), glm::vec3((float)t.pos[0], (float)t.pos[1], (float)t.pos[2])) * rot;
	}

	VrpnDeviceDriver::VrpnDeviceDriver(const json::Value &config)
	{
		tracker = new vrpn_Tracker_Remote("test_tracker@localhost");
		tracker->register_change_handler(this, handle_tracker_pos_quat);

		if (!config.isNull())
		{
			for (const json::Value &t : config)
			{
				if (t.isMember("transform"))
				{
					glm::mat4 transform;
					if (t["transform"].isMember("rot"))
					{
						transform = glm::rotate(transform, glm::radians(t["transform"]["rot"][0].asFloat()), glm::vec3(1, 0, 0));
						transform = glm::rotate(transform, glm::radians(t["transform"]["rot"][1].asFloat()), glm::vec3(0, 1, 0));
						transform = glm::rotate(transform, glm::radians(t["transform"]["rot"][2].asFloat()), glm::vec3(0, 0, 1));
					}
					postTransforms[t["id"].asInt()] = transform;
				}
			}
		}

	}

	void VrpnDeviceDriver::update()
	{
		tracker->mainloop();
	}




	VrpnDeviceDriver::VrpnDeviceDriverAdaptor::VrpnDeviceDriverAdaptor(VrpnDeviceDriver* driver, int id)
	{
		this->driver = driver;
		this->id = id;
	}

	glm::mat4 VrpnDeviceDriver::VrpnDeviceDriverAdaptor::getData()
	{
		return driver->data[id];
	}
}


#endif