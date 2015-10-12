#include "ServerConnection.h"

#include <VrLib/Log.h>
#include <VrLib/json.h>

namespace vrlib
{
	ServerConnection::ServerConnection() : running(true), backgroundThread(&ServerConnection::thread, this)
	{
		s = 0;
	}


	void ServerConnection::thread()
	{
		while (running)
		{
			struct sockaddr_in addr;
			struct hostent* host;

			host = gethostbyname(apiHost.c_str());
			if (host == NULL)
			{
				logger << "Could not look up host " << apiHost << "', are you connected to the internet?";
				return;
			}
			addr.sin_family = host->h_addrtype;
			memcpy((char*)&addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
			addr.sin_port = htons(apiPort);
			memset(addr.sin_zero, 0, 8);

			if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			{
				logger << "Cannot create socket, try a reboot" << Log::newline;
				closesocket(s);
				Sleep(60000);
				continue;
			}

			int rc;
			int siz = sizeof(addr);
			rc = ::connect(s, (struct sockaddr*) &addr, siz);
			if (rc < 0)
			{
				logger << "Could not connect to api host: " << apiHost << Log::newline;
				closesocket(s);
				Sleep(1000);
				continue;
			}

			logger << "Connected to remote API" << Log::newline;

			while (running && s != 0)
			{
				Sleep(1000);



			}
		}
	}

	void ServerConnection::send(const json::Value &value)
	{
		std::string data;
		data << value;
		int len = data.size();
		int rc = ::send(s, (char*)&len, 4, 0);
		if (rc < 0)
		{
			closesocket(s);
			s = 0;
			return;
		}
		::send(s, data.c_str(), data.size(), 0);
		if (rc < 0)
		{
			closesocket(s);
			s = 0;
			return;
		}
	}


	void ServerConnection::update(double frameTime)
	{
	}


	void ServerConnection::sendFps(float fps)
	{
		json::Value v;
		v["action"] = "fps";
		v["data"] = fps;
		send(v);
	}

}