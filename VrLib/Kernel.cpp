#include "Kernel.h"

#include <fstream>
#include <cctype>

#include <VrLib/Application.h>
#include <VrLib/Log.h>
#include <VrLib/Viewport.h>
#include <VrLib/User.h>
#include <VrLib/ClusterManager.h>
#include <VrLib/ClusterManagers/ClusterManagerMaster.h>
#include <VrLib/ClusterManagers/ClusterManagerSlave.h>
#include <VrLib/ClusterData.h>
#include <VrLib/Device.h>
#include <VrLib/drivers/MouseButton.h>
#include <VrLib/drivers/Keyboard.h>
#include <VrLib/drivers/SimPosition.h>
#include <VrLib/drivers/Sim2dInput.h>

#ifdef WIN32
#include <VrLib/drivers/XBOXController.h>
#include <VrLib/drivers/GloveDriver.h>
#include <VrLib/drivers/RaceWheelDriver.h>
#include <VrLib/drivers/Oculus.h>
#include <VrLib/drivers/HydraDriver.h>
#include <VrLib/drivers/LeapMotion.h>
#include <VrLib/drivers/Vrpn.h>
#include <VrLib/drivers/OpenVR.h>
#endif
#include <VrLib/PerfMon.h>
#include <VrLib/ServerConnection.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifndef WIN32
#include <unistd.h>
#endif


namespace vrlib
{
	Log logger;
	double PCFreq = 0.0;
	__int64 CounterStart = 0;


	Kernel::Kernel(void)
	{
		currentApplication = nullptr;
		newApplication = nullptr;

		mouseDriver = nullptr;
		keyboardDriver = nullptr;
		simPositionDriver = nullptr;
		sim2dInputDriver = nullptr;
		raceWheelDriver = nullptr;
		oculusDriver = nullptr;
		openvrDriver = nullptr;
		serverConnection = nullptr;
	}


	Kernel::~Kernel(void)
	{
	}

	void Kernel::loadConfig(std::string fileName)
	{
		std::ifstream file;
		file.open(fileName);
		if (!file.is_open())
		{
			logger << "Could not open file " << fileName << Log::newline;
			return;
		}
		json::Value newValue = json::readJson(file);
		mergeConfig(config, newValue);
	}

	void Kernel::setApp(Application* application)
	{
		newApplication = application;
	}


	void Kernel::start()
	{
		if (config.isNull() || config.size() == 0)
		{
			logger << "No configuration file loaded" << Log::newline;
			logger << "Press enter key to exit" << Log::newline;
			getchar();
			return;
		}
		std::ofstream(".config.combined.json") << config;

		setLocalConfig();
		loadDeviceDrivers();
		loadCluster();
		createWindow();
		serverConnection = new ServerConnection();

		User* user = new User("MainUser");
		users.push_back(user);

		createViewports(user);


		frameCount = 0;

		double time = 0;
		double frameTime;
		double lastFps = 0;
		if (config.isMember("users"))
		{
			headDevice = new PositionalDevice();
			headDevice->init(config["users"][0u]["src"].asString());
		}
		else
			headDevice = NULL;


		running = true;
		tick(0, 0);
		PerfMon::getInstance()->resetTimer();
		while (running)
		{
			frameTime = PerfMon::getInstance()->getTime();
			PerfMon::getInstance()->resetTimer();

			time += frameTime;

			if (lastFps + 10000 < time)
			{
				lastFps = time;
				serverConnection->sendFps(frameCount / 10.0f);
				logger << "FPS: " << frameCount / 10.0f << Log::newline;
				frameCount = 0;
			}

			tick(frameTime, time);
#ifdef WIN32
			Sleep(0);
#else
			sleep(0);
#endif
		}
	}

	void Kernel::stop()
	{
		running = false;
	}


	void Kernel::mergeConfig(json::Value &config, const json::Value &newConfig)
	{
		for (json::Value::Iterator it = newConfig.begin(); it != newConfig.end(); it++)
			if (config.isMember(it.key()))
				if (config[it.key()].isObject())
					mergeConfig(config[it.key()], it.value());
				else if (config[it.key()].isArray())
				{
					for (size_t ii = 0; ii < newConfig[it.key()].size(); ii++)
						config[it.key()].push_back(it.value()[ii]);
				}
				else
					config[it.key()] = it.value();
			else
				config[it.key()] = it.value();
	}






