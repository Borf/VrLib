#include <vrlib/log.h>
#include "thread.h"

#ifndef WIN32
#include <sys/prctl.h>
#endif

namespace vrlib
{
	Thread::Thread(std::string pName) : name(pName)
	{
		running = false;
	}

	Thread::~Thread()
	{
#ifdef WIN32
		WaitForSingleObject(threadHandle, INFINITE);
#else
		if(running)
			pthread_join(thread, NULL);
#endif
	}

	void Thread::start()
	{
#ifdef WIN32
		threadHandle = CreateThread(NULL, 0, Thread::threadStarter, this, 0, &threadId);
#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		pthread_create(&thread, &attr, threadStarter, this);
		pthread_attr_destroy(&attr);
#endif
		running = true;
		logger << "Thread " << name << " started" << Log::newline;
	}

#ifdef WIN32
	DWORD WINAPI Thread::threadStarter(LPVOID lpParam)
	{

		return ((Thread*)lpParam)->run();
	}
#else
	void* Thread::threadStarter(void* lpParam)
	{
#ifdef WIN32
#else
		prctl(PR_SET_NAME, (unsigned long)((Thread*)lpParam)->name.substr(0, math::min(16, (int)((Thread*)lpParam)->name.size())).c_str());
#endif
		pthread_exit((void*)((Thread*)lpParam)->run());
	}
#endif


	void Thread::waitForTermination()
	{
#ifdef WIN32
		WaitForSingleObject(threadHandle, INFINITE);
#else
		void * bla;
		pthread_join(thread, &bla);
#endif
	}


	void Thread::sleep(int i)
	{
#ifdef WIN32
		Sleep(i * 1000);
#else
		::sleep(i);
#endif
	}
}