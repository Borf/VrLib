#pragma once

#include <map>

namespace vrlib
{
	template <class T >
	class Multiton
	{
	public:
		static T* getInstance(std::string name)
		{
			static std::map<std::string, T*> mapje;
			std::map<std::string, T*>::iterator it = mapje.find(name);
			if (it == mapje.end())
			{
				T* t = new T();
				mapje[name] = t;
				return t;
			}
			else
				return it->second;
		}
	};
}