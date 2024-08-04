#ifdef HUD_TIMER_H_RECURSE_GUARD
#error Recursive header files inclusion detected in hud_timer.h
#else //HUD_TIMER_H_RECURSE_GUARD

#define HUD_TIMER_H_RECURSE_GUARD

#ifndef HUD_TIMER_H_GUARD
#define HUD_TIMER_H_GUARD
#pragma once

#ifdef __cplusplus

extern CTimer g_RTATimer;
extern CTimer g_IGTTimer;

class CHudTimer: public CBXTHudBase
{
public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void TimerStart();
	void TimerStop();
	void TimerReset();

	bool ShouldSync() { return (hud_timer_serversync->value && !UTIL_IsHost()); }
	bool IsInILMode() { return hud_timer_il_mode->value; };

	cvar_t* svensplit_time_update_frequency;
	cvar_t* interprocess_enable;

private:
	cvar_t* hud_timer_serversync;
	cvar_t* hud_timer_il_mode;
	cvar_t* hud_timer;
	cvar_t* hud_timer_anchor;
	cvar_t* hud_timer_offset;
};

extern CHudTimer* g_lpHUDTimer;

#else //!__cplusplus
#error C++ compiler required to compile hud_timer.h
#endif //__cplusplus

#endif //HUD_TIMER_H_GUARD

#undef HUD_TIMER_H_RECURSE_GUARD
#endif //HUD_TIMER_H_RECURSE_GUARD