#pragma once

#include <WinSock2.h>
#include <vector>
#include <VrLib/ClusterManager.h>


namespace vrlib
{
	class ClusterMaster : public ClusterManager
	{
		class ClusterNode
		{
		public:
			SOCKET s;
			std::string hostname;

			ClusterNode(std::string hostname) { this->hostname = hostname; s = 0; };

		};

		std::vector<ClusterNode*> nodes;

	public:
		ClusterMaster(std::vector<std::pair<std::string, std::string> > nodes);

		virtual bool isMaster();
		virtual bool sync(std::string data);
	};
}