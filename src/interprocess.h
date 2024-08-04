#ifndef INTERPROCESS_H_INCLUDED
#define INTERPROCESS_H_INCLUDED

#include <vector>
#include <string>

namespace Interprocess
{
	struct Time
	{
		uint32_t hours;
		uint8_t minutes;
		uint8_t seconds;
		uint16_t milliseconds;
	};
	
	void Initialize();
	void Shutdown();
	
	void WriteTime(const Time& time);
	
	void WriteGameEnd(const Time& time);
	void WriteMapChange(const Time& time, const std::string& map);
	void WriteTimerReset(const Time& time);
	void WriteTimerStart(const Time& time);
	void WriteSSALeapOfFaith(const Time& time);
	Time GetTime();
}

#endif // INTERPROCESS_H_INCLUDED