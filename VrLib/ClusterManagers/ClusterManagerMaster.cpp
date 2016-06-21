#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <VrLib/ClusterManagers/ClusterManagerMaster.h>
#include <VrLib/Log.h>
#include <VrLib/BinaryStream.h>
#include <VrLib/PerfMon.h>
#include <string.h>

#ifndef WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#define closesocket(x) ::close((x))
typedef int SOCKET;
#endif



namespace vrlib
{
	bool ClusterMaster::isMaster()
	{
		return true;
	}

	ClusterMaster::ClusterMaster(std::vector<std::pair<std::string, std::string> > n)
	{
		for (size_t i = 0; i < n.size(); i++)
			nodes.push_back(new ClusterNode(n[i].first));

		bool allConnected = false;
		while (!allConnected)
		{
			for (size_t i = 0; i < n.size(); i++)
			{
				if (nodes[i]->s != 0)
					continue;
				logger << "Connecting to slave " << nodes[i]->hostname << Log::newline;
				struct sockaddr_in addr;
				struct hostent* host;

				//host = gethostbyname(n[i].second.c_str());
				host = gethostbyname(nodes[i]->hostname.c_str());
				if (host == NULL)
				{
					logger << "Could not look up host " << n[i].second << "', are you connected to the internet?";
					continue;
				}
				addr.sin_family = host->h_addrtype;
				memcpy((char*)&addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
				addr.sin_port = htons(8888);
				memset(addr.sin_zero, 0, 8);

				if ((nodes[i]->s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				{
					logger << "Cannot create socket, try a reboot" << Log::newline;
					closesocket(nodes[i]->s);
					nodes[i]->s = 0;
					return;
				}

				int rc;
				int siz = sizeof(addr);
				rc = ::connect(nodes[i]->s, (struct sockaddr*) &addr, siz);
				if (rc < 0)
				{
					logger << "Could not connect to slave " << nodes[i]->hostname << Log::newline;
					closesocket(nodes[i]->s);
					nodes[i]->s = 0;
					continue;
				}
				logger << "Connected to " << nodes[i]->hostname << Log::newline;
			}

			allConnected = true;
			for (size_t i = 0; i < nodes.size(); i++)
				if (nodes[i]->s == 0)
					allConnected = false;
		}
	}


	bool ClusterMaster::sync(std::string data)
	{
		int len = data.size();

		char* buf = new char[data.size() + 4];
		memcpy(buf, (char*)&len, 4);
		memcpy(buf + 4, data.c_str(), len);

		for (size_t i = 0; i < nodes.size(); i++)
		{
			int sent = 0;
			while (sent < len + 4)
			{
				int rc = send(nodes[i]->s, buf + sent, len + 4 - sent, 0);
				if (rc < 0)
					return false;
				sent += rc;
			}
		}

		char ack[1];
		for (size_t i = 0; i < nodes.size(); i++)
		{
			double startTime = PerfMon::getInstance()->getTime();
			int rc = recv(nodes[i]->s, ack, 1, 0);
			if (rc != 1)
				return false;
			double endTime = PerfMon::getInstance()->getTime();
			if (endTime - startTime > 10)
				logger << "Waiting for node " << nodes[i]->hostname << ", " << endTime - startTime << "ms" << Log::newline;
		}

		delete[] buf;

		return true;
	}

}