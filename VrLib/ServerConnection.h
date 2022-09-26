#pragma once

#include <thread>
#include <map>
#include <list>
#include <mutex>

#ifdef WIN32
#include <windows.h>
#else
typedef int SOCKET;

#endif

#include <VrLib/json.hpp>

namespace vrlib
{
	class ServerConnection;
	class Tunnel
	{
	private:
		std::list<nlohmann::json> queue;
		ServerConnection* connection;
	public:
		std::string id;
		void send(const nlohmann::json &data);
		nlohmann::json recv();
		int available();

		std::mutex mtx;

		Tunnel(const std::string &id, ServerConnection* connection)
		{
			this->id = id;
			this->connection = connection;
		}
		friend class ServerConnection;
	};

	class ServerConnection
	{
	private:
		const int apiPort = 6666;
		const std::string apiHost = "145.48.6.10";
		SOCKET s;
		nlohmann::json config;

		std::map<std::string, std::function<void(const nlohmann::json &)>> callbacks;
		std::map<std::string, std::function<void(const nlohmann::json &)>> singleCallbacks;

		std::function<void(Tunnel*)> tunnelCallback;
		std::map<std::string, Tunnel*> tunnels;
	public:
		bool running;
		std::thread backgroundThread;

		ServerConnection(nlohmann::json &config);


		void thread();

		void update(double frameTime);
		bool isConnected();
		void send(const nlohmann::json &value, int sock = 0);
		void waitForConnection();

		void callBackOnce(const std::string &action, std::function<void(const nlohmann::json &)> callback);
		nlohmann::json call(const std::string &action, const nlohmann::json& data = nullptr);
		Tunnel* createTunnel(const std::string &sessionId);
		void onTunnelCreate(const std::function<void(Tunnel*)> &onTunnel, const std::string &key = "");
		inline void onTunnelCreate(const std::string &key, const std::function<void(Tunnel*)> &onTunnel) {
			onTunnelCreate(onTunnel, key);
		};

		void sendFps(float fps);
	};
}