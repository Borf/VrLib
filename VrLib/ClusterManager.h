#pragma once

#include <string>
#include <list>
#include <map>

namespace vrlib
{

	class Device;
	class DeviceDriverAdaptor;
	class ClusterDataBase;

	class ClusterManager
	{
	public:

		ClusterManager();
		~ClusterManager();


		virtual bool sync(std::string) { return false; };
		virtual bool sync(std::map<std::string, DeviceDriverAdaptor*> &adaptors) { return false; };
		virtual bool sync(std::list<ClusterDataBase*> &clusterData) { return false; };

		virtual bool inCluster() { return true; };
		virtual bool isMaster() = 0;
	};

}
