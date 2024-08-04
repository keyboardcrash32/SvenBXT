#include "SvenBXT.h"

typedef void (FASTCALL* _CNihilanth_DyingThink)(void* thisptr);

_CNihilanth_DyingThink ORIG_CNihilanth_DyingThink = NULL;
_LoadThisDll ORIG_LoadThisDll = NULL;

int gmsgBXTTimer = 0;

void FASTCALL HOOKED_CNihilanth_DyingThink(void* thisptr)
{
	g_lpHUDTimer->TimerStop();

	Interprocess::WriteGameEnd(Interprocess::GetTime());

	ORIG_CNihilanth_DyingThink(thisptr);
}

void HOOKED_LoadThisDll(char* szDllFilename)
{
	ORIG_LoadThisDll(szDllFilename);

	g_lpServer = Sys_GetModuleHandle(szDllFilename);

	if (!g_lpServer)
	{
		Sys_Printf("[Server] Failed to get module handle. Path: \"%s\"\n", szDllFilename);
		return;
	}

	SV_HookStuff();
	SV_Initialize(); // may break some things but seems ok
}

void SV_SyncTimer()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgBXTTimer);
	WRITE_LONG(g_RTATimer.GetTime());
	WRITE_BYTE(g_RTATimer.IsStopped());
	MESSAGE_END();
}

void SV_Initialize()
{
	if (!g_engfuncs)
		return;

	gmsgBXTTimer = REG_USER_MSG("BXTTimer", -1);
	g_engfuncs->pfnAddServerCommand("sbxt_sv_synctimer", SV_SyncTimer);
}

void SV_HookStuff()
{
	g_lpFuncHook_Server = funchook_create();

	int status;

#ifdef PLATFORM_LINUX
	GET_VARIABLE(Server, ORIG_CNihilanth_DyingThink, _ZN10CNihilanth10DyingThinkEv);
#else // PLATFORM_WINDOWS
	static void* handle;
	static void* base;
	static size_t size;

	if (!MemUtils::GetModuleInfo(L"svencoop\\dlls\\server.dll", &handle, &base, &size)) // TODO: use szDllFilename
	{
		Sys_Printf("[Server] Failed to get module info from server.\n");
		return;
	}

	sv_utils = Utils::Utils(handle, base, size);

	SPTServerFind(CNihilanth_DyingThink);
#endif

	CreateHook(Server, CNihilanth_DyingThink);
	funchook_install(g_lpFuncHook_Server, 0);
}