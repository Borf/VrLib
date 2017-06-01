#pragma once

#include <map>

namespace vrlib
{
/*	template <class T>
	class Singleton
	{
	public:
		static T* getInstance()
		{
			static T* t = new T();
			return t;
		}
	};*/

	template <class T, class R = void>
	class Singleton
	{
	public:
		static T* getInstance(R r)
		{
			static T* t = new T(r);
			return t;
		}
	};

	template <class T>
	class Singleton<T, void>
	{
	public:
		static T* getInstance()
		{
			static T* t = new T();
			return t;
		}
	};


/*	template <class T, class R>
	class MapSingleton
	{
	public:
		static T* getInstance(R r)
		{
			static std::map<R, T> cache;
			auto it = cache.find(r);
			if (it == cache.end())
			{
				cache[r] = new T(r);
				it = cache.find(r);
			}
		
			return *it;
		}
	};*/

	template <class T>
	class AutoSingleton
	{
		T* object;
	public:
		AutoSingleton()
		{
			object = T::getInstance();
		}

		T* operator -> ()
		{
			return object;
		}
	};
}