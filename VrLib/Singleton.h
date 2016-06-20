#pragma once
namespace vrlib
{
	template <class T>
	class Singleton
	{
	public:
		static T* getInstance()
		{
			static T* t = new T();
			return t;
		}
	};


	template <class T>
	class AutoSingleton
	{
		T* object;
	public:
		AutoSingleton()
		{
			object = T->getInstance();
		}
		
		T* operator -> ()
		{
			return object;
		}
	};
}