#include "Util.h"

namespace vrlib
{
	namespace util
	{


		std::vector<std::string> split(std::string value, const std::string &seperator)
		{
			std::vector<std::string> ret;
			while (value.find(seperator) != std::string::npos)
			{
				int index = value.find(seperator);
				if (index != 0)
					ret.push_back(value.substr(0, index));
				value = value.substr(index + seperator.length());
			}
			ret.push_back(value);
			return ret;
		}

	}

}