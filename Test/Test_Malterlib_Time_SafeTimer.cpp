// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Process/ProcessLaunch>

#define DMalteribTimeTestWithPrecision

#ifdef DMalteribTimeTestWithPrecision
static fp64 const g_CheckTime = 0.2;
static fp64 const g_AllowedDiff = 1.1;
#else
static fp64 const g_CheckTime = 1.0;
static fp64 const g_AllowedDiff = 1.03;
#endif

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
					, NMib::NContainer::fg_CreateVector<NMib::NStr::CStr>("--test", fg_TestGetCurrentPath(), "--filter-results", "[\"All\"]", "--process-recursive")
					, StdOut
					, _oError
					, ExitCode
				)
			;

			return bRetVal && ExitCode == 0;
		}

		bool f_UnstableTimerTest(NMib::NFunction::TCFunction<void()>&& _fTamperWithTime, bool _bExpectSafeTime = true)
		{
			NMib::NTime::CStopwatch Stopwatch;
			{
				// Precache
				Stopwatch.f_Start();
				NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
				NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();
			}

			_fTamperWithTime();
			NMib::NTime::CTime Start = NMib::NTime::CTime::fs_NowUTC();
			auto TimerStart = NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();
			Stopwatch.f_Start();
			{
				// Precache
				NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
				NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();
			}
			NMib::NSys::fg_Thread_Sleep(g_CheckTime);
			NMib::NTime::CTime End = NMib::NTime::CTime::fs_NowUTC();
			fp32 TotalTimeTimer = Stopwatch.f_GetTime();
			auto TimerEnd = NMib::NTime::NPlatform::fg_TimerRaw_SafeGet();

			fp64 TotalTime = (End - Start).f_GetSecondsFraction();
			fp64 TotalTimeTimerRaw = fp64(TimerEnd - TimerStart) / fp64(NMib::NTime::NPlatform::fg_TimerRaw_SafeFrequency());

			bool bSuccess = true;

#if defined(DMibSafeTimerAvailable)
			if (_bExpectSafeTime && !NMib::NTime::CSystem_Time::fs_IsSafeTimerEnabled())
			{
				DMibConErrOut("Safe timer is not enabled when it should be{\n}", 0);
				bSuccess = false;
			}

			if (!_bExpectSafeTime && NMib::NTime::CSystem_Time::fs_IsSafeTimerEnabled())
			{
				DMibConErrOut("Safe timer is enabled when it should not be{\n}", 0);
				bSuccess = false;
			}
#endif
			if (!((TotalTime / TotalTimeTimerRaw) > fp64(1.0) / g_AllowedDiff && (TotalTime / TotalTimeTimerRaw) < g_AllowedDiff))
			{
				DMibConErrOut("TotalTime: {}, TotalTimeTimerRaw: {} = {}{\n}", TotalTime, TotalTimeTimerRaw, (TotalTime / TotalTimeTimerRaw));
				bSuccess = false;
			}

			if (!((TotalTimeTimer / TotalTimeTimerRaw) > fp64(1.0) / g_AllowedDiff && (TotalTimeTimer / TotalTimeTimerRaw) < g_AllowedDiff))
			{
				DMibConErrOut("TotalTimeTimer: {}, TotalTimeTimerRaw: {} = {}{\n}", TotalTimeTimer, TotalTimeTimerRaw, (TotalTimeTimer / TotalTimeTimerRaw));
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
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::CSystem_Time::fs_EnableSafeTimer(); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			for (mint iWrapPos = 0; iWrapPos < 4; ++iWrapPos)
			{
				NMib::NStr::CStr WrapPos = NMib::NStr::fg_Format("WrapPos{}", iWrapPos);
				DMibTestCategory(WrapPos)
				{
					DMibTestSuite("Safe timer wrap")
					{
						if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
						{
#ifdef DMalteribTimeTestWithPrecision
							NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
							for (mint i = 0; i < 5; ++i)
							{
								NMib::NTime::CSystem_Time::fs_MakeSafeTimerWrap(g_CheckTime * 0.4f, iWrapPos);
								DMibTestPath(NMib::NStr::fg_Format("Wrap {}", i));
								bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::CSystem_Time::fs_EnableSafeTimer();});
								if (!bRetVal)
									NMib::NTest::fg_TestSetReturnValue(1);
							}
						}
						else
						{
							NMib::NStr::CStr ErrorString;
							bool bTestSuccessful = f_RecursiveTest(ErrorString);

							DMibTest(DMibExpr(bTestSuccessful));
							DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
						}
					};
					DMibTestSuite("Safe timer wrap backwards")
					{
						if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
						{
#ifdef DMalteribTimeTestWithPrecision
							NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
							for (mint i = 0; i < 5; ++i)
							{
								NMib::NTime::CSystem_Time::fs_MakeSafeTimerWrap(0.0f, iWrapPos);
								NMib::NSys::fg_Thread_Sleep(g_CheckTime * 0.1f);
								DMibTestPath(NMib::NStr::fg_Format("Wrap {}", i));
								bool bRetVal = f_UnstableTimerTest
									(
										[]
										{
											NMib::NTime::CTime::fs_NowUTC();
											NMib::NTime::NPlatform::fg_TimerRaw_SafeOffset(-g_CheckTime * 0.5f);
											NMib::NTime::CTime::fs_NowUTC();
											NMib::NTime::NPlatform::fg_TimerRaw_SafeOffset(g_CheckTime * 0.5f);
											NMib::NTime::CTime::fs_NowUTC();
										}
										, false
									)
								;
								if (!bRetVal)
									NMib::NTest::fg_TestSetReturnValue(1);
							}
						}
						else
						{
							NMib::NStr::CStr ErrorString;
							bool bTestSuccessful = f_RecursiveTest(ErrorString);

							DMibTest(DMibExpr(bTestSuccessful));
							DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
						}
					};

					DMibTestSuite("Safe timer wrap no auto enable")
					{
						if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
						{
#ifdef DMalteribTimeTestWithPrecision
							NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
							for (mint i = 0; i < 5; ++i)
							{
								NMib::NTime::CSystem_Time::fs_MakeSafeTimerWrap(g_CheckTime * 0.4f, iWrapPos);
								DMibTestPath(NMib::NStr::fg_Format("Wrap {}", i));
								bool bRetVal = f_UnstableTimerTest([] {	}, false);
								if (!bRetVal)
									NMib::NTest::fg_TestSetReturnValue(1);
							}
						}
						else
						{
							NMib::NStr::CStr ErrorString;
							bool bTestSuccessful = f_RecursiveTest(ErrorString);

							DMibTest(DMibExpr(bTestSuccessful));
							DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
						}
					};
				};
			}
			DMibTestSuite("Safe timer backwards")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					for (mint i = 0; i < 5; ++i)
					{
						DMibTestPath(NMib::NStr::fg_Format("Wrap {}", i));
						bool bRetVal = f_UnstableTimerTest
							(
								[]
								{
									NMib::NTime::CTime::fs_NowUTC();
									NMib::NTime::NPlatform::fg_TimerRaw_SafeOffset(-g_CheckTime * 0.1f);
									NMib::NTime::CTime::fs_NowUTC();
									NMib::NTime::NPlatform::fg_TimerRaw_SafeOffset(g_CheckTime * 0.1f);
									NMib::NTime::CTime::fs_NowUTC();
								}
								, false
							)
						;
						if (!bRetVal)
							NMib::NTest::fg_TestSetReturnValue(1);
					}
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable always zero")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysZero); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable always negative")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysNegative); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable always max int")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysMaxInt); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable once zero")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_OnceZero); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable once max int")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_OnceMaxInt); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};

			DMibTestSuite("Safe timer unstable once negative")
			{
				if (fg_TestReportFlags() & ETestReportFlag_ProcessRecursive)
				{
#ifdef DMalteribTimeTestWithPrecision
					NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision();
#endif
					bool bRetVal = f_UnstableTimerTest([] {	NMib::NTime::NPlatform::fg_MakeTimerUnstable(NMib::NTime::NPlatform::EUnstableTimerMode_OnceNegative); });
					NMib::NTest::fg_TestSetReturnValue(bRetVal ? 0 : 1);
				}
				else
				{
					NMib::NStr::CStr ErrorString;
					bool bTestSuccessful = f_RecursiveTest(ErrorString);

					DMibTest(DMibExpr(bTestSuccessful));
					DMibTest(DMibExpr(ErrorString) == DMibExpr(""));
				}
			};
#endif
		}

	};
}
DMibTestRegister(CSafeTimer_Tests, Malterlib::Time);
