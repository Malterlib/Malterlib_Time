// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

class CSafeTimerReporterApplication : public NMib::CApplication
{
public:
	CSafeTimerReporterApplication()
	{

	}
	aint f_Main()
	{
		while (true)
		{
			uint64 Value = NMib::NSys::fg_TimerRaw_SafeGet();
			uint32 Value32 = Value;
			fp64 TimeUntilWrap = fp64(uint32(0xffffffff) - Value32) / 1000.0;
			DMibConErrOut("0x{nfh,sf0,sj16}: Time until wrap {} s\n", Value, TimeUntilWrap);
			NMib::NSys::fg_Thread_Sleep(1.0);
		}
		return NMib::NTest::fg_RunTests();
	}

};

DMibAppImplement(CSafeTimerReporterApplication);

