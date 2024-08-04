#ifdef SVENBXT_H_RECURSE_GUARD
#error Recursive header files inclusion detected in SvenBXT.h
#else // SVENBXT_H_RECURSE_GUARD

#define SVENBXT_H_RECURSE_GUARD

#ifndef SVENBXT_H_GUARD
#define SVENBXT_H_GUARD
#pragma once

#ifdef __cplusplus

// GLOBAL DEFINE
#define SVENBXT_VERSION __DATE__ // maybe something else? :thinking:
#define SVENBXT_GITHUB_URL "https://github.com/ScriptedSnark/SvenBXT"

// INTERPROCESS & SVENSPLIT
#define SVENSPLIT_PIPE_NAME "SvenBXT-SvenSplit"

enum class MessageType : unsigned char
{
	TIME = 0x00,
	EVENT = 0x01
};

enum class EventType : unsigned char
{
	GAMEEND = 0x00,
	MAPCHANGE = 0x01,
	TIMER_RESET = 0x02,
	TIMER_START = 0x03,
	SS_ALEAPOFFAITH = 0x04
};

#include "interprocess.h"

// WINDOWS
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <DbgHelp.h>
#else
#include <dlfcn.h>
#include <stdarg.h>
#include <cstring>
#endif

// STL
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <array>
#include <random>
#include <fstream>
#include <thread>

#include "external/funchook/include/funchook.h"

#ifdef PLATFORM_WINDOWS
#include "iface.hpp"

#include "external/SPTLib/Utils.hpp"
#include "engine_patterns.hpp"
extern Utils utils;
extern Utils sv_utils;
#endif

#include <stdio.h>
#include "hlsdk_mini.hpp"
#include "engine/sound.h"
#include "engine/server.h"

// OWN STUFF
#include "Utils.h"
#include "CTimer.h"

extern funchook_t* g_lpFuncHook_Server;
extern funchook_t* g_lpFuncHook_Client;
extern dllhandle_t g_lpEngine;
extern dllhandle_t g_lpServer;
extern dllhandle_t g_lpClient;
extern cl_enginefunc_t* g_lpEngfuncs;
extern enginefuncs_t* g_engfuncs;
extern globalvars_t* gpGlobals;
extern server_t* sv;

#include "cl_dll/cdll_int.h"
#include "cl_dll/hud.h"
#include "cl_dll/parsemsg.h"
#include "cl_dll/view.h"

#include "engine/gl_screen.h"

#include "dlls/enginecallback.h"
#include "dlls/server.h"

// HUD
#include "cl_dll/hud_speedometer.h"
#include "cl_dll/hud_viewangles.h"
#include "cl_dll/hud_origin.h"
#include "cl_dll/hud_timer.h"
#include "cl_dll/hud_jumpspeed.h"

#ifdef PLATFORM_WINDOWS
#define FASTCALL __fastcall
#elif defined(PLATFORM_LINUX)
#define FASTCALL
#else
#error Unsupported platform
#endif

#define GET_VARIABLE(tag, resultVar, procName)								   \
	resultVar = (decltype(resultVar))Sys_GetProcAddress(g_lp##tag, #procName); \
	if (!resultVar)                                                            \
		Sys_Printf("[" #tag "] Failed to get \"" #procName "\".\n");            \
	else                                                                       \
		Sys_Printf("[" #tag "] Found " #procName " at %p.\n", resultVar)      \

#define Find(lib, func_name)                                                                                  \
	if ((ORIG_##func_name = reinterpret_cast<_##func_name>(Sys_GetProcAddress(g_lp##lib, "" #func_name "")))) \
		Sys_Printf("[" #lib "] Found " #func_name " at %p.\n", ORIG_##func_name);                             \
	else                                                                                                      \
		Sys_Printf("[" #lib "] Could not find " #func_name ".\n")


#ifdef PLATFORM_WINDOWS
#define SPTEngineFind(future_name)                                                                                                \
	{                                                                                                                       \
		auto f##future_name = utils.FindAsync(ORIG_##future_name, patterns::engine::future_name);                           \
		auto pattern = f##future_name.get();                                                                                \
		if (ORIG_##future_name)                                                                                             \
		{                                                                                                                   \
			Sys_Printf("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, pattern->name()); \
		}                                                                                                                   \
		else                                                                                                                    \
			Sys_Printf("[hw dll] Could not find " #future_name ".\n");															\
	}

#define SPTServerFind(future_name)                                                                                              \
	{                                                                                                                           \
		auto f##future_name = sv_utils.FindAsync(ORIG_##future_name, patterns::server::future_name);                               \
		auto pattern = f##future_name.get();                                                                                    \
		if (ORIG_##future_name)                                                                                                 \
		{                                                                                                                       \
			Sys_Printf("[Server] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, pattern->name()); \
		}                                                                                                                       \
	}
#endif

#define CreateHook(lib, func_name)                                                                           \
	do                                                                                                       \
	{                                                                                                        \
		Sys_Printf("[" #lib "] Creating hook for " #func_name "...\n");                                      \
		status = funchook_prepare(g_lpFuncHook_##lib, (void**)&ORIG_##func_name, (void*)HOOKED_##func_name); \
		if (status == FUNCHOOK_ERROR_SUCCESS)                                                                \
			Sys_Printf("[" #lib "] Hooked " #func_name "!\n");                                               \
		else                                                                                                 \
			Sys_Printf("[" #lib "] Failed to hook " #func_name "! Code: %i\n", status);                      \
	} while (0)

#else //!__cplusplus
#error C++ compiler required to compile SvenBXT.h
#endif //__cplusplus

#endif // SVENBXT_H_GUARD

#undef SVENBXT_H_RECURSE_GUARD
#endif // SVENBXT_H_RECURSE_GUARD
