#pragma once

#include <windows.h>

#include <VrLib/Singleton.h>

#include <list>

namespace vrlib
{

	class FrameData;

	class PerfMon : public Singleton < PerfMon >
	{
		double frequency;
		__int64 startTime;

		FrameData* currentFrame;
	public:
		PerfMon();
		~PerfMon();


		std::list<FrameData*> performanceInfo;

		void resetTimer();
		double getTime();
	};

}