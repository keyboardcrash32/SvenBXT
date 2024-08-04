#include "interprocess.h"
#include <windows.h>
#include "Utils.h"
#include "SvenBXT.h"
#include "cl_dll/hud_timer.h"

namespace Interprocess
{
	static HANDLE pipe_SvenSplit = INVALID_HANDLE_VALUE;
	static OVERLAPPED overlapped;
	static bool writing_to_pipe;
	
	static void InitSvenSplitPipe()
	{
		pipe_SvenSplit = CreateNamedPipe(
			"\\\\.\\pipe\\" SVENSPLIT_PIPE_NAME,
			PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS,
			1,
			256 * 1000,
			0,
			0,
			NULL);
		if (pipe_SvenSplit == INVALID_HANDLE_VALUE)
		{
			Sys_Printf("Error opening the SvenSplit pipe: %d\n", GetLastError());
			Sys_Printf("SvenSplit integration is not available.\n");
			return;
		}
		Sys_Printf("Opened the SvenSplit pipe.\n");
	
		std::memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		if (overlapped.hEvent == NULL)
		{
			Sys_Printf("Error creating an event for overlapped: %d. Closing the SvenSplit pipe.\n", GetLastError());
			Sys_Printf("SvenSplit integration is not available.\n");
			CloseHandle(pipe_SvenSplit);
			pipe_SvenSplit = INVALID_HANDLE_VALUE;
		}
	}
	
	void Initialize()
	{
		InitSvenSplitPipe();
	}
	
	static void ShutdownSvenSplitPipe()
	{
		if (pipe_SvenSplit != INVALID_HANDLE_VALUE)
		{
			CloseHandle(pipe_SvenSplit);
			Sys_Printf("Closed the SvenSplit pipe.\n");
		}
		pipe_SvenSplit = INVALID_HANDLE_VALUE;
	
		CloseHandle(overlapped.hEvent);
		std::memset(&overlapped, 0, sizeof(overlapped));
	}
	
	void Shutdown()
	{
		ShutdownSvenSplitPipe();
	}
	
	static void WriteSvenSplit(const std::vector<char>& data)
	{
		if (pipe_SvenSplit == INVALID_HANDLE_VALUE)
			return;
	
		if (writing_to_pipe)
		{
			if (WaitForSingleObject(overlapped.hEvent, INFINITE) != WAIT_OBJECT_0)
			{
				// Some weird error?
				Sys_Printf("WaitForSingleObject failed with %d.\n", GetLastError());
				DisconnectNamedPipe(pipe_SvenSplit);
				return WriteSvenSplit(data);
			}
			writing_to_pipe = false;
		}
	
		if (!ConnectNamedPipe(pipe_SvenSplit, &overlapped))
		{
			auto err = GetLastError();
			if (err == ERROR_NO_DATA)
			{
				// Client has disconnected.
				DisconnectNamedPipe(pipe_SvenSplit);
				return WriteSvenSplit(data);
			}
			else if (err == ERROR_IO_PENDING)
			{
				// Waiting for someone to connect.
				return;
			}
			else if (err != ERROR_PIPE_CONNECTED)
			{
				// Some weird error with pipe?
				// Try remaking it.
				Sys_Printf("ConnectNamedPipe failed with %d.\n", err);
				ShutdownSvenSplitPipe();
				InitSvenSplitPipe();
				return WriteSvenSplit(data);
			}
		}
	
		if (!WriteFile(pipe_SvenSplit, data.data(), data.size(), NULL, &overlapped))
		{
			auto err = GetLastError();
			if (err == ERROR_IO_PENDING)
			{
				// Started writing.
				writing_to_pipe = true;
				return;
			}
			else
			{
				Sys_Printf("WriteFile failed with %d.\n", err);
				DisconnectNamedPipe(pipe_SvenSplit);
				return;
			}
		}
	}
	
	static size_t AddTimeToBuffer(char* buf, const Time& time)
	{
		std::memcpy(buf, &time.hours, sizeof(time.hours));
		std::memcpy(buf + sizeof(time.hours), &time.minutes, sizeof(time.minutes));
		std::memcpy(buf + sizeof(time.hours) + sizeof(time.minutes), &time.seconds, sizeof(time.seconds));
		std::memcpy(buf + sizeof(time.hours) + sizeof(time.minutes) + sizeof(time.seconds), &time.milliseconds, sizeof(time.milliseconds));
		return sizeof(time.hours) + sizeof(time.minutes) + sizeof(time.seconds) + sizeof(time.milliseconds);
	}
	
	void WriteTime(const Time& time)
	{
		if (!g_lpHUDTimer->interprocess_enable->value)
			return;

		std::vector<char> buf(10);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::TIME);
		AddTimeToBuffer(buf.data() + 2, time);
	
		if (g_lpHUDTimer->svensplit_time_update_frequency->value > 0.0f)
		{
			static auto last_time = std::chrono::steady_clock::now() - std::chrono::milliseconds(static_cast<long long>(1000 / g_lpHUDTimer->svensplit_time_update_frequency->value) + 1);
			auto now = std::chrono::steady_clock::now();
			if (now >= last_time + std::chrono::milliseconds(static_cast<long long>(1000 / g_lpHUDTimer->svensplit_time_update_frequency->value)))
			{
				WriteSvenSplit(buf);
				last_time = now;
			}
		}
		else
		{
			WriteSvenSplit(buf);
		}
	}
	
	void WriteGameEnd(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::GAMEEND);
		AddTimeToBuffer(buf.data() + 3, time);
	
		WriteSvenSplit(buf);
	}
	
	void WriteMapChange(const Time& time, const std::string& map)
	{
		int32_t size = static_cast<int32_t>(map.size());
	
		std::vector<char> buf(15 + size);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::MAPCHANGE);
		auto time_size = AddTimeToBuffer(buf.data() + 3, time);
	
		std::memcpy(buf.data() + 3 + time_size, &size, sizeof(size));
		std::memcpy(buf.data() + 3 + time_size + 4, map.data(), size);
	
		WriteSvenSplit(buf);
	}
	
	void WriteTimerReset(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::TIMER_RESET);
		AddTimeToBuffer(buf.data() + 3, time);
	
		WriteSvenSplit(buf);
	}
	
	void WriteTimerStart(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::TIMER_START);
		AddTimeToBuffer(buf.data() + 3, time);
	
		WriteSvenSplit(buf);
	}
	
	void WriteSSALeapOfFaith(const Time& time)
	{
		std::vector<char> buf(11);
		buf[0] = static_cast<char>(buf.size());
		buf[1] = static_cast<char>(MessageType::EVENT);
		buf[2] = static_cast<char>(EventType::SS_ALEAPOFFAITH);
		AddTimeToBuffer(buf.data() + 3, time);
	
		WriteSvenSplit(buf);
	}

	Interprocess::Time GetTime()
	{
		int hours = g_RTATimer.GetHours();
		int minutes = g_RTATimer.GetMinutes();
		int seconds = g_RTATimer.GetSeconds();
		int milliseconds = g_RTATimer.GetMilliseconds();

		return Interprocess::Time{
			static_cast<uint32_t>(hours),
			static_cast<uint8_t>(minutes),
			static_cast<uint8_t>(seconds),
			static_cast<uint16_t>(milliseconds)
		};
	}
}