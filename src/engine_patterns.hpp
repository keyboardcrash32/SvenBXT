#ifdef REGS_PATTERNS_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in engine_patterns.hpp
#else //REGS_PATTERNS_HPP_RECURSE_GUARD

#define REGS_PATTERNS_HPP_RECURSE_GUARD

#ifndef REGS_PATTERNS_HPP_GUARD
#define REGS_PATTERNS_HPP_GUARD
#pragma once

#include "external/SPTLib/patterns.hpp"
#include "external/SPTLib/MemUtils.h"

namespace patterns
{
	namespace engine
	{
		PATTERNS(ClientDLL_HudInit,
			"HL-9920",
			"A1 ?? ?? ?? ?? 85 C0 75 ?? 68 47 04 00 00", // + 0x4D
			"HL-8684",
			"55 8B EC E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 85 C0", // ClientDLL_CheckStudioInterface (+ 0x2A)
			"HL-4554",
			"E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 85 C0 74 ?? 8B 44 24 ??", // ClientDLL_CheckStudioInterface (+ 0x28)
			"Sven-5.25",
			"A1 ?? ?? ?? ?? 85 C0 75 ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? A1 ?? ?? ?? ??"
		);

		PATTERNS(ClientDLL_Init,
			"HL-9920",
			"55 8B EC 81 EC 04 02 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 ?? 68 ?? ?? ?? ?? 8D 85 ?? ?? ?? ??",
			"HL-8684",
			"55 8B EC 81 EC 00 02 00 00 68 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? 68",
			"HL-4554",
			"81 EC 00 04 00 00 8D 44 24 00 68 ?? 6D ?? ??",
			"HL-3248",
			"81 EC 00 04 00 00 8D 44 24 00 68 94 75 EB 01",
			"Sven-5.25",
			"81 EC 04 02 00 00 A1 ?? ?? ?? ?? 33 C4 89 84 24 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8D 44 24 ?? 68 00 02 00 00"
		);

		PATTERNS(Netchan_CreateFragments,
			"HL-8684", "55 8B EC B8 14 00 01 00 E8 ?? ?? ?? ?? 53",
			"Sven-5.25", "B8 1C 00 04 00");

		PATTERNS(SZ_Write,
			"HL-8684", "55 8B EC 56 8B 75 ?? 85 F6 75 ?? 8B 45 ??",
			"Sven-5.25", "8B 4C 24 ?? 85 C9 75 ?? 56");

		PATTERNS(SZ_GetSpace,
			"HL-8684", "55 8B EC 56 8B 75 ?? 57 8B 7D ?? 8B 4E ??",
			"Sven-5.25", "56 8B 74 24 08 57 8B 7C 24 10 8B 4E 10 8B 56 0C 8D 04 39 3B C2 0F 8E ?? ?? ?? ?? F6 46 04 01 75 2E 8B 06 85 D2 75 11 85 C0 75 05 B8 ?? ?? ?? ?? 50 68 ?? ?? ?? ?? EB 0F 85 C0 75 05 B8");

		PATTERNS(Mod_LoadTextures,
			"HL-8684", "55 8B EC B8 C0 53 05 00",
			"Sven-5.25", "83 EC 74 A1 ?? ?? ?? ?? 33 C4 89 44 24 ?? 53 8B 5C 24 ??");

		PATTERNS(S_StartDynamicSound,
			"HL-9920",
			"55 8B EC 83 EC 5C A1 ?? ?? ?? ?? 33 C5 89 45 ?? 83 3D ?? ?? ?? ?? 00 8B 45 ?? 8B 4D ??",
			"HL-8684",
			"55 8B EC 83 EC 48 A1 ?? ?? ?? ?? 53",
			"HL-4554",
			"?? ?? ?? ?? ?? ?? ?? ?? 53 55 56 85 C0 57 C7 44 24 ?? 00 00 00 00 0F 84 ?? ?? ?? ??", // BXT steals my S_StartDynamicSound :o
			"Sven-5.25",
			"83 EC 58 A1 ?? ?? ?? ?? 33 C4 89 44 24 ?? 8B 44 24 ??");

		PATTERNS(S_StartStaticSound,
			"HL-9920",
			"55 8B EC 83 EC 50 A1 ?? ?? ?? ?? 33 C5 89 45 ?? 57",
			"HL-8684",
			"55 8B EC 83 EC 44 53 56 57 8B 7D ?? 85 FF",
			"HL-4554",
			"83 EC 44 53 55 8B 6C 24 ?? 56 85 ED");
	}
}

#endif //REGS_PATTERNS_HPP_GUARD

#undef REGS_PATTERNS_HPP_RECURSE_GUARD
#endif //REGS_PATTERNS_HPP_RECURSE_GUARD