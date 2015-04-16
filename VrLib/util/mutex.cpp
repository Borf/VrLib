#include "mutex.h"
#include <VrLib/Log.h>
namespace vrlib
{
	void Mutex::lock()
	{
#ifdef WIN32
		EnterCriticalSection(&cs);
		if (debugEnabled)
			logger << "Mutex " << (int)&cs << " lock" << Log::newline;
#else
		pthread_mutex_lock(&mutex);
#endif
	}

	void Mutex::unLock()
	{
#ifdef WIN32
		LeaveCriticalSection(&cs);
		if (debugEnabled)
			logger << "Mutex " << (int)&cs << " unLock" << Log::newline;
#else
		pthread_mutex_unlock(&mutex);
#endif
	}

	Mutex::Mutex()
	{
#ifdef WIN32
		InitializeCriticalSection(&cs);
#else
		pthread_mutex_init(&mutex, NULL);
#endif
		debugEnabled = false;
	}

	Mutex::~Mutex()
	{
#ifdef WIN32
		DeleteCriticalSection(&cs);
#else
		pthread_mutex_destroy(&mutex);
#endif
	}
}