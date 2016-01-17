// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

inline_always int64 NMib::NTime::NPlatform::fg_Timer_CyclesFast()
{
	return __rdtsc();
}
