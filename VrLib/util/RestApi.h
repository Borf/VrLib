#pragma once

#include <VrLib/Singleton.h>
#include <VrLib/json.hpp>
#include <string>
#include <vector>


namespace vrlib
{
	class RestApi : public Singleton<RestApi>
	{
		static const std::string apiHost;
		static std::string myHostname;
		static std::string getHostName();
	public:
		enum class Method { POST, GET, PUT };

		static json getSessionInfo(const std::string &sessionId, const std::string &key);


		static json callApi(Method method, const std::string &url, const std::vector<std::string> &headers, const json &postData = nullptr);
		static json buildJson(const std::string &data);
	private:

	};

}