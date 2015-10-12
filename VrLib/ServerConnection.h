#pragma once

#include <thread>
#include <windows.h>

namespace vrlib
{
	namespace json { class Value; }
	class ServerConnection
	{
		const std::string apiHost = "127.0.0.1";
		const int apiPort = 5000;
		SOCKET s;
	public:
		std::thread backgroundThread;
		bool running;

		ServerConnection();


		void thread();

		void update(double frameTime);
		bool isConnected();
		void send(const json::Value &value);


		void sendFps(float fps);

	};

}