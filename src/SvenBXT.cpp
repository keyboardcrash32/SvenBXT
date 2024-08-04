//
// Created by scriptedsnark on 7/18/24.
//

#include "SvenBXT.h"

#ifdef PLATFORM_WINDOWS
Utils utils = Utils::Utils(NULL, NULL, NULL);
Utils sv_utils = Utils::Utils(NULL, NULL, NULL);
#endif

funchook_t* g_lpFuncHook_Server;
funchook_t* g_lpFuncHook_Engine;
funchook_t* g_lpFuncHook_Client;

dllhandle_t g_lpEngine;
dllhandle_t g_lpServer;
dllhandle_t g_lpClient;

cl_enginefunc_t* g_lpEngfuncs;
enginefuncs_t* g_engfuncs;
globalvars_t* gpGlobals;
server_t* sv;

bool g_bHasLoaded = false;

void SvenBXT_HookClient();

void SvenBXT_FindEngineStuff()
{
	TRACE("Finding engine stuff...\n");

	g_lpFuncHook_Engine = funchook_create();

	int status;
#ifdef PLATFORM_LINUX
	g_lpEngfuncs = (cl_enginefunc_t*)Sys_GetProcAddress(g_lpEngine, "cl_enginefuncs");

	if (!g_lpEngfuncs)
	{
		Sys_Printf("[hw so] Failed to get \"cl_enginefuncs\".\n");
		return;
	}
	else
	{
		Sys_Printf("[hw so] Found cl_enginefuncs at %p.\n", g_lpEngfuncs);
		SvenBXT_HookClient();
	}

	// TODO: use GET_VARIABLE define
	enginefuncs_t** p_engfuncs = (enginefuncs_t**)Sys_GetProcAddress(g_lpEngine, "g_pengfuncsExportedToDlls");
	g_engfuncs = (*p_engfuncs);
	gpGlobals = (globalvars_t*)Sys_GetProcAddress(g_lpEngine, "gGlobalVariables");
	sv = (server_t*)Sys_GetProcAddress(g_lpEngine, "sv");

	if (!gpGlobals)
	{
		Sys_Printf("[hw so] Failed to get \"gGlobalVariables\".\n");
	}

	if (!g_engfuncs)
	{
		Sys_Printf("[hw so] Failed to get \"g_engfuncsExportedToDlls\".\n[hw dll] Sharing time to clients is not available.\n");
	}

	if (!sv)
	{
		Sys_Printf("[hw so] Failed to get \"sv\".\n");
	}

	GET_VARIABLE(Engine, ORIG_GL_Begin2D, _Z10GL_Begin2Dv);
	GET_VARIABLE(Engine, ORIG_GL_Finish2D, _Z11GL_Finish2Dv);

	GET_VARIABLE(Engine, ORIG_LoadThisDll, _Z11LoadThisDllPc);
	GET_VARIABLE(Engine, ORIG_SCR_BeginLoadingPlaque, _Z22SCR_BeginLoadingPlaquei);
	GET_VARIABLE(Engine, ORIG_SCR_EndLoadingPlaque, _Z20SCR_EndLoadingPlaquev);
	GET_VARIABLE(Engine, ORIG_GL_EndRendering, _Z15GL_EndRenderingv);
#else
	void* handle;
	static void* base;
	static size_t size;

	if (!MemUtils::GetModuleInfo(L"hw.dll", &handle, &base, &size))
	{
		Sys_Printf("Failed to get module info from engine.\n");
		return;
	}

	utils = Utils::Utils(handle, base, size);

	void* ClientDLL_Init;
	auto fClientDLL_Init = utils.FindAsync(
		ClientDLL_Init,
		patterns::engine::ClientDLL_Init,
		[&](auto pattern)
		{
			switch (pattern - patterns::engine::ClientDLL_Init.cbegin())
			{
			default:
			case 0: // Sven-5.25
				Sys_Printf("Searching cl_enginefuncs in Sven-5.25 pattern...\n");
				g_lpEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 332);

				if (g_lpEngfuncs)
				{
					Sys_Printf("[hw dll] Found cl_enginefuncs at 0x%p.\n", g_lpEngfuncs);
					SvenBXT_HookClient();
				}
				break;
			}
		});

	void* LoadThisDll;
	auto fLoadThisDll = utils.FindAsync(
		LoadThisDll,
		patterns::engine::LoadThisDll,
		[&](auto pattern)
		{
			switch (pattern - patterns::engine::LoadThisDll.cbegin())
			{
			default:
			case 0: // Sven-5.25
				Sys_Printf("Searching g_engfuncs in Sven-5.25 pattern...\n");
				enginefuncs_t** p_engfuncs = *reinterpret_cast<enginefuncs_t***>(reinterpret_cast<uintptr_t>(LoadThisDll) + 109);
				g_engfuncs = (*p_engfuncs);
				gpGlobals = *reinterpret_cast<globalvars_t**>(reinterpret_cast<uintptr_t>(LoadThisDll) + 67);

				if (g_engfuncs)
					Sys_Printf("[hw dll] Found g_engfuncs at 0x%p.\n", g_engfuncs);

				if (gpGlobals)
					Sys_Printf("[hw dll] Found gpGlobals at 0x%p.\n", gpGlobals);
				break;
			}
		});

	void* Host_ClearMemory;
	auto fHost_ClearMemory = utils.FindAsync(
		Host_ClearMemory,
		patterns::engine::Host_ClearMemory,
		[&](auto pattern)
		{
			switch (pattern - patterns::engine::Host_ClearMemory.cbegin())
			{
			default:
			case 0: // HL-9920
				Sys_Printf("Searching sv in HL-9920 pattern...\n");
				sv = *reinterpret_cast<server_t**>(reinterpret_cast<uintptr_t>(Host_ClearMemory) + 0xA4);

				if (sv)
				{
					Sys_Printf("[hw dll] Found sv at 0x%p.\n", sv);
				}
				break;
			case 1: // HL-8684
				Sys_Printf("Searching sv in HL-8684 pattern...\n");
				sv = *reinterpret_cast<server_t**>(reinterpret_cast<uintptr_t>(Host_ClearMemory) + 0x5E);
				if (sv)
				{
					Sys_Printf("[hw dll] Found sv at 0x%p.\n", sv);
				}
				break;
			case 2: // HL-4554
				Sys_Printf("Searching sv in HL-4554 pattern...\n");
				sv = *reinterpret_cast<server_t**>(reinterpret_cast<uintptr_t>(Host_ClearMemory) + 0x5C);
				if (sv)
				{
					Sys_Printf("[hw dll] Found sv at 0x%p.\n", sv);
				}
				break;
			case 3: // Sven-5.25
				Sys_Printf("Searching sv in Sven-5.25 pattern...\n");
				sv = *reinterpret_cast<server_t**>(reinterpret_cast<uintptr_t>(Host_ClearMemory) + 0x98);
				if (sv)
				{
					Sys_Printf("[hw dll] Found sv at 0x%p.\n", sv);
				}
				break;
			}
		});

	SPTEngineFind(GL_Begin2D);
	SPTEngineFind(GL_Finish2D);

	SPTEngineFind(LoadThisDll);
	SPTEngineFind(SCR_BeginLoadingPlaque);
	SPTEngineFind(SCR_EndLoadingPlaque);
	SPTEngineFind(GL_EndRendering);
