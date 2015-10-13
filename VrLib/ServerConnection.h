#pragma once

#include <thread>
#include <map>
#include <windows.h>

#include <VrLib/json.h>

namespace vrlib
{
	namespace json { class Value; }

	class Tunnel
	{
	public:
	};

	class ServerConnection
	{
		const std::string apiHost = "127.0.0.1";
		const int apiPort = 5000;
		SOCKET s;
		std::map<std::string, std::function<void(const json::Value &)>> callbacks;
		std::map<std::string, std::function<void(const json::Value &)>> singleCallbacks;
	public:
		std::thread backgroundThread;
		bool running;

		ServerConnection();


		void thread();

		void update(double frameTime);
		bool isConnected();
		void send(const json::Value &value);


		void callBackOnce(const std::string &action, std::function<void(const json::Value &)> callback);
		json::Value call(const std::string &action, const json::Value& data = json::Value::null);
		Tunnel* createTunnel(const std::string &sessionId);

		void sendFps(float fps);

	};

}