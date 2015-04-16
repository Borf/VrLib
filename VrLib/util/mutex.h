#pragma once
#include <VrLib/VrLib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
class cSignal;
#endif


namespace vrlib
{
	class Mutex
	{
	private:
#ifdef WIN32
		CRITICAL_SECTION cs;
#else
		pthread_mutex_t	mutex;
#endif
	public:
		bool debugEnabled;
		Mutex();
		~Mutex();

		void lock();
		void unLock();
		friend class cSignal;
	};

	//idea
	template<class T>
	class cMutexData
	{
	private:
		Mutex mutex;
		T data;
	public:
		//			T& operator .()
		//			{
		//				return data;
		//			}
		T* operator ->()
		{
			return &data;
		}

		T& operator = (T& other)
		{
			data = other;
		}
	};

}
