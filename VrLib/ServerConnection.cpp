#include "ServerConnection.h"

#include <VrLib/Log.h>
#include <VrLib/json.hpp>
#include <GL/glew.h>
#include <ctime>
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
#include "Log.h"

namespace vrlib
{
	const unsigned char* renderer;
	ServerConnection::ServerConnection(json &config) : running(false), backgroundThread(&ServerConnection::thread, this)
	{
		this->config = config;
		if (this->config.is_null())
		{
			this->config["ip"] = apiHost;
			this->config["port"] = apiPort;
		}

		s = 0;
		tunnelCallback = nullptr;

		callbacks["session/start"] = [](const json & data) {
			logger << "session/start" << Log::newline;
			std::string dataString = data.dump();
			logger << dataString << Log::newline;
		};
		callbacks["tunnel/connect"] = [this](const json & data)
		{
			Tunnel* t = new Tunnel(data["data"]["id"], this);
			tunnels[t->id] = t;
			if (tunnelCallback)
				tunnelCallback(t);
		};
		callbacks["tunnel/send"] = [this](const json & data)
		{
			if (tunnels.find(data["data"]["id"]) == tunnels.end())
			{
				logger << "Got data for a tunnel that doesn't exist" << Log::newline;
				return;
			}
			tunnels[data["data"]["id"]]->mtx.lock();
			tunnels[data["data"]["id"]]->queue.push_back(data["data"]["data"]);
			tunnels[data["data"]["id"]]->mtx.unlock();
		};
		renderer = glGetString(GL_RENDERER);
		running = true;
	}


