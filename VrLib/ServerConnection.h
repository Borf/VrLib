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

#include <VrLib/json.h>

namespace vrlib
{
	namespace json { class Value; }
	class ServerConnection;
	class Tunnel
	{
	private:
		std::list<json::Value> queue;
		ServerConnection* connection;
	public:
		std::string id;
		void send(const json::Value &data);
		json::Value recv();
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
#if 1
		const std::string apiHost = "127.0.0.1";
#else
		const std::string apiHost = "145.48.6.10";
#endif
		const int apiPort = 6666;
		SOCKET s;
		std::map<std::string, std::function<void(const json::Value &)>> callbacks;
		std::map<std::string, std::function<void(const json::Value &)>> singleCallbacks;

		std::function<void(Tunnel*)> tunnelCallback;
		std::map<std::string, Tunnel*> tunnels;

	public:
		bool running;
		std::thread backgroundThread;

		ServerConnection();


		void thread();

		void update(double frameTime);
		bool isConnected();
		void send(const json::Value &value, int sock = 0);
		void waitForConnection();

		void callBackOnce(const std::string &action, std::function<void(const json::Value &)> callback);
		json::Value call(const std::string &action, const json::Value& data = json::Value::null);
		Tunnel* createTunnel(const std::string &sessionId);
		void onTunnelCreate(const std::function<void(Tunnel*)> &onTunnel);

		void sendFps(float fps);

	};

}