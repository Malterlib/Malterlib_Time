// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#if defined(DArchitecture_arm64)
namespace NMib::NTime::NPlatform
{
	extern mint g_CyclesScale;
}
#endif

inline_always int64 NMib::NTime::NPlatform::fg_Timer_CyclesFast()
{
#if defined(DArchitecture_x86) || defined(DArchitecture_x64)
	return __rdtsc();
#elif defined(DArchitecture_arm64)
	return _ReadStatusReg(DMibArm64_CNTVCT_EL0) * g_CyclesScale;
#else
	#error "Implement this";
#endif
}
