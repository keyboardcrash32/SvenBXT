#include "SvenBXT.h"

cvar_t* hud_bordersize;

// From hl_weapons
_HUD_PostRunCmd ORIG_HUD_PostRunCmd = nullptr;

// From cdll_int
_Initialize ORIG_Initialize = nullptr;
_HUD_VidInit ORIG_HUD_VidInit = nullptr;
_HUD_Init ORIG_HUD_Init = nullptr;
_HUD_Redraw ORIG_HUD_Redraw = nullptr;
_HUD_UpdateClientData ORIG_HUD_UpdateClientData = nullptr;
_HUD_Reset ORIG_HUD_Reset = nullptr;
_HUD_PlayerMove ORIG_HUD_PlayerMove = nullptr;
_HUD_PlayerMoveInit ORIG_HUD_PlayerMoveInit = nullptr;
_HUD_PlayerMoveTexture ORIG_HUD_PlayerMoveTexture = nullptr;
_HUD_ConnectionlessPacket ORIG_HUD_ConnectionlessPacket = nullptr;
_HUD_GetHullBounds ORIG_HUD_GetHullBounds = nullptr;
_HUD_Frame ORIG_HUD_Frame = nullptr;
_HUD_VoiceStatus ORIG_HUD_VoiceStatus = nullptr;
_HUD_DirectorMessage ORIG_HUD_DirectorMessage = nullptr;
_HUD_ChatInputPosition ORIG_HUD_ChatInputPosition = nullptr;

// From demo
_Demo_ReadBuffer ORIG_Demo_ReadBuffer = nullptr;

// From entity
_HUD_AddEntity ORIG_HUD_AddEntity = nullptr;
_HUD_CreateEntities ORIG_HUD_CreateEntities = nullptr;
_HUD_StudioEvent ORIG_HUD_StudioEvent = nullptr;
_HUD_TxferLocalOverrides ORIG_HUD_TxferLocalOverrides = nullptr;
_HUD_ProcessPlayerState ORIG_HUD_ProcessPlayerState = nullptr;
_HUD_TxferPredictionData ORIG_HUD_TxferPredictionData = nullptr;
_HUD_TempEntUpdate ORIG_HUD_TempEntUpdate = nullptr;
_HUD_GetUserEntity ORIG_HUD_GetUserEntity = nullptr;

// From in_camera
_CAM_Think ORIG_CAM_Think = nullptr;
_CL_IsThirdPerson ORIG_CL_IsThirdPerson = nullptr;
_CL_CameraOffset ORIG_CL_CameraOffset = nullptr;

// From input
_KB_Find ORIG_KB_Find = nullptr;
_CL_CreateMove ORIG_CL_CreateMove = nullptr;
_HUD_Shutdown ORIG_HUD_Shutdown = nullptr;
_HUD_Key_Event ORIG_HUD_Key_Event = nullptr;

// From inputw32
_IN_ActivateMouse ORIG_IN_ActivateMouse = nullptr;
_IN_DeactivateMouse ORIG_IN_DeactivateMouse = nullptr;
_IN_MouseEvent ORIG_IN_MouseEvent = nullptr;
_IN_Accumulate ORIG_IN_Accumulate = nullptr;
_IN_ClearStates ORIG_IN_ClearStates = nullptr;

// From tri
_HUD_DrawNormalTriangles ORIG_HUD_DrawNormalTriangles = nullptr;
_HUD_DrawTransparentTriangles ORIG_HUD_DrawTransparentTriangles = nullptr;

// From view
_V_CalcRefdef ORIG_V_CalcRefdef = nullptr;

// From GameStudioModelRenderer
_HUD_GetStudioModelInterface ORIG_HUD_GetStudioModelInterface = nullptr;

int HOOKED_HUD_VidInit(void)
{
	TRACE("Initializing visuals...\n");
	gBXTHud.VidInit();

	return ORIG_HUD_VidInit();
}

int HOOKED_HUD_Redraw(float time, int intermission)
{
	gBXTHud.Redraw(time, intermission);

	return ORIG_HUD_Redraw(time, intermission);
}

void CL_Initialize()
{
	int status;

	hud_bordersize = g_lpEngfuncs->GetCvarPointer("hud_bordersize"); // TODO: use this for HUD padding

	Find(Client, V_CalcRefdef);
	Find(Client, HUD_VidInit);
	Find(Client, HUD_Redraw);
	CreateHook(Client, V_CalcRefdef);
	CreateHook(Client, HUD_VidInit);
	CreateHook(Client, HUD_Redraw);

	TRACE("Initializing interprocesses...\n");
	Interprocess::Initialize();
	
	TRACE("Initializing HUD...\n");
	gBXTHud.Init();
}