	void ServerConnection::thread()
	{
		while (!running)
		{
		#ifdef WIN32
			Sleep(1);
		#else
			usleep(1000);
		#endif
		}
		char hostname[1024] = "";
		char applicationName[1024] = "";
		char username[1024] = "";

		#ifdef WIN32
		gethostname(hostname, 1024);
		::GetModuleFileName(0, applicationName, 1024);
		DWORD username_len = 1024;
		GetUserName(username, &username_len);
		#endif

		std::time_t startTime = std::time(nullptr);

		bool lastConnected = true;
		while (running)
		{
			struct sockaddr_in addr;
			struct hostent* host;

			std::string ipAddr = config["ip"];

			host = gethostbyname(ipAddr.c_str());
			if (host == NULL)
			{
				logger << "Could not look up host " << config["ip"] << "', are you connected to the internet?";
				return;
			}
			addr.sin_family = host->h_addrtype;
			memcpy((char*)&addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
			addr.sin_port = htons(config["port"]);
			memset(addr.sin_zero, 0, 8);

			SOCKET tempSocket = 0;

			if ((tempSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			{
				logger << "Cannot create socket, try a reboot" << Log::newline;
				closesocket(s);
				#ifdef WIN32
				Sleep(60000);
				#else
				sleep(60);
				#endif
				continue;
			}

			int rc;
			int siz = sizeof(addr);
			rc = ::connect(tempSocket, (struct sockaddr*) &addr, siz);
			if (rc < 0)
			{
				if(lastConnected)
					logger << "Could not connect to api host: " << config["ip"] << Log::newline;
				lastConnected = false;
				closesocket(tempSocket);
				#ifdef WIN32
				Sleep(1000);
				#else
				sleep(1);
				#endif
				continue;
			}
			lastConnected = true;
			logger << "Connected to remote API" << Log::newline;

			json packet;
			packet["id"] = "session/start";
			packet["data"]["host"] = hostname;
			packet["data"]["file"] = applicationName;
			packet["data"]["renderer"] = std::string((char*)renderer);
			packet["data"]["starttime"] = (int)startTime;
			packet["data"]["user"] = username;
			send(packet, tempSocket);

			s = tempSocket;

			std::string buffer;
			char buf[1024];
			while (running && s != 0)
			{
				int rc = recv(s, buf, 1024, 0);
				if (rc < 0)
				{
					closesocket(s);
					s = 0;
					break;
				}
				buffer += std::string(buf, rc);
				while (buffer.size() > 4)
				{
					unsigned int len = *((unsigned int*)&buffer[0]);
					if (buffer.size() >= len + 4)
					{
						json data = json::parse(buffer.substr(4, len));
						buffer = buffer.substr(4 + len);

						if (data.find("id") == data.end())
						{
							logger << "Invalid packet from server" << Log::newline;
							logger << data << Log::newline;
							closesocket(s);
							s = 0;
							break;
						}

						if (callbacks.find(data["id"]) != callbacks.end()) {
							callbacks[data["id"]](data);
						}
						else if (singleCallbacks.find(data["id"]) != singleCallbacks.end()) {
							singleCallbacks[data["id"]](data);
							singleCallbacks.erase(singleCallbacks.find(data["id"]));
						}
						else
						{
							logger << "Invalid packet from server" << Log::newline;
							logger << data.dump() << Log::newline;
							//closesocket(s);
							//s = 0;
							//break;
						}
					}
					else
						break;
				}
			}
			lastConnected = false;
			logger << "Disconnected...." << Log::newline;
#ifdef WIN32
			Sleep(1000);
#else
			sleep(1);
#endif
		}
	}

	void ServerConnection::send(const json &value, int sock)
	{
		std::string data = value.dump();
		unsigned int len = data.size();
		int rc = ::send(sock == 0 ? s : sock, (char*)&len, 4, 0);
		if (rc < 0)
		{
			closesocket(s);
			s = 0;
			return;
		}
		::send(sock == 0 ? s : sock, data.c_str(), data.size(), 0);
		if (rc < 0)
		{
			closesocket(s);
			s = 0;
			return;
		}
	}

	void ServerConnection::waitForConnection()
	{
		while (s == 0)
#ifdef WIN32
			Sleep(10);
#else
			sleep(0);
#endif
	}


	void ServerConnection::callBackOnce(const std::string &action, std::function<void(const json &)> callback)
	{
		singleCallbacks[action] = callback;
	}

	json ServerConnection::call(const std::string &action, const json& data)
	{
		json result;
		if (s == 0)
			return result;
		bool done = false;
		callBackOnce(action, [&done, &result](const json &data)
		{
			result = data["data"];
			done = true;
		});
		json packet;
		packet["id"] = action;
		packet["data"] = data;
		send(packet);
		int i = 0;
		while (!done && i < 1000)
		{
#ifdef WIN32
			Sleep(2);
#else
			sleep(0);
#endif
			i++;
		}
		return result;
	}

	void ServerConnection::update(double frameTime)
	{
	}


	bool ServerConnection::isConnected()
	{
		return s != 0;
	}

	void ServerConnection::sendFps(float fps)
	{
		json v;
		v["id"] = "session/report";
		v["data"]["fps"] = fps;
		send(v);
	}



	Tunnel* ServerConnection::createTunnel(const std::string &sessionId)
	{
		waitForConnection();
		json data;
		data["session"] = sessionId;
		json result = call("tunnel/create", data);

		if (result["status"] == "ok")
		{
			Tunnel* t = new Tunnel(result["id"], this);
			tunnels[t->id] = t;
			return t;
		}

		logger<<"Could not create tunnel!\n"<< result << Log::newline;
		return NULL;
	}

	void ServerConnection::onTunnelCreate(const std::function<void(Tunnel*)> &onTunnel, const std::string &key)
	{
		waitForConnection();
		json v;
		v["id"] = "session/enable";
		v["data"].push_back("tunnel");
		if(key != "")
			v["key"] = key;
		send(v);
		tunnelCallback = onTunnel;
	}

	void Tunnel::send(const json &data)
	{
		if (!this)
			return;
		json packet;
		packet["id"] = "tunnel/send";
		packet["data"]["dest"] = id;
		packet["data"]["data"] = data;
		connection->send(packet);
	}

	json Tunnel::recv()
	{
		mtx.lock();
		json res = queue.front();
		queue.pop_front();
		mtx.unlock();
		return res;
	}

	int Tunnel::available()
	{
		mtx.lock();
		int size = queue.size();
		mtx.unlock();
		return size;
	}

}