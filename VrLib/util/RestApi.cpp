#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "RestApi.h"

#include <VrLib/Log.h>
#include <VrLib/json.hpp>
#include <algorithm>
#include <glm/glm.hpp>
#include <string.h>
#include <thread>
#include <chrono>

#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
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
	RestApi::RestApi()
	{
		char hostname[1024];
		gethostname(hostname, 1024);
		myHostname = hostname;
	
		logger << "Rest API: hostname: " << myHostname << Log::newline;
	}


	RestApi::~RestApi()
	{
	}

	void RestApi::registerAsEnvironment()
	{
		std::vector<std::string> headers;
		headers.push_back("Content-Type: application/json");
		json postData;
		postData["name"] = myHostname;
		postData["platform"] = "cave";
		postData["scenarios"].push_back("Presentatie1.json");
		postData["scenarios"].push_back("Presentatie2.json");
		postData["scenarios"].push_back("Presentatie3.json");
		json ret = callApi(POST, "environment", headers, postData);


		int id = ret["success"]["id"];

		logger << ret << Log::newline;

		int sessionId = 0;

		while (sessionId == 0)
		{
			json ret = callApi(PUT, "environment/" + std::to_string(id), headers, json());

			sessionId = ret["success"]["sessionid"];
			if (sessionId != 0)
				break;
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}

		json session = callApi(GET, "session:" + std::to_string(sessionId), headers, json());

	}



	std::vector<std::string> split(std::string str, const std::string &sep)
	{
		std::vector<std::string> ret;
		std::size_t index;
		while (true)
		{
			index = str.find(sep);
			if (index == std::string::npos)
				break;
			ret.push_back(str.substr(0, index));
			str = str.substr(index + sep.size());
		}
		ret.push_back(str);
		return ret;
	}

	json RestApi::callApi(Method method, const std::string &url, const std::vector<std::string> &headers, const json &postData /*= Json::nullValue*/)
	{
		std::string request;
		if (method == POST)
			request += "POST";
		if (method == GET)
			request += "GET";
		if (method == PUT)
			request += "PUT";

		logger << "REST " << request << " call to " << apiHost << "/" << url << Log::newline;


		struct sockaddr_in addr;
		struct hostent* host;

		host = gethostbyname(apiHost.c_str());
		if (host == NULL)
		{
			logger << "Could not look up host " << apiHost << "', are you connected to the internet?";
			return NULL;
		}
		addr.sin_family = host->h_addrtype;
		memcpy((char*)&addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
		addr.sin_port = htons(1337);
		memset(addr.sin_zero, 0, 8);
	
		SOCKET s;
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			logger << "Cannot create socket, try a reboot" << Log::newline;
			closesocket(s);
			return NULL;
		}

		int rc;
		int siz = sizeof(addr);
		rc = ::connect(s, (struct sockaddr*) &addr, siz);
		if (rc < 0)
		{
			logger << "Could not connect to REST api host: " << apiHost << Log::newline;
			closesocket(s);
			return NULL;
		}
	
		std::string postDataStr = postData.dump();

		request += " /" + url + " HTTP/1.1\r\n";

		request += "Host: " + apiHost + "\r\n";

		if (method == POST)
		{
			request += "Content-Length: " + Log::format("%i\r\n", postDataStr.size());
			request += "Content-Type: application/json\r\n";
		}

		for (auto h : headers)
			request += h + "\r\n";
		request += "\r\n";
		if (method == POST)
		{
			request += postDataStr;
		}

		size_t sent = 0;
		while (sent < request.size())
		{
			int rc = send(s, request.c_str() + sent, glm::min(request.size()-sent, 1024u), 0);
			if (rc <= 0)
			{
				logger << "Error sending to socket" << Log::newline;
				closesocket(s);
				return NULL;
			}
			sent += rc;
		}

		std::string buffer;
		bool headersFound = false;
		int len = -1;
		while (true)
		{
			char buf[1024];
			int rc = recv(s, buf, 1024, 0);
			if (rc <= 0)
			{
				closesocket(s);
				break;
			}
			buffer += std::string(buf, rc);
			if (!headersFound && buffer.find("\r\n\r\n") != std::string::npos)
			{
				headersFound = true;
				std::vector<std::string> headers = split(buffer.substr(0, buffer.find("\r\n\r\n")), "\r\n");
				for (auto h : headers)
				{
					std::string lower = h;
					std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
					if (lower.find(":"))
						lower = lower.substr(0, lower.find(":"));
					if (lower == "content-length")
						len = atoi(h.substr(lower.size() + 1).c_str());
				}
				buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
			}
			if (headersFound && (int)buffer.size() >= len && len != -1)
				break;
		}


		json ret(buffer);

		closesocket(s);




		return ret;
	}

	json RestApi::buildJson(const std::string &data)
	{
		return json(data);
	}
}