#endif

	CreateHook(Engine, LoadThisDll);
	CreateHook(Engine, SCR_BeginLoadingPlaque);
	CreateHook(Engine, SCR_EndLoadingPlaque);
	CreateHook(Engine, GL_EndRendering);

	funchook_install(g_lpFuncHook_Engine, 0);
}

void SvenBXT_HookClient()
{
	TRACE("Hooking client...\n");
#ifdef PLATFORM_WINDOWS
	g_lpClient = Sys_GetModuleHandle("client");
#else
	g_lpClient = Sys_GetModuleHandle("svencoop/cl_dlls/client.so");
#endif

	if (!g_lpClient)
	{
		Sys_Printf("Failed to get client module handle.\n");
		return;
	}

	g_lpFuncHook_Client = funchook_create();

	CL_Initialize();

	funchook_install(g_lpFuncHook_Client, 0);
}

void SvenBXT_HookEngine()
{
	g_lpEngine = Sys_GetModuleHandle("hw" DLL_FORMAT);

	if (!g_lpEngine)
	{
		Sys_Printf("Failed to get engine module handle.\n");
		return;
	}

	SvenBXT_FindEngineStuff();
}

void SvenBXT_UnhookClient()
{
	if (g_lpFuncHook_Client)
	{
		funchook_uninstall(g_lpFuncHook_Client, 0);
		funchook_destroy(g_lpFuncHook_Client);
	}
}

void SvenBXT_UnhookEngine()
{
	if (g_lpFuncHook_Engine)
	{
		funchook_uninstall(g_lpFuncHook_Engine, 0);
		funchook_destroy(g_lpFuncHook_Engine);
	}
}

//-----------------------------------------------------------------------------
// Purpose: hook in right time a.k.a I don't wanna hook LoadLibraryA/dlopen
//-----------------------------------------------------------------------------
void WaitUntilClientLoads()
{
lbl_waitFor:
#ifdef PLATFORM_WINDOWS
	g_lpClient = Sys_GetModuleHandle("client");
#else
	g_lpClient = Sys_GetModuleHandle("svencoop/cl_dlls/client" DLL_FORMAT);
#endif

	if (g_lpClient)
	{
		SvenBXT_HookEngine();
		g_bHasLoaded = true;
	}
	else
		goto lbl_waitFor;
}

void SvenBXT_Main()
{
	if (g_bHasLoaded)
	{
		Sys_Printf("Loader/injector or whatever else tried to initialize SvenBXT again!");
		return;
	}

	// too lazy to do this using fopen
	std::ofstream ofs;
	ofs.open("svenbxt.log", std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	TRACE("Initializing SvenBXT...\n");

	std::thread t(WaitUntilClientLoads);
	t.detach();
}

void SvenBXT_Shutdown()
{
	TRACE("Shutting down...\n");

	SvenBXT_UnhookEngine();
	SvenBXT_UnhookClient();
	
	Interprocess::Shutdown();

	g_bHasLoaded = false;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		AllocConsole();
		FILE *in, *out;
		freopen_s(&in, "conin$", "r", stdin);
		freopen_s(&out, "conout$", "w+", stdout);
#endif
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(SvenBXT_Main), NULL, NULL, NULL);
		break;

	case DLL_PROCESS_DETACH:
		SvenBXT_Shutdown();
		break;
	}

	return TRUE;
}
#else
static __attribute__((constructor)) void Construct()
{
	SvenBXT_Main();
}

static __attribute__((destructor)) void Destruct()
{
	SvenBXT_Shutdown();
}
#endif