	DeviceDriver* Kernel::getDeviceDriver(std::string name)
	{
		if (name == "SimPosition")
		{
			SimPositionDeviceDriver* driver = new SimPositionDeviceDriver(config["driverconfig"]["SimPosition"]);
			if (simPositionDriver)
				logger << "Double simpositiondriver" << Log::newline;
			simPositionDriver = driver;
			return driver;
		}
		else if (name == "Sim2d")
		{
			Sim2dInputDeviceDriver* driver = new Sim2dInputDeviceDriver(config["driverconfig"]["Sim2d"]);
			if (sim2dInputDriver)
				logger << "Double sim2dDriver" << Log::newline;
			sim2dInputDriver = driver;
			return driver;
		}
		else if (name == "mouse")
		{
			MouseButtonDeviceDriver* driver = new MouseButtonDeviceDriver();
			if (mouseDriver)
				logger << "Double Mousedriver" << Log::newline;
			mouseDriver = driver;
			return driver;
		}
		else if (name == "keyboard")
		{
			KeyboardDeviceDriver* driver = new KeyboardDeviceDriver();
			if (keyboardDriver)
				logger << "Double Keyboarddriver" << Log::newline;
			keyboardDriver = driver;
			return driver;
		}
#ifdef WIN32
		else if (name == "vrpn")
			return new VrpnDeviceDriver();
		else if (name == "XBOX")
		{
			XBOXDeviceDriver* driver = new XBOXDeviceDriver();
			if (xboxDriver)
				logger << "Double XBOX controller driver" << Log::newline;
			xboxDriver = driver;
			return driver;
		}
		else if (name == "glove")
			return new GloveDeviceDriver();
		else if (name == "hydra")
			return new HydraDeviceDriver();
		else if (name == "leapmotion")
			return new LeapMotionDeviceDriver();
		else if (name == "wheel")
		{

			HWND _window = GetActiveWindow();
			HWND _activeWindow;

			while (_window != NULL)
			{
				_activeWindow = _window;
				_window = GetParent(_window);
			}

			RaceWheelDriver* driver = new RaceWheelDriver(_activeWindow);

			/*if (RaceWheelDriver)
				logger << "Double Keyboarddriver" << Log::newline;*/
			raceWheelDriver = driver;
			//raceWheelDriver = new cRaceWheelDriver(_activeWindow);
			return driver;
		}
		else if (name == "oculus")
		{
			if (!oculusDriver)
				oculusDriver = new OculusDeviceDriver(config["driverconfig"]["Oculus"]);
			return oculusDriver;
		}
		else if (name == "openvr")
		{
			if(!openvrDriver)
				openvrDriver = new OpenVRDriver(config["driverconfig"]["openvr"]);
			return openvrDriver;
		}
#endif
		else
		{
			logger << "Unknown driver requested: '" << name << "'" << Log::newline;
			return NULL;
		}
	}

	DeviceDriverAdaptor* Kernel::getDeviceDriverAdaptor(std::string name)
	{
		if (adaptors.find(name) == adaptors.end())
		{
			logger << "Unable to find device " << name << Log::newline;
			return NULL;
		}
		return adaptors[name];
	}

	void Kernel::registerDevice(Device* device)
	{
		for (std::list<Device*>::iterator it = devices.begin(); it != devices.end(); it++)
			if (*it == device)
				return;
		devices.push_back(device);
	}

	bool Kernel::isMaster()
	{
		if (!clusterManager)
			return true;
		return clusterManager->isMaster();
	}

	void Kernel::registerClusterData(ClusterDataBase* data)
	{
		clusterData.push_back(data);
	}

	void Kernel::setLocalConfig()
	{
		char hostname[1024];
		gethostname(hostname, 1024);
		localConfig = json::Value::null;
		if(config.isMember("computers"))
			for (size_t i = 0; i < config["computers"].size(); i++)
			{
				if (config["computers"][i]["host"].asString() == hostname)
				{
					localConfig = config["computers"][i];
					logger << "Found config: " << hostname << Log::newline;
				}
			}
		if (localConfig.isNull() && config.isMember("local"))
			localConfig = config["local"];

		if (localConfig.isNull())
		{
			logger << "Couldn't find config for this host!: Hostname '" << hostname << "'" << Log::newline;
			for (size_t i = 0; i < config["computers"].size(); i++)
				logger << "Found: " << config["computers"][i]["host"].asString() << Log::newline;
		}
	}

	void Kernel::loadDeviceDrivers()
	{
		if (!config.isMember("devices"))
			return;
		for (size_t i = 0; i < config["devices"].size(); i++)
		{
			if (drivers.find(config["devices"][i]["driver"].asString()) == drivers.end())
				drivers[config["devices"][i]["driver"].asString()] = getDeviceDriver(config["devices"][i]["driver"].asString());

			DeviceDriver* driver = drivers[config["devices"][i]["driver"].asString()];
			if (!driver)
				continue;
			adaptors[config["devices"][i]["name"].asString()] = driver->getAdaptor(config["devices"][i].get("src", "").asString());
		}
	}

	void Kernel::loadCluster()
	{
		if (localConfig.isMember("mode"))
		{
			if (localConfig["mode"].asString() == "slave")
			{
				clusterManager = new ClusterSlave();
			}
			else if (localConfig["mode"].asString() == "master")
			{
				std::vector<std::pair<std::string, std::string> > clusterNodes;
				for (size_t i = 0; i < config["computers"].size(); i++)
					if (config["computers"][i]["mode"].asString() == "slave")
						clusterNodes.push_back(std::pair<std::string, std::string>(config["computers"][i]["host"].asString(), config["computers"][i]["ip"].asString()));
				clusterManager = new ClusterMaster(clusterNodes);
			}
			else
				logger << "Unknown configuration mode: " << localConfig["mode"].asString() << Log::newline;
		}
		else
			clusterManager = NULL;
	}


	void Kernel::createViewports(User* user)
	{
		if (!localConfig.isMember("viewports"))
			return;
		for (size_t i = 0; i < localConfig["viewports"].size(); i++)
		{
			Viewport* viewport = Viewport::createViewport(this, localConfig["viewports"][i], config["computers"]);
			if (viewport)
				viewports.push_back(viewport);
		}
	}

	void Kernel::tick(double frameTime, double time)
	{
		checkForNewApp();
		serverConnection->update(frameTime);
		double startTime = PerfMon::getInstance()->getTime();


		syncDevices(frameTime);

		if (PerfMon::getInstance()->getTime() - startTime > 14)
			logger << "Cluster sync 1: " << PerfMon::getInstance()->getTime() - startTime << Log::newline;

		if(headDevice)
			(*users.begin())->matrix = headDevice->getData();
		currentApplication->preFrame(frameTime, time);


		startTime = PerfMon::getInstance()->getTime();
		syncClusterData();

		if (PerfMon::getInstance()->getTime() - startTime > 10)
			logger << "Cluster sync 2: " << PerfMon::getInstance()->getTime() - startTime << Log::newline;


		currentApplication->latePreFrame();

		glViewport(0, 0, windowWidth, windowHeight); // Set the viewport size to fill the window
		glClearColor(currentApplication->clearColor[0], currentApplication->clearColor[1], currentApplication->clearColor[2], currentApplication->clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
		for (std::list<Viewport*>::iterator it = viewports.begin(); it != viewports.end(); it++)
		{
			Viewport* vp = *it;
			glViewport((int)(windowWidth*vp->x()), (int)(windowHeight*vp->y()), (int)(windowWidth*vp->width()), (int)(windowHeight*vp->height())); // Set the viewport size to fill the window
			glClear(GL_DEPTH_BUFFER_BIT); // Clear required buffers

			vp->draw(currentApplication);
		}
		swapBuffer();
		frameCount++;
	}

	void Kernel::checkForNewApp()
	{
		if (newApplication)
		{
			if (currentApplication)
			{
				currentApplication->stop();
				delete currentApplication;
			}
			currentApplication = newApplication;
			newApplication = NULL;
			currentApplication->init();
		}
	}

	User* Kernel::getUser(std::string userName)
	{
		for (std::list<User*>::iterator it = users.begin(); it != users.end(); it++)
			if ((*it)->getName() == userName)
				return *it;
		return NULL;
	}

	void Kernel::syncDevices(double frameTime)
	{
		if (simPositionDriver && keyboardDriver)
			simPositionDriver->update(keyboardDriver, frameTime);
		if (sim2dInputDriver && keyboardDriver)
			sim2dInputDriver->update(keyboardDriver, frameTime);

#ifdef WIN32
		if (oculusDriver && keyboardDriver)
			oculusDriver->update(keyboardDriver);
#endif

		if (isMaster())
		{
			for (std::map<std::string, DeviceDriver*>::iterator it = drivers.begin(); it != drivers.end(); it++)
				it->second->update();

			BinaryStream data(2048);
			for (std::map<std::string, DeviceDriverAdaptor*>::iterator it = adaptors.begin(); it != adaptors.end(); it++)
				if(it->second)
					it->second->updateDataMaster(data);

			if (clusterManager)
				if (!clusterManager->sync(data.str()))
					stop();
		}
		else
		{
			if (!clusterManager->sync(adaptors))
				stop();
		}
	}

	void Kernel::syncClusterData()
	{
		if (isMaster())
		{
			int size = 0;
			for (std::list<ClusterDataBase*>::iterator it = clusterData.begin(); it != clusterData.end(); it++)
				size += (*it)->getEstimatedSize();
			BinaryStream data(size);
			for (std::list<ClusterDataBase*>::iterator it = clusterData.begin(); it != clusterData.end(); it++)
				(*it)->updateDataMaster(data);

			if (clusterManager)
				if (!clusterManager->sync(data.str()))
					stop();
		}
		else
		{
			if (!clusterManager->sync(clusterData))
				stop();
		}
	}

}
