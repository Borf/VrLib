#pragma once

#include <VrLib/Singleton.h>
#include <VrLib/json.h>
#include <string>
#include <vector>


namespace vrlib
{
	class RestApi : public Singleton<RestApi>
	{
		const std::string apiHost = "127.0.0.1:1337";
		std::string myHostname;


	public:
		RestApi();
		~RestApi();


		void registerAsEnvironment();
	
	private:
		enum Method { POST, GET, PUT };

		json::Value callApi(Method method, const std::string &url, const std::vector<std::string> &headers, const json::Value &postData = json::Value::null);
		json::Value buildJson(const std::string &data);

	};

}