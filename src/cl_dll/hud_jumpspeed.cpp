#include "SvenBXT.h"

CHudJumpspeed* g_lpJumpspeedHUD;

int CHudJumpspeed::Init()
{
	g_lpJumpspeedHUD = this;
	m_iFlags |= HUD_ACTIVE;

	hud_jumpspeed = CVAR_CREATE("sbxt_hud_jumpspeed", "0", 0);
	hud_jumpspeed_below_cross = CVAR_CREATE("sbxt_hud_jumpspeed_below_cross", "0", 0);
	hud_jumpspeed_height = CVAR_CREATE("sbxt_hud_jumpspeed_height", "0", 0);

	return 0;
}

int CHudJumpspeed::VidInit()
{
	passedTime = FADE_DURATION_JUMPSPEED;
	return 1;
}

int CHudJumpspeed::Draw(float flTime)
{
	if (!hud_jumpspeed->value)
		return 0;

	int x = ScreenWidth / 2;
	int y = ScreenHeight - gBXTHud.m_iFontHeight - gBXTHud.m_iFontHeight / 2;

	if (hud_jumpspeed_below_cross->value != 0.0f)
		y = ScreenHeight / 2 + gBXTHud.m_iFontHeight / 2 + gBXTHud.m_iFontHeight;
	else if (hud_jumpspeed_height->value != 0.0f)
		y = hud_jumpspeed_height->value;

	// SVEN_HUD_COLOR
	int r = 100;
	int g = 130;
	int b = 200;

	// Can be negative if we went back in time (for example, loaded a save).
	double timeDelta = std::fmax(flTime - lastTime, 0.0f);
	passedTime += timeDelta;

	// Check for Inf, NaN, etc.
	if (passedTime > FADE_DURATION_JUMPSPEED || !std::isnormal(passedTime))
		passedTime = FADE_DURATION_JUMPSPEED;

	float colorVel[3] = {r - fadingFrom[0] / FADE_DURATION_JUMPSPEED,
		g - fadingFrom[1] / FADE_DURATION_JUMPSPEED,
		b - fadingFrom[2] / FADE_DURATION_JUMPSPEED};

	r = static_cast<int>(r - colorVel[0] * (FADE_DURATION_JUMPSPEED - passedTime));
	g = static_cast<int>(g - colorVel[1] * (FADE_DURATION_JUMPSPEED - passedTime));
	b = static_cast<int>(b - colorVel[2] * (FADE_DURATION_JUMPSPEED - passedTime));

	lastTime = flTime;
	gBXTHud.DrawHudNumberCentered(x, y, speed, r, g, b);

	return 0;
}

void CHudJumpspeed::UpdateSpeed(const float velocity[3])
{
	if (FADE_DURATION_JUMPSPEED > 0.0f)
	{
		if ((velocity[2] != 0.0f && prevVel[2] == 0.0f) || (velocity[2] > 0.0f && prevVel[2] < 0.0f))
		{
			double difference = std::hypot(velocity[0], velocity[1]) - speed;
			if (difference != 0.0f)
			{
				if (difference > 0.0f)
				{
					fadingFrom[0] = 0;
					fadingFrom[1] = 255;
					fadingFrom[2] = 0;
				}
				else
				{
					fadingFrom[0] = 255;
					fadingFrom[1] = 0;
					fadingFrom[2] = 0;
				}

				passedTime = 0.0;
				speed = std::hypot(velocity[0], velocity[1]); // TODO: std::round?
			}
		}
	}
	VectorCopy(velocity, prevVel);
}