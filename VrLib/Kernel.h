#pragma once

#include <VrLib/VrLib.h>
#include <VrLib/json.hpp>


#include <glm/vec2.hpp>

#include <string>
#include <list>
#include <map>

#ifdef WIN32
#pragma comment(lib, "vrlib.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "leap.lib")
#pragma comment(lib, "libovr.lib")
#pragma comment(lib, "openvr_api.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "vrpn.lib")
#pragma comment(lib, "quat.lib")

//ffmpeg
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")


#ifdef _DEBUG
//#pragma comment(lib, "sixense.lib")	// debug version needs vs debug installed
//#pragma comment(lib, "sixense_utils.lib")	// debug version needs vs debug installed
#pragma comment(lib, "fgloved.lib")
#pragma comment(lib, "freetype255d.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")
#else
//#pragma comment(lib, "sixense.lib")
//#pragma comment(lib, "sixense_utils.lib")
#pragma comment(lib, "fglove.lib")
#pragma comment(lib, "freetype255.lib")
#pragma comment(lib, "assimp-vc140-mt-static.lib") //so that you don't get annoying dll trouble
#endif
#endif


namespace vrlib
{
	class Application;
	class Viewport;
	class User;
	class Device;
	class ClusterManager;
	class ClusterDataBase;
	class DeviceDriver;
	class DeviceDriverAdaptor;
	class MouseButtonDeviceDriver;
	class KeyboardDeviceDriver;
	class SimPositionDeviceDriver;
	class PositionalDevice;
	class RaceWheelDriver;
	class OculusDeviceDriver;
	class OpenVRDriver;
	class Oculus;
	class XBOXDeviceDriver;
	class ServerConnection;
	class Sim2dInputDeviceDriver;


	class KernelWindows;

	class Kernel // singleton, but special one
	{
	protected:
		bool running;
		int 	frameCount;

		nlohmann::json config;
		nlohmann::json localConfig;

		Application* currentApplication;
		Application* newApplication;


		PositionalDevice* headDevice;									//TODO: fix this properly

		std::list<Viewport*> viewports;
		std::list<User*> users;
		std::map<std::string, DeviceDriverAdaptor*> adaptors;			//are things that get information out of a driver (and have a name too), like a keyboardkey or a mousebuttonpress
		std::map<std::string, DeviceDriver*> drivers;					//devices are things like a mousedriver, keyboarddriver, vrpn driver
		std::list<Device*>	devices;
		std::list<ClusterDataBase*> clusterData;

		ClusterManager* clusterManager;



		MouseButtonDeviceDriver* mouseDriver;
		KeyboardDeviceDriver* keyboardDriver;
		SimPositionDeviceDriver* simPositionDriver;
		Sim2dInputDeviceDriver* sim2dInputDriver;

		OculusDeviceDriver* oculusDriver;
		OpenVRDriver* openvrDriver;
		XBOXDeviceDriver* xboxDriver;

		int windowWidth;
		int windowHeight;

		virtual void createWindow() = 0;
		virtual void swapBuffer() = 0;
		//virtual HWND GetHWND();


		void mergeConfig(nlohmann::json &config, const nlohmann::json &newConfig);
		DeviceDriver* getDeviceDriver(std::string name);
		void registerClusterData(ClusterDataBase*);
		void loadCluster();
		void loadDeviceDrivers();
		void setLocalConfig();
		void createViewports(User* user);
		void checkForNewApp();
		virtual void tick(double frameTime, double time);

		void syncClusterData();

		void syncDevices(double frameTime);

		void registerDevice(Device* device);
		DeviceDriverAdaptor* getDeviceDriverAdaptor(std::string name);
		User* getUser(std::string userName);


		//using some friends because we want only want to keep the public API public, the rest should be internal
		friend class Viewport;											//needs an user for initialization
		friend class SimulatorViewport;									//needs the userlist
		friend class ClusterDataBase;									//needs to be able to register itself
		friend class OculusDeviceDriver;								//needs access to the adaptors
		friend class ConfigPanel;										//needs access to the adaptors
		template <class R, class T> friend class DeviceImpl;			//devices need to be able to register itself
		friend class NormalApp;

		Kernel();
	public:
		ServerConnection* serverConnection;

		virtual ~Kernel();
		RaceWheelDriver* raceWheelDriver; // this one is public so that you can sent an effect to it
		static Kernel* getInstance();	//IS DEFINED IN KERNEL_WINDOWS.CPP
		inline int getWindowHeight()
		{
			return windowHeight;
		}
		inline int getWindowWidth()
		{
			return windowWidth;
		}
		inline glm::ivec2 getWindowSize()
		{
			return glm::ivec2(windowWidth, windowHeight);
		}

		void loadConfig(std::string fileName);
		void setApp(Application* application);

		void start();
		void stop();

		bool isMaster();

	};
}

