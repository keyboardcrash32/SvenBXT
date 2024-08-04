#include "SvenBXT.h"

CTimer g_RTATimer;
CTimer g_IGTTimer;

CHudTimer* g_lpHUDTimer;

void __CmdFunc_TimerStart()
{
	if (!g_lpHUDTimer)
		return;

	if (g_lpHUDTimer->ShouldSync())
		return;

	g_lpHUDTimer->TimerStart();
}

void __CmdFunc_TimerStop()
{
	if (!g_lpHUDTimer)
		return;

	if (g_lpHUDTimer->ShouldSync())
		return;

	g_lpHUDTimer->TimerStop();
}

void __CmdFunc_TimerReset()
{
	if (!g_lpHUDTimer)
		return;

	if (g_lpHUDTimer->ShouldSync())
		return;

	g_lpHUDTimer->TimerStop();
	g_lpHUDTimer->TimerReset();
}

int __MsgFunc_BXTTimer(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	if (!g_lpHUDTimer->ShouldSync())
		return 0;

	long time = READ_LONG();
	bool stop = READ_BYTE();

	g_RTATimer.SyncTimer(time, stop);
	Interprocess::WriteTime(Interprocess::GetTime());

	return 0;
}

int CHudTimer::Init()
{
	g_lpHUDTimer = this;
	m_iFlags |= HUD_ALWAYSRENDER;

	hud_timer_serversync = CVAR_CREATE("sbxt_timer_serversync", "1", 0);
	hud_timer_il_mode = CVAR_CREATE("sbxt_timer_il_mode", "0", 0);
	hud_timer = CVAR_CREATE("sbxt_hud_timer", "0", 0);
	hud_timer_anchor = CVAR_CREATE("sbxt_hud_timer_anchor", "0.0 0.5", 0);
	hud_timer_offset = CVAR_CREATE("sbxt_hud_timer_offset", "", 0);

	svensplit_time_update_frequency = CVAR_CREATE("_sbxt_svensplit_time_update_frequency", "41", 0);
	interprocess_enable = CVAR_CREATE("sbxt_interprocess_enable", "0", 0);

	HOOK_COMMAND("sbxt_timer_start", TimerStart);
	HOOK_COMMAND("sbxt_timer_stop", TimerStop);
	HOOK_COMMAND("sbxt_timer_reset", TimerReset);

	HOOK_MESSAGE(BXTTimer);

	return 0;
}

int CHudTimer::VidInit()
{
	return 1;
}

int CHudTimer::Draw(float time)
{
	Interprocess::WriteTime(Interprocess::GetTime());

	if (!hud_timer->value)
		return 0;

	int x, y;
	gBXTHud.GetPosition(hud_timer_offset, hud_timer_anchor, &x, &y, 0, 0);

	int hours = g_RTATimer.GetHours();
	int minutes = g_RTATimer.GetMinutes();
	int seconds = g_RTATimer.GetSeconds();
	int milliseconds = g_RTATimer.GetMilliseconds();
	
	if (hud_timer->value == 1)
	{
		char rta_timer[64];
		sprintf(rta_timer, "RTA | %d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
		g_lpEngfuncs->VGUI2_DrawLocalizedHudString(x, y, rta_timer, SVEN_HUD_COLOR);
	}
	else if (hud_timer->value >= 2)
	{
		if (hours)
		{
			x = gBXTHud.DrawNumberTimer(hours, x, y, 1);
			gBXTHud.DrawColon(x, y);
			x += NumberWidth;
		}

		if (hours || minutes)
		{
			int fieldMinWidth = (hours && minutes < 10) ? 2 : 1;
			x = gBXTHud.DrawNumberTimer(minutes, x, y, fieldMinWidth);
			gBXTHud.DrawColon(x, y);
			x += NumberWidth;
		}

		int fieldMinWidth = ((hours || minutes) && seconds < 10) ? 2 : 1;
		x = gBXTHud.DrawNumberTimer(seconds, x, y, fieldMinWidth);

		gBXTHud.DrawDecimalSeparator(x, y);
		x += NumberWidth;

		gBXTHud.DrawNumberTimer(milliseconds, x, y, 3);
	}

	//sprintf_s(igt_timer, "IGT | %d:%02d:%02d.%03d", g_IGTTimer.GetHours(), g_IGTTimer.GetMinutes(), g_IGTTimer.GetSeconds(), g_IGTTimer.GetMilliseconds());
	//g_lpEngfuncs->VGUI2_DrawLocalizedHudString(x, y + CharHeight, rta_timer, SVEN_HUD_COLOR);

	return 0;
}

void CHudTimer::TimerStart()
{
	g_RTATimer.StartTimer();
	//g_IGTTimer.StartTimer();

	Interprocess::WriteTimerStart(Interprocess::GetTime());

	if (UTIL_IsHost())
	{
		SV_SyncTimer();
	}
}

void CHudTimer::TimerStop()
{
	g_RTATimer.StopTimer();
	//g_IGTTimer.StopTimer();

	if (UTIL_IsHost())
	{
		SV_SyncTimer();
	}
}

void CHudTimer::TimerReset()
{
	g_RTATimer.ResetTimer();
	//g_IGTTimer.ResetTimer();

	Interprocess::WriteTimerReset(Interprocess::GetTime());

	if (UTIL_IsHost())
	{
		SV_SyncTimer();
	}
}