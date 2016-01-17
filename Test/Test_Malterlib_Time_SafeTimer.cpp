// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Process/ProcessLaunch>

namespace 
{
	class CSafeTimer_Tests : public NMib::NTest::CTest
	{
	public:

		bool f_RecursiveTest(NMib::NStr::CStr& _oError)
		{
			NMib::NStr::CStr StdOut;
			uint32 ExitCode = 2;

			bool bRetVal = NMib::NProcess::CProcessLaunch::fs_LaunchBlock
				(
					NMib::NFile::CFile::fs_GetProgramPath()
					, NMib::NContainer::fg_CreateVector<NMib::NStr::CStr>("--Tests", fg_TestGetCurrentPath(), "--TestResults", "(All", "ProcessRecursive)")
					, StdOut
					, _oError
					, ExitCode
				)
			;

			return bRetVal && ExitCode == 0;
		}

		bool f_UnstableTimerTest(NMib::NFunction::TCFunction<void()>&& _fTamperWithTime)
		{
			NMib::NTime::CClock Clock;
			{
				// Precache
				Clock.f_Start();
				NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
				NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();
			}

			NMib::NTime::CTime Start = NMib::NTime::CTime::fs_NowUTC();
			auto TimerStart = NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();
			Clock.f_Start();
			_fTamperWithTime();
			{
				// Precache
				NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
				NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();
			}
			NMib::NSys::fg_Thread_Sleep(1.0f);
			NMib::NTime::CTime End = NMib::NTime::CTime::fs_NowUTC();
			fp32 TotalTimeTimer = Clock.f_GetTime();
			auto TimerEnd = NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();

			fp64 TotalTime = (End - Start).f_GetSecondsFraction();
			fp64 TotalTimeTimerRaw = fp64(TimerEnd - TimerStart) / fp64(NMib::NTime::NPlatform::fg_TimerRaw_SafeFrequency());

			bool bSuccess = true;

			if (!((TotalTime / TotalTimeTimerRaw) > 0.98 && (TotalTime / TotalTimeTimerRaw) < 1.02))
			{
				DMibConErrOut("TotalTime: {}, TotalTimeTimerRaw: {}{\n}", TotalTime << TotalTimeTimerRaw);
				bSuccess = false;
			}

			if (!((TotalTimeTimer / TotalTimeTimerRaw) > 0.98 && (TotalTimeTimer / TotalTimeTimerRaw) < 1.02))
			{
				DMibConErrOut("TotalTimeTimer: {}, TotalTimeTimerRaw: {}{\n}", TotalTimeTimer << TotalTimeTimerRaw);
				bSuccess = false;
			}

			return bSuccess;
		}

		void f_DoTests()
		{
#if !defined(DMibSafeTimerAvailable) || !DMibConfig_Tests_Enable
			return;
#else
			DMibTestSuite("Safe timer")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::CSystem_Time::fs_EnableSafeTimer(); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer wrap")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					for (mint i = 0; i < 5; ++i)
					{
						NMib::NTime::NPlatform::fg_MakeSafeTimerWrap(0.4f);
						DMibTestPath(NMib::NStr::fg_Format("Wrap {}", i));
						bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::CSystem_Time::fs_EnableSafeTimer();});
						if (!bRetVal)
							NMib::NTest::fg_TestSetReturnValue(1);
					}
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable always zero")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysZero); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable always negative")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysNegative); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable always max int")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysMaxInt); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable once zero")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_OnceZero); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable once max int")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_OnceMaxInt); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable once negative")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_OnceNegative); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr Error;
					bool bTestSuccessful = f_RecursiveTest(Error);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(Error) == DMibExpr(""));
				}
			};
#endif
		}

	};
}
DMibTestRegister(CSafeTimer_Tests, Malterlib::Time);
