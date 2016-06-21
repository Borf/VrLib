#include <VrLib/ClusterManagers/ClusterManagerSlave.h>
#include <VrLib/Log.h>
#include <VrLib/PerfMon.h>
#include <VrLib/BinaryStream.h>
#include <VrLib/Device.h>
#include <VrLib/ClusterData.h>
#include <string.h>

#ifdef WIN32
#include <WS2tcpip.h> //socklen_t
#else
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
	bool ClusterSlave::isMaster()
	{
		return false;
	}

	ClusterSlave::ClusterSlave()
	{
		logger << "Configured as slave: listening and waiting for master" << Log::newline;

		SOCKET listenSocket;
		struct sockaddr_in sin;
		if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			logger << "Slave: Error creating socket!" << Log::newline;
			return;
		}

		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
#ifdef WIN32
		//TODO	sin.sin_addr = inet_addr(bindport.c_str());
#else
		inet_aton("0.0.0.0", &sin.sin_addr);
#endif
		sin.sin_port = htons(8888);

		int t_reuse = 1;
		setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&t_reuse, sizeof(t_reuse));
		if ((bind(listenSocket, (struct sockaddr*)&sin, sizeof(sin))))
		{
			logger << "Slave: Error binding socket" << Log::newline;
			closesocket(listenSocket);
			return;
		}

		if ((listen(listenSocket, 1)))
		{
			logger << "cSocketServer: Error listening to port" << Log::newline;
			closesocket(listenSocket);
			return;
		}

		logger << "Listening on for connections" << Log::newline;

		struct sockaddr_in client;
		socklen_t size = sizeof(client);
		s = accept(listenSocket, (struct sockaddr*)&client, &size);

		logger << "CONNECTED" << Log::newline;

		/*	#ifdef WIN32	//no more blocking, can just recv from it now :D
		unsigned long l = 1;
		ioctlsocket(s, FIONBIO, &l);
		#else
		fcntl(sock,F_SETFL,fcntl(s,F_GETFL) | O_NONBLOCK);
		#endif*/
	}


	bool ClusterSlave::sync(std::map<std::string, DeviceDriverAdaptor*> &adaptors)
	{

		int len;
		int rc = recv(s, (char*)&len, 4, 0);
		if (rc != 4)
			return false;

		char* data = new char[len];;
		int index = 0;
		while (index < len)
		{
			rc = recv(s, data + index, len - index, 0);
			if (rc < 1)
				return false;
			index += rc;
		}
		int rc2 = send(s, "@", 1, 0);
		if (rc2 != 1)
			return false;

		/*
		char buf[2048];
		int rc = recv(s, buf, 2048, 0);
		int rc2 = send(s, "@", 1, 0);
		if(rc2 != 1)
		return false;

		int len = ((int*)buf)[0];
		if(len+4 != rc)
		return false;*/

		BinaryStream buffer(data, len);
		for (std::map<std::string, DeviceDriverAdaptor*>::iterator it = adaptors.begin(); it != adaptors.end(); it++)
			it->second->updateDataSlave(buffer);

		delete[] data;

		return true;
	}

	bool ClusterSlave::sync(std::list<ClusterDataBase*> &clusterData)
	{
		int len;
		int rc = recv(s, (char*)&len, 4, 0);
		if (rc != 4)
			return false;
		int rc2 = send(s, "x", 1, 0);
		if (rc2 != 1)
			return false;

		char* data = new char[len];;
		int index = 0;
		while (index < len)
		{
			rc = recv(s, data + index, len - index, 0);
			if (rc < 1)
				return false;
			index += rc;
		}




		BinaryStream buffer(data, len);
		for (std::list<ClusterDataBase*>::iterator it = clusterData.begin(); it != clusterData.end(); it++)
			(*it)->updateDataSlave(buffer);

		return true;
	}
}