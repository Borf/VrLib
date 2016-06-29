#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <VrLib/Singleton.h>

#include <list>

namespace vrlib
{

	class FrameData;

	class PerfMon : public Singleton < PerfMon >
	{
#ifdef WIN32
		double frequency;
		__int64 startTime;
#endif
		FrameData* currentFrame;
	public:
		PerfMon();
		~PerfMon();


		std::list<FrameData*> performanceInfo;

		void resetTimer();
		double getTime();
	};

}