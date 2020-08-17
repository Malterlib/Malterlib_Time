// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace NMib
{
	namespace NTime
	{
		namespace NPlatform
		{
			namespace NPrivate
			{
#ifndef DCompiler_clang
				#if defined(__i386__)
					static __inline__ unsigned long long fg_RDTSC(void)
					{
						unsigned long long int x;
						__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
						return x;
					}
				#elif defined(__x86_64__)

					static __inline__ uint64 fg_RDTSC(void)
					{
						uint32 hi, lo;
						__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
						return ( (uint64)lo)|( ((uint64)hi)<<32 );
					}

				#elif defined(__powerpc__)
					static __inline__ unsigned long long fg_RDTSC(void)
					{
						unsigned long long int result=0;
						unsigned long int upper, lower,tmp;
						__asm__ volatile(
										 "0:                  \n"
										 "\tmftbu   %0           \n"
										 "\tmftb    %1           \n"
										 "\tmftbu   %2           \n"
										 "\tcmpw    %2,%0        \n"
										 "\tbne     0b         \n"
										 : "=r"(upper),"=r"(lower),"=r"(tmp)
										 );
						result = upper;
						result = result<<32;
						result = result|lower;

						return(result);
					}
				#else

				 #error "implement this"

				#endif
#endif
			}
		}
	}
}

#if defined(DCompiler_clang) && (defined(DArchitecture_arm64) || defined(DArchitecture_arm64e))
namespace NMib::NTime::NPlatform
{
	extern mint g_CyclesScale;
}
#endif

inline_always int64 NMib::NTime::NPlatform::fg_Timer_Cycles()
{
#ifdef DPlatformFamily_Emscripten
	return NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
#else
	#ifdef DCompiler_clang
		#if defined(DArchitecture_arm64) || defined(DArchitecture_arm64e)
			mint Counter;
			asm volatile ("mrs %0, CNTVCT_EL0" : "=r" (Counter));
			return Counter * g_CyclesScale;
		#else
			return __builtin_readcyclecounter();
		#endif
	#else
		return NMib::NTime::NPlatform::NPrivate::fg_RDTSC();
	#endif
#endif
}

inline_always int64 NMib::NTime::NPlatform::fg_Timer_CyclesFast()
{
#ifdef DPlatformFamily_Emscripten
   return NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
#else
   #ifdef DCompiler_clang
		#if defined(DArchitecture_arm64) || defined(DArchitecture_arm64e)
			mint Counter;
			asm volatile ("mrs %0, CNTVCT_EL0" : "=r" (Counter));
			return Counter * g_CyclesScale;
		#else
			return __builtin_readcyclecounter();
		#endif
   #else
	   return NMib::NTime::NPlatform::NPrivate::fg_RDTSC();
   #endif
#endif
}
