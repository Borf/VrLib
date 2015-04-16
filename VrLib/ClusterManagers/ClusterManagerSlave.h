#pragma once

#include <WinSock2.h>
#include <VrLib/ClusterManager.h>

namespace vrlib
{
	class ClusterSlave : public ClusterManager
	{
		SOCKET s;

	public:
		ClusterSlave();

		virtual bool isMaster();
		virtual bool sync(std::map<std::string, DeviceDriverAdaptor*> &adaptors);
		virtual bool sync(std::list<ClusterDataBase*> &clusterData);
	};
}