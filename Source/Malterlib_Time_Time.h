// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Contract/Contract>
#include <Mib/Concurrency/AsyncResult>

namespace NMib::NTime
{
	class CTimeSpanConvert_BabylonianCommon;
	class CTime;

	namespace NPrivate
	{
		struct CConst
		{
			constexpr static uint64 mc_SecondsInDay = 86'400;
			constexpr static uint64 mc_SecondsInDayHalf = mc_SecondsInDay / 2;
			constexpr static pfp64 mc_SecondsInDayFp = 86'400.0;
			constexpr static uint64 mc_SecondsInHour = 3'600;
			constexpr static uint64 mc_SecondsInHourHalf = mc_SecondsInHour / 2;
			constexpr static pfp64 mc_SecondsInHourFp = 3'600.0;
			constexpr static uint64 mc_SecondsInMinute = 60;
			constexpr static uint64 mc_SecondsInMinuteHalf = mc_SecondsInMinute / 2;
			constexpr static pfp64 mc_SecondsInMinuteFp = 60.0;
			constexpr static pfp64 mc_SecondsInWeekFp = 604'800.0;

			constexpr static uint64 mc_DaysInMedianYear = 365;
			constexpr static uint64 mc_DaysInLeapYear = 366;

			constexpr static int64 mc_SecondsInDaySigned = mc_SecondsInDay;
			constexpr static int64 mc_SecondsInHourSigned = mc_SecondsInHour;
			constexpr static int64 mc_SecondsInMinuteSigned = mc_SecondsInMinute;
			constexpr static int64 mc_SecondsInWeekSigned = 604'800;

			constexpr static uint64 mc_InvalidTimeSeconds = constant_uint64(0xffffffffffffffff);
			constexpr static int64 mc_InvalidSpanSeconds = constant_int64(0x7fffffffffffffff);
			constexpr static uint64 mc_InvalidFractionInt = constant_uint64(0xffffffffffffffff);

			constexpr static uint64 mc_EndOfTime = mc_InvalidTimeSeconds - 1;

			constexpr static uint64 mc_FractionDividend = 9'223'372'031'757'829'470; // Fraction of second one second is 9223372031757829470 (2006688023 * Cs-133)
			constexpr static uint64 mc_FractionDividendHalf = mc_FractionDividend / 2;
			constexpr static pfp64 mc_FractionDividendFp = 9'223'372'031'757'829'470.0;
			constexpr static pfp64 mc_FractionDividendFpInv = 1.0 / mc_FractionDividendFp;

			constexpr static uint64 mc_UnixEpochSeconds = constant_uint64(237'148'622'167'132'800);
			constexpr static uint64 mc_YearZeroPlus1DaySeconds = constant_uint64(237'148'560'000'000'000);
			constexpr static uint64 mc_YearOneBcSeconds = mc_YearZeroPlus1DaySeconds - mc_SecondsInDay;
			constexpr static uint64 mc_YearOneAdSeconds = mc_YearOneBcSeconds + mc_SecondsInDay * mc_DaysInLeapYear;
			constexpr static uint64 mc_YearTwoAdSeconds = mc_YearOneAdSeconds + mc_SecondsInDay * mc_DaysInMedianYear;

			constexpr static uint64 mc_AverageSecondsInYear = constant_uint64(31'556'952);
			constexpr static uint64 mc_SecondsInLeapYear = mc_SecondsInDay * mc_DaysInLeapYear;
			constexpr static uint64 mc_YearOffset = constant_uint64(7'514'938'800);
			constexpr static int64 mc_MaxYear = ((mc_InvalidTimeSeconds - 1) - mc_YearOneBcSeconds) / mc_AverageSecondsInYear;
			constexpr static int64 mc_MinYear = -int64(mc_YearOneBcSeconds / mc_AverageSecondsInYear);
			constexpr static uint64 mc_YearOffsetSeconds = (mc_YearOffset * mc_AverageSecondsInYear) - mc_YearZeroPlus1DaySeconds;

			static_assert(mc_MaxYear == constant_int64(577'039'110'548));
			static_assert(mc_MinYear == constant_int64(-7'514'938'705));
		};
	}

	class CTimeSpan
	{
		friend class CTimeSpanConvert_BabylonianCommon;
		friend class CTime;
	private:
		int64 m_Seconds = NPrivate::CConst::mc_InvalidSpanSeconds;
		uint64 m_Fraction = 0;

#ifdef DMibDebuggerHelpers
		static ch8 const* fsp_DebugStr(void* _pTimeSpan);
#endif
	public:
		explicit CTimeSpan(int64 _Value)
			: m_Seconds (_Value)
		{
#ifdef DMibDebuggerHelpers
			static_assert(TCInstantiateValue<&CTimeSpan::fsp_DebugStr>::mc_Value);
#endif
		}
		explicit CTimeSpan(int64 _Value, uint64 _Fraction)
			: m_Seconds (_Value)
			, m_Fraction(_Fraction)
		{
#ifdef DMibDebuggerHelpers
			static_assert(TCInstantiateValue<&CTimeSpan::fsp_DebugStr>::mc_Value);
#endif
		}
		CTimeSpan()
		{
#ifdef DMibDebuggerHelpers
			static_assert(TCInstantiateValue<&CTimeSpan::fsp_DebugStr>::mc_Value);
#endif
		}

		bool f_IsValid() const
		{
			return m_Seconds != NPrivate::CConst::mc_InvalidSpanSeconds;
		}

		CTimeSpan operator + (const CTimeSpan &_Other) const
		{
			CTimeSpan Ret;
			Ret.m_Seconds = m_Seconds + _Other.m_Seconds;
			uint64 FracAdd = _Other.m_Fraction;
			uint64 Temp = NPrivate::CConst::mc_FractionDividend - m_Fraction;
			if (FracAdd >= Temp)
			{
				++Ret.m_Seconds;
				Ret.m_Fraction = FracAdd - Temp;
			}
			else
				Ret.m_Fraction = m_Fraction + FracAdd;
			return Ret;
		}

		CTimeSpan operator - (const CTimeSpan &_Other) const
		{
			CTimeSpan Ret;

			Ret.m_Seconds = m_Seconds - _Other.m_Seconds;
			uint64 FracSub = _Other.m_Fraction;
			uint64 Temp = m_Fraction;

			if (FracSub > Temp)
			{
				--Ret.m_Seconds;
				Ret.m_Fraction = NPrivate::CConst::mc_FractionDividend - (FracSub - Temp);
			}
			else
				Ret.m_Fraction = Temp - FracSub;

			return Ret;
		}

		CTimeSpan operator - () const
		{
			CTimeSpan Temp;
			Temp.f_SetSeconds(0);
			return Temp - *this;
		}

		CTimeSpan operator * (int64 _nTimes) const
		{
			CTimeSpan Ret;
			Ret.m_Seconds = m_Seconds * _nTimes;
			Ret.m_Fraction = m_Fraction;
			if (m_Fraction)
			{
				int128 TempFrac = m_Fraction;
				TempFrac *= _nTimes;
				int64 nSeconds = int64(TempFrac / int128(NPrivate::CConst::mc_FractionDividend));
				Ret.m_Seconds += nSeconds;
				Ret.m_Fraction = uint64(TempFrac - int128(NPrivate::CConst::mc_FractionDividend) * (int128)nSeconds);
			}

			return Ret;
		}

		CTimeSpan operator / (int64 _nTimes) const
		{
			CTimeSpan Ret;
			int128 Temp = (int128(m_Seconds) * int128(NPrivate::CConst::mc_FractionDividend) + int128(m_Fraction)) / int128(_nTimes);
			int64 nSeconds = int64(Temp / int128(NPrivate::CConst::mc_FractionDividend));
			Ret.m_Seconds = nSeconds;
			Ret.m_Fraction = uint64(Temp - int128(NPrivate::CConst::mc_FractionDividend) * int128(nSeconds));

			return Ret;
		}

		CTimeSpan& operator += (const CTimeSpan &_Other)
		{
			m_Seconds += _Other.m_Seconds;
			uint64 FracAdd = _Other.m_Fraction;
			uint64 Temp = NPrivate::CConst::mc_FractionDividend - m_Fraction;
			if (FracAdd >= Temp)
			{
				++m_Seconds;
				m_Fraction = FracAdd - Temp;
			}
			else
				m_Fraction += FracAdd;

			return *this;
		}

		CTimeSpan& operator -= (const CTimeSpan &_Other)
		{
			m_Seconds -= _Other.m_Seconds;
			uint64 FracSub = _Other.m_Fraction;
			uint64 Temp = m_Fraction;

			if (FracSub > Temp)
			{
				--m_Seconds;
				m_Fraction = NPrivate::CConst::mc_FractionDividend - (FracSub - Temp);
			}
			else
				m_Fraction -= FracSub;

			return *this;
		}

		CTimeSpan& operator *= (int64 _nTimes)
		{
			m_Seconds *= _nTimes;
			if (m_Fraction)
			{
				int128 TempFrac = m_Fraction;
				TempFrac *= _nTimes;
				int64 nSeconds = fg_Convert<int64>(TempFrac / int128(NPrivate::CConst::mc_FractionDividend));
				m_Seconds += nSeconds;
				m_Fraction = fg_Convert<int64>(TempFrac - int128(NPrivate::CConst::mc_FractionDividend) * int128(nSeconds));
			}

			return *this;
		}
		CTimeSpan& operator /= (int64 _nTimes)
		{
			int128 Temp = (int128(m_Seconds) * int128(NPrivate::CConst::mc_FractionDividend) + int128(m_Fraction)) / int128(_nTimes);
			int64 nSeconds = fg_Convert<int64>(Temp / int128(NPrivate::CConst::mc_FractionDividend));
			m_Seconds = nSeconds;
			m_Fraction = fg_Convert<int64>(Temp - int128(NPrivate::CConst::mc_FractionDividend) * int128(nSeconds));

			return *this;
		}

		auto operator <=> (const CTimeSpan &_Other) const = default;

		int64 f_GetSeconds() const
		{
			return m_Seconds;
		}

		void f_SetSeconds(int64 _Seconds)
		{
			m_Seconds = _Seconds;
			m_Fraction = 0;
		}

		void f_SetSecondsNoFraction(int64 _Seconds)
		{
			m_Seconds = _Seconds;
		}

		uint64 f_GetFractionInt() const
		{
			return m_Fraction;
		}

		void f_SetFractionInt(uint64 _Fraction)
		{
			m_Fraction = _Fraction;
		}

		fp64 f_GetFraction() const
		{
			return fp64(m_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
		}

		void f_SetFraction(fp64 _Fraction)
		{
			m_Fraction = (_Fraction * NPrivate::CConst::mc_FractionDividendFp).f_ToUnsignedInt();
		}

		fp64 f_GetSecondsFraction() const
		{
			return fp64(m_Seconds) + fp64(m_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
		}

		void f_SetSecondsFraction(fp64 _Seconds)
		{
			f_SetSeconds(_Seconds.f_ToInt());
			f_SetFraction(_Seconds.f_Fraction());
		}

		static CTimeSpan fs_UTCOffset();

		static CTimeSpan fs_SmallestTime()
		{
			CTimeSpan Ret;
			Ret.m_Seconds = 0;
			Ret.m_Fraction = 1;
			return Ret;
		}

	};

	class CTimeConvert_BabylonianCommon;
	class CTimeConvert_ProlepticGreogrian;

	class CTime
	{
		friend class CTimeConvert_BabylonianCommon;
		friend class CTimeConvert_ProlepticGreogrian;
	private:
		uint64 m_Seconds = NPrivate::CConst::mc_InvalidTimeSeconds; // Number of seconds since -7'514'938'706-11-23 (Gregorian Proleptic).
		uint64 m_Fraction = 0;

#ifdef DMibDebuggerHelpers
		static ch8 const* fsp_DebugStr(void* _pTime);
#endif

		constexpr CTime(uint64 _Seconds, uint64 _Fraction)
			: m_Seconds(_Seconds)
			, m_Fraction(_Fraction)
		{
#ifdef DMibDebuggerHelpers
			static_assert(TCInstantiateValue<&CTime::fsp_DebugStr>::mc_Value);
#endif
		}
	public:
		CTime()
		{
#ifdef DMibDebuggerHelpers
			static_assert(TCInstantiateValue<&CTime::fsp_DebugStr>::mc_Value);
#endif
		}

		bool f_IsValid() const
		{
			return m_Seconds != NPrivate::CConst::mc_InvalidTimeSeconds;
		}

		auto operator <=> (const CTime &_Other) const = default;

		CTime& operator += (const CTimeSpan &_Other)
		{
			m_Seconds += _Other.m_Seconds;
			uint64 FracAdd = _Other.m_Fraction;
			uint64 Temp = NPrivate::CConst::mc_FractionDividend - m_Fraction;
			if (FracAdd >= Temp)
			{
				++m_Seconds;
				m_Fraction = FracAdd - Temp;
			}
			else
				m_Fraction += FracAdd;
			return *this;
		}

		CTime& operator -= (const CTimeSpan &_Other)
		{
			m_Seconds -= _Other.m_Seconds;
			uint64 FracSub = _Other.m_Fraction;
			uint64 Temp = m_Fraction;

			if (FracSub > Temp)
			{
				--m_Seconds;
				m_Fraction = NPrivate::CConst::mc_FractionDividend - (FracSub - Temp);
			}
			else
				m_Fraction -= FracSub;
			return *this;
		}

		CTimeSpan operator - (const CTime &_Other) const
		{
			CTimeSpan Ret;
			DMibSafeCheck(this->f_IsValid() && _Other.f_IsValid(), "You should not calculate spans on invalid dates");

			int64 ToSet;
			if (m_Seconds > _Other.m_Seconds)
			{
				uint64 SpanSeconds = m_Seconds - _Other.m_Seconds;
				if (SpanSeconds >= (constant_uint64(1) << 63) - 1)
				{
					SpanSeconds = (constant_uint64(1) << 63) - 2;
				}
				ToSet = SpanSeconds;
			}
			else
			{
				uint64 SpanSeconds = _Other.m_Seconds - m_Seconds;
				if (SpanSeconds >= (constant_uint64(1) << 63) - 1)
				{
					SpanSeconds = (constant_uint64(1) << 63) - 2;
				}
				ToSet = -SpanSeconds;
			}
			Ret.f_SetSecondsNoFraction(ToSet);
			uint64 FracSub = _Other.m_Fraction;
			uint64 Temp = m_Fraction;

			if (FracSub > Temp)
			{
				--Ret.m_Seconds;
				Ret.f_SetFractionInt(NPrivate::CConst::mc_FractionDividend - (FracSub - Temp));
			}
			else
				Ret.f_SetFractionInt(Temp - FracSub);

			return Ret;
		}

		CTime operator - (const CTimeSpan &_Other) const
		{
			DMibSafeCheck(f_IsValid(), "Must be valid");
			CTime Ret;

			Ret.m_Seconds = m_Seconds - _Other.m_Seconds;
			uint64 FracSub = _Other.m_Fraction;
			uint64 Temp = m_Fraction;

			if (FracSub > Temp)
			{
				--Ret.m_Seconds;
				Ret.m_Fraction = NPrivate::CConst::mc_FractionDividend - (FracSub - Temp);
			}
			else
				Ret.m_Fraction = Temp - FracSub;

			return Ret;
		}

		CTime operator + (const CTimeSpan &_Other) const
		{
			DMibSafeCheck(f_IsValid(), "Must be valid");
			CTime Ret;
			Ret.m_Seconds = m_Seconds + _Other.m_Seconds;
			uint64 FracAdd = _Other.m_Fraction;
			uint64 Temp = NPrivate::CConst::mc_FractionDividend - m_Fraction;
			if (FracAdd >= Temp)
			{
				++Ret.m_Seconds;
				Ret.m_Fraction = FracAdd - Temp;
			}
			else
				Ret.m_Fraction = m_Fraction + FracAdd;
			return Ret;
		}

		CTime &operator ++ ()
		{
			DMibSafeCheck(f_IsValid(), "Must be valid");
			if (m_Fraction == NPrivate::CConst::mc_FractionDividend - 1)
			{
				m_Fraction = 0;
				++m_Seconds;
			}
			else
			{
				++m_Fraction;
			}

			return *this;
		}

		CTime &operator -- ()
		{
			DMibSafeCheck(f_IsValid(), "Must be valid");
			if (m_Fraction == 0)
			{
				m_Fraction = NPrivate::CConst::mc_FractionDividend - 1;
				--m_Seconds;
			}
			else
			{
				--m_Fraction;
			}

			return *this;
		}

		constexpr uint64 f_GetSeconds() const
		{
			return m_Seconds;
		}

		void f_SetSeconds(uint64 _Seconds)
		{
			m_Seconds = _Seconds;
			m_Fraction = 0;
		}

		void f_SetSecondsNoFraction(uint64 _Seconds)
		{
			m_Seconds = _Seconds;
		}

		constexpr uint64 f_GetFractionInt() const
		{
			return m_Fraction;
		}

		void f_SetFractionInt(uint64 _Fraction)
		{
			m_Fraction = _Fraction;
		}

		fp64 f_GetFraction() const
		{
			return fp64(m_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
		}

		void f_SetFraction(fp64 _Fraction)
		{
			m_Fraction = (_Fraction * NPrivate::CConst::mc_FractionDividendFp).f_ToUnsignedInt();
		}

		CTime f_ToUTC() const;
		CTime f_ToLocal() const;

		CTime f_ToUtcLegacy() const;
		CTime f_ToLocalLegacy() const;

		static CTime fs_NowLocal();

		static CTime fs_NowUTC();

		static CTime fs_EndOfTime()
		{
			CTime Ret;
			Ret.m_Seconds = NPrivate::CConst::mc_EndOfTime;
			Ret.m_Fraction = NPrivate::CConst::mc_FractionDividend - 1;

			return Ret;
		}

		static CTime fs_StartOfTime()
		{
			CTime Ret;
			Ret.m_Seconds = 0;
			Ret.m_Fraction = 0;

			return Ret;
		}

		constexpr static CTime fs_Create(uint64 _Seconds, uint64 _Fraction = 0)
		{
			return CTime{_Seconds, _Fraction};
		}

		static int64 fs_GetResolution();

		fp64 f_GetSecondsFraction() const
		{
			return fp64(m_Seconds) + fp64(m_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
		}

		struct CFormatOptions
		{
			uint32 m_Components = 0;
			bool m_bDateOnly = false;
			bool m_bFullPrecision = false;
			bool m_bBcSuffix = false;
			ch32 m_DateTimeSeparator = ' ';
			ch32 m_DateSeparator = '-';
			ch32 m_TimeSeparator = ':';
			ch32 m_SecondSeparator = '.';
		};

		template <typename tf_COption, typename tf_CArgs>
		bool f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option, tf_CArgs &_Args) const;

		template <typename tf_CStr, typename tf_COptions>
		void f_Format(tf_CStr &_FormatInto, tf_COptions const &_Options) const;
	};

	namespace NPrivate
	{
		template <typename tf_CType>
		struct TCIsTime
		{
			constexpr static bool mc_bValue = false;
		};

		template <>
		struct TCIsTime<CTime>
		{
			constexpr static bool mc_bValue = true;
		};
	}

	template <typename tf_CType>
	concept cIsTime = NPrivate::TCIsTime<NTraits::TCRemoveReferenceAndQualifiers<tf_CType>>::mc_bValue;

	class CTimeSpanConvert_BabylonianCommon
	{
	protected:
		const CTimeSpan *m_pTime;
	public:
		CTimeSpanConvert_BabylonianCommon(const CTimeSpan &_Time)
		: m_pTime(&_Time)
		{
		}

		CTimeSpanConvert_BabylonianCommon &operator = (const CTimeSpan &_Time)
		{
			m_pTime = &_Time;

			return *this;
		}

	public:

		static CTimeSpan fs_CreateSpan(int64 _Weeks, int64 _Days, int64 _Hours = 0, int64 _Minutes = 0, int64 _Seconds = 0, fp64 _Fraction = 0.0)
		{
			CTimeSpan NewSpan;
			int64 Seconds = _Seconds;
			Seconds += _Minutes * NPrivate::CConst::mc_SecondsInMinuteSigned;
			Seconds += _Hours * NPrivate::CConst::mc_SecondsInHourSigned;
			Seconds += _Days * NPrivate::CConst::mc_SecondsInDaySigned;
			Seconds += _Weeks * NPrivate::CConst::mc_SecondsInWeekSigned;

			NewSpan.f_SetSecondsNoFraction(Seconds);
			NewSpan.f_SetFraction(_Fraction);

			return NewSpan;
		}

		static CTimeSpan fs_CreateWeekSpan(int64 _Weeks)
		{
			CTimeSpan NewSpan;
			NewSpan.f_SetSeconds(_Weeks * 604800);
			return NewSpan;
		}

		static CTimeSpan fs_CreateDaySpan(int64 _Days)
		{
			CTimeSpan NewSpan;
			NewSpan.f_SetSeconds(_Days * NPrivate::CConst::mc_SecondsInDaySigned);
			return NewSpan;
		}

		static CTimeSpan fs_CreateHourSpan(int64 _Hours)
		{
			CTimeSpan NewSpan;
			NewSpan.f_SetSeconds(_Hours * NPrivate::CConst::mc_SecondsInHourSigned);
			return NewSpan;
		}


		static CTimeSpan fs_CreateMinuteSpan(int64 _Minutes)
		{
			CTimeSpan NewSpan;
			NewSpan.f_SetSeconds(_Minutes * NPrivate::CConst::mc_SecondsInMinuteSigned);
			return NewSpan;
		}


		static CTimeSpan fs_CreateSecondSpan(int64 _Seconds)
		{
			CTimeSpan NewSpan;
			NewSpan.f_SetSeconds(_Seconds);
			return NewSpan;
		}

		static CTimeSpan fs_CreateSpanFromWeeks(fp64 _Weeks)
		{
			return fs_CreateSpanFromSeconds(_Weeks * 604800.0);
		}

		static CTimeSpan fs_CreateSpanFromDays(fp64 _Days)
		{
			return fs_CreateSpanFromSeconds(_Days * NPrivate::CConst::mc_SecondsInDayFp);
		}

		static CTimeSpan fs_CreateSpanFromHours(fp64 _Hours)
		{
			return fs_CreateSpanFromSeconds(_Hours * NPrivate::CConst::mc_SecondsInHourFp);
		}

		static CTimeSpan fs_CreateSpanFromMinutes(fp64 _Minutes)
		{
			return fs_CreateSpanFromSeconds(_Minutes * NPrivate::CConst::mc_SecondsInMinuteFp);
		}

		static CTimeSpan fs_CreateSpanFromSeconds(fp64 _Seconds)
		{
			CTimeSpan NewSpan;

			int64 Seconds = _Seconds.f_ToInt();
			NewSpan.f_SetSecondsNoFraction(Seconds);
			NewSpan.f_SetFraction(_Seconds - Seconds);

			return NewSpan;
		}

		static CTimeSpan fs_CreateSpanFromSeconds(int64 _Seconds, fp64 _Fraction)
		{
			CTimeSpan NewSpan;

			NewSpan.f_SetSecondsNoFraction(_Seconds);
			NewSpan.f_SetFraction(_Fraction);

			return NewSpan;
		}

		int64 f_GetWeeks() const
		{
			return f_GetSeconds() / NPrivate::CConst::mc_SecondsInWeekSigned;
		}

		fp64 f_GetWeeksFloat() const
		{
			return m_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInWeekFp;
		}

		int64 f_GetDays() const
		{
			return f_GetSeconds() / NPrivate::CConst::mc_SecondsInDaySigned;
		}

		fp64 f_GetDaysFloat() const
		{
			return m_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInDayFp;
		}

		int64 f_GetHours() const
		{
			return f_GetSeconds() / NPrivate::CConst::mc_SecondsInHourSigned;
		}

		fp64 f_GetHoursFloat() const
		{
			return m_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInHourFp;
		}

		int64 f_GetMinutes() const
		{
			return f_GetSeconds() / NPrivate::CConst::mc_SecondsInMinuteSigned;
		}

		fp64 f_GetMinutesFloat() const
		{
			return m_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInMinuteFp;
		}

		int64 f_GetSeconds() const
		{
			auto Seconds = m_pTime->f_GetSeconds();
			if (Seconds < 0 && m_pTime->f_GetFractionInt() != 0)
				return Seconds + 1;
			else
				return Seconds;
		}

		fp64 f_GetSecondsFloat() const
		{
			return m_pTime->f_GetSecondsFraction();
		}

		fp64 f_GetFraction() const
		{
			auto Seconds = m_pTime->f_GetSeconds();
			if (Seconds < 0)
				return fp64(NPrivate::CConst::mc_FractionDividend - m_pTime->f_GetFractionInt()) * NPrivate::CConst::mc_FractionDividendFpInv;
			else
				return m_pTime->f_GetFraction();
		}

		aint f_GetHourOfDay() const
		{
			int64 Seconds = fg_Abs(f_GetSeconds());
			Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInDaySigned) * NPrivate::CConst::mc_SecondsInDaySigned;
			return Seconds / NPrivate::CConst::mc_SecondsInHourSigned;
		}

		aint f_GetMinuteOfHour() const
		{
			int64 Seconds = fg_Abs(f_GetSeconds());
			Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInDaySigned) * NPrivate::CConst::mc_SecondsInDaySigned;
			Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInHourSigned) * NPrivate::CConst::mc_SecondsInHourSigned;
			return Seconds / NPrivate::CConst::mc_SecondsInMinuteSigned;
		}

		aint f_GetSecondOfMinute() const
		{
			int64 Seconds = fg_Abs(f_GetSeconds());
			Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInDaySigned) * NPrivate::CConst::mc_SecondsInDaySigned;
			Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInHourSigned) * NPrivate::CConst::mc_SecondsInHourSigned;
			Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInMinuteSigned) * NPrivate::CConst::mc_SecondsInMinuteSigned;
			return Seconds;
		}
	};


	class CTimeConvert_BabylonianCommon
	{
	protected:
		const NTime::CTime *m_pTime;
	public:
		CTimeConvert_BabylonianCommon(const NTime::CTime &_Time)
		: m_pTime(&_Time)
		{
		}

		CTimeConvert_BabylonianCommon &operator = (const NTime::CTime &_Time)
		{
			m_pTime = &_Time;

			return *this;
		}
		uint64 f_UnixMilliseconds() const;
		static CTime fs_FromUnixMilliseconds(uint64 _Milliseconds);
		uint64 f_UnixSeconds() const;
		static CTime fs_FromUnixSeconds(uint64 _Seconds);
		uint64 f_UnixMinutes() const;
		static CTime fs_FromUnixMinutes(uint64 _Minutes);
		fp64 f_UnixSecondsFraction() const;
		static CTime fs_FromUnixSecondsFraction(fp64 _Seconds);

	protected:

		static void fsp_AddTime(CTime &_AddResult, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, fp64 _Fraction = 0)
		{
			_AddResult.m_Seconds += _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second;
			uint64 FracAdd = (_Fraction * NPrivate::CConst::mc_FractionDividendFp).f_ToUnsignedInt();
			if (FracAdd >= NPrivate::CConst::mc_FractionDividend)
				FracAdd = NPrivate::CConst::mc_FractionDividend - 1;
			uint64 Temp = NPrivate::CConst::mc_FractionDividend - _AddResult.m_Fraction;
			if (FracAdd >= Temp)
			{
				++_AddResult.m_Seconds;
				_AddResult.m_Fraction = FracAdd - Temp;
			}
			else
				_AddResult.m_Fraction += FracAdd;
		}

		static void fsp_AddTimeIntFrac(CTime &_AddResult, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _Fraction = 0)
		{
			_AddResult.m_Seconds += _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second;
			uint64 FracAdd = _Fraction;
			uint64 Temp = NPrivate::CConst::mc_FractionDividend - _AddResult.m_Fraction;
			if (FracAdd >= Temp)
			{
				++_AddResult.m_Seconds;
				_AddResult.m_Fraction = FracAdd - Temp;
			}
			else
				_AddResult.m_Fraction += FracAdd;
		}
		constexpr static CTime fsp_AddTimeIntFracConstexr(CTime const &_Old, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _Fraction = 0)
		{
			return (_Fraction >= (NPrivate::CConst::mc_FractionDividend - _Old.m_Fraction))
				? CTime
				{
					_Old.m_Seconds + _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second + 1
					, _Fraction - (NPrivate::CConst::mc_FractionDividend - _Old.m_Fraction)
				}
				: CTime{_Old.m_Seconds + _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second, _Old.m_Fraction + _Fraction}
			;
		}
	};

	class CTimeConvert_ISOWeek;

	class CTimeConvert_ProlepticGreogrian : public CTimeConvert_BabylonianCommon
	{
		friend class CTimeConvert_ISOWeek;

	public:
		class CDateTime
		{
		public:
			int64 m_Year; // Note: 1 BC is year 0.
			aint m_Month;
			aint m_DayOfYear;
			aint m_DayOfMonth;
			aint m_DayOfWeek;
			aint m_Hour;
			aint m_Minute;
			aint m_Second;
			fp64 m_Fraction;
			uint64 m_FractionInt = NPrivate::CConst::mc_InvalidFractionInt;
			bool m_bIsLeapYear;
		};

	private:
		constexpr static const int32 ms_MonthDayOfYear[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
		constexpr static const int32 ms_DaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		constexpr static inline_small aint fsp_GetDayOfYearFromMonth(aint _Month)
		{
			return ms_MonthDayOfYear[_Month];
		}

		constexpr static inline_small aint fsp_IsLeapYear(int64 _Year)
		{
			return !(_Year & 0x3) && ((_Year % 100) || (!(_Year % 400)));
		}

		constexpr static inline_small aint fsp_GetDayOfYearFromMonth(int64 _Year, aint _Month)
		{
			return fsp_GetDayOfYearFromMonth(_Month) + (fsp_IsLeapYear(_Year) && _Month > 1 ? 1 : 0);
		}

		inline_always constexpr static int64 fsp_GetSecondsFromYearConstexpr(int64 _Year)
		{
			DMibFastCheck(_Year >= NPrivate::CConst::mc_MinYear);
			DMibFastCheck(_Year <= NPrivate::CConst::mc_MaxYear);

			auto BasedYear = NPrivate::CConst::mc_YearOffset + _Year;
			return
				(BasedYear) * (NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay)	// Days in normal year
				- NPrivate::CConst::mc_YearOffsetSeconds
				+
				(
					((BasedYear-1) / 4) * NPrivate::CConst::mc_SecondsInDay		// Leap year every 4 years
					- ((BasedYear-1) / 100) * NPrivate::CConst::mc_SecondsInDay	// Not a leap year every 100 years
					+ ((BasedYear-1) / 400) * NPrivate::CConst::mc_SecondsInDay	// A leap year every 400 years despite 100 year rule
				)
			;
		}

		static int64 fsp_GetSecondsFromYear(int64 _Year)
		{
			DMibRequire(_Year >= NPrivate::CConst::mc_MinYear);
			DMibRequire(_Year <= NPrivate::CConst::mc_MaxYear);

			return fsp_GetSecondsFromYearConstexpr(_Year);
		}

		static inline_medium aint fsp_GetWeekDay(int64 _Year, aint _DayOfYear)
		{
			int64 Days = _Year * NPrivate::CConst::mc_DaysInMedianYear;
			Days += (_Year-1) / 4;
			Days -= (_Year-1) / 100;
			Days += (_Year-1) / 400;
			Days += _DayOfYear;

			Days = ((Days-1) % 7);
			if (Days < 0)
				Days += 7;
			return Days;
		}

		template <aint t_StopAtStage, bool t_bExtractMonth>
		void fp_ExtractDateTimeAD(CDateTime &_Dest, uint64 _Seconds) const
		{
			uint64 Seconds = _Seconds;

			_Dest.m_Year = (Seconds / NPrivate::CConst::mc_AverageSecondsInYear);

			for (mint i = 0; i < 2; ++i)
			{
				uint64 YearSub = _Dest.m_Year;

				if (YearSub > 0)
					--YearSub;

				Seconds -= uint64(_Dest.m_Year) * (NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay);	// Days in normal year
				Seconds -= (YearSub / 4) * NPrivate::CConst::mc_SecondsInDay;	// Leap year every 4 years
				Seconds += (YearSub / 100) * NPrivate::CConst::mc_SecondsInDay;	// Not a leap year every 100 years
				Seconds -= (YearSub / 400) * NPrivate::CConst::mc_SecondsInDay;	// A leap year every 400 years despite 100 year rule
				if (Seconds <= NPrivate::CConst::mc_SecondsInLeapYear)
					break;
				else if (Seconds <= NPrivate::CConst::mc_SecondsInLeapYear * 2)
				{
					Seconds = _Seconds;
					++_Dest.m_Year;
				}
				else
				{
					Seconds = _Seconds;
					--_Dest.m_Year;
				}
			}

			DMibFastCheck(Seconds <= NPrivate::CConst::mc_SecondsInLeapYear);

			bool bIsLeapYear = fsp_IsLeapYear(_Dest.m_Year);
			if (bIsLeapYear)
			{
				if (Seconds >= NPrivate::CConst::mc_DaysInLeapYear * NPrivate::CConst::mc_SecondsInDay)
				{
					_Dest.m_Year += 1;
					Seconds -= NPrivate::CConst::mc_DaysInLeapYear * NPrivate::CConst::mc_SecondsInDay;
					bIsLeapYear = false;
				}
			}
			else
			{
				if (Seconds >= NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay)
				{
					_Dest.m_Year += 1;
					Seconds -= NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay;
					if (!(_Dest.m_Year & 3))
						bIsLeapYear = fsp_IsLeapYear(_Dest.m_Year);
					else
						bIsLeapYear = false;
				}
			}

			_Dest.m_bIsLeapYear = bIsLeapYear;

			if constexpr (t_StopAtStage == 1)
				return;

			aint DayOfYear = Seconds / NPrivate::CConst::mc_SecondsInDay;

			_Dest.m_DayOfWeek = fsp_GetWeekDay(_Dest.m_Year, DayOfYear);
			_Dest.m_DayOfYear = DayOfYear;

			if constexpr (t_StopAtStage == 2)
				return;

			Seconds -= DayOfYear * NPrivate::CConst::mc_SecondsInDay;

			if constexpr (t_bExtractMonth)
			{
				aint PassedLeapDay = 0;
				aint DayOfYearMonth = DayOfYear;

				if (bIsLeapYear && DayOfYear >= 59)
				{
					--DayOfYearMonth;
					PassedLeapDay = 1;
				}

				_Dest.m_Month = 11;
				for (aint i = 1; i < 12; ++i)
				{
					if (DayOfYearMonth < fsp_GetDayOfYearFromMonth(i))
					{
						_Dest.m_Month = i - 1;
						break;
					}
				}

				++_Dest.m_Month;

				if constexpr (t_StopAtStage == 3)
					return;

				if (PassedLeapDay && _Dest.m_Month == 2)
					_Dest.m_DayOfMonth = DayOfYearMonth - fsp_GetDayOfYearFromMonth(_Dest.m_Month-1) + 2;
				else
					_Dest.m_DayOfMonth = DayOfYearMonth - fsp_GetDayOfYearFromMonth(_Dest.m_Month-1) + 1;

				if constexpr (t_StopAtStage == 4)
					return;
			}

			_Dest.m_Hour = Seconds / NPrivate::CConst::mc_SecondsInHour;

			if constexpr (t_StopAtStage == 5)
				return;
			Seconds -= _Dest.m_Hour * NPrivate::CConst::mc_SecondsInHour;

			_Dest.m_Minute = Seconds / NPrivate::CConst::mc_SecondsInMinute;
			if constexpr (t_StopAtStage == 6)
				return;
			Seconds -= _Dest.m_Minute * NPrivate::CConst::mc_SecondsInMinute;

			_Dest.m_Second = Seconds;
			if constexpr (t_StopAtStage == 7)
				return;

			_Dest.m_Fraction = m_pTime->f_GetFraction();
			_Dest.m_FractionInt = m_pTime->f_GetFractionInt();
		}

		template <aint t_StopAtStage, bool t_bExtractMonth>
		void fp_ExtractDateTimeBC(CDateTime &_Dest, uint64 _Seconds) const
		{
			auto FixupYear = g_OnScopeExit / [&]()
				{
					_Dest.m_Year -= NPrivate::CConst::mc_YearOffset;
				}
			;

			fp_ExtractDateTimeAD<t_StopAtStage, t_bExtractMonth>(_Dest, _Seconds + NPrivate::CConst::mc_YearOffsetSeconds);
		}

		template <aint t_StopAtStage, bool t_bExtractMonth>
		void fp_ExtractDateTime(CDateTime &_Dest) const
		{
			uint64 Seconds = m_pTime->f_GetSeconds();

			if (m_pTime->f_IsValid())
			{
				if (Seconds <= NPrivate::CConst::mc_YearTwoAdSeconds)
					fp_ExtractDateTimeBC<t_StopAtStage, t_bExtractMonth>(_Dest, Seconds);
				else
				{
					Seconds -= NPrivate::CConst::mc_YearZeroPlus1DaySeconds;
					fp_ExtractDateTimeAD<t_StopAtStage, t_bExtractMonth>(_Dest, Seconds);
				}
			}
			else
			{
				NMemory::fg_ObjectSet((int64 *)&_Dest, 0, sizeof(_Dest)/sizeof(int64));
				_Dest.m_Month = 1;
				_Dest.m_DayOfMonth = 1;
			}
		}

	public:
		CTimeConvert_ProlepticGreogrian(const NTime::CTime &_Time) : CTimeConvert_BabylonianCommon(_Time)
		{
		}

		CTimeConvert_ProlepticGreogrian &operator = (const NTime::CTime &_Time)
		{
				m_pTime = &_Time;

				return *this;
		}

		static inline_small aint fs_GetDaysInMonth(aint _Month)
		{
			return ms_DaysInMonth[_Month];
		}

		static inline_small aint fs_GetDaysInMonth(int64 _Year, aint _Month)
		{
			aint Day;
			if (fsp_IsLeapYear(_Year) && _Month == 1)
				Day = 1;
			else
				Day = 0;
			Day += fs_GetDaysInMonth(_Month);
			return Day;
		}

		void f_ExtractDateTime(CDateTime &_Destination) const
		{
			fp_ExtractDateTime<-1, true>(_Destination);
		}

		CDateTime f_ExtractDateTime() const
		{
			CDateTime Destination;
			fp_ExtractDateTime<-1, true>(Destination);
			return Destination;
		}

		template <aint t_StopAtStage>
		void f_ExtractDateTime(CDateTime &_Destination) const
		{
			fp_ExtractDateTime<t_StopAtStage, true>(_Destination);
		}

		template <aint t_StopAtStage>
		CDateTime f_ExtractDateTime() const
		{
			CDateTime Destination;
			fp_ExtractDateTime<t_StopAtStage, true>(Destination);
			return Destination;
		}

		int64 f_GetYear() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<1, true>(DateTime);
			return DateTime.m_Year;
		}

		aint f_GetMonth() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<3, true>(DateTime);
			return DateTime.m_Month;
		}

		aint f_GetDayOfYear() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<3, true>(DateTime);
			return DateTime.m_DayOfYear;
		}

		aint f_GetDayOfWeek() const
		{
			return ((m_pTime->f_GetSeconds() / NPrivate::CConst::mc_SecondsInDay) + 1) % 7;
		}

		aint f_GetDayOfMonth() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<4, true>(DateTime);
			return DateTime.m_DayOfMonth;
		}

		aint f_GetHour() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<5, true>(DateTime);
			return DateTime.m_Hour;
		}

		aint f_GetMinute() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<6, true>(DateTime);
			return DateTime.m_Minute;
		}

		aint f_GetSecond() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<7, true>(DateTime);
			return DateTime.m_Second;
		}

		fp64 f_GetFraction() const
		{
			return m_pTime->f_GetFraction();
		}

		static CTime fs_GetYearZero()
		{
			return CTime::fs_Create(NPrivate::CConst::mc_YearOneBcSeconds);
		}

		static CTime fs_CreateTime(int64 _Year, aint _Month = 1, aint _DayOfMonth = 1, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, fp64 _Fraction = 0)
		{
			--_Month;
			--_DayOfMonth;
			DMibSafeCheck(_Month >= 0 && _Month <= 11, "Month range check error");
			DMibSafeCheck(_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month), "Day of month range check error");
			DMibSafeCheck(_Hour >= 0 && _Hour <= 23, "Hour range check error");
			DMibSafeCheck(_Minute >= 0 && _Minute <= 59, "Minute range check error");
			DMibSafeCheck(_Second >= 0 && _Second <= 59, "Second range check error");
			DMibSafeCheck(_Fraction >= 0.0 && _Fraction < 1.0, "Fraction range check error");
			uint64 Seconds = fsp_GetSecondsFromYear(_Year);
			Seconds += fsp_GetDayOfYearFromMonth(_Year, _Month) * NPrivate::CConst::mc_SecondsInDay;
			Seconds += _DayOfMonth * NPrivate::CConst::mc_SecondsInDay;
			CTime NewTime{Seconds, 0};
			fsp_AddTime(NewTime, _Hour, _Minute, _Second, _Fraction);
			return NewTime;
		}

		static CTime fs_CreateTimeIntFrac(int64 _Year, aint _Month = 1, aint _DayOfMonth = 1, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _FractionInt = 0)
		{
			--_Month;
			--_DayOfMonth;
			DMibSafeCheck(_Month >= 0 && _Month <= 11, "Month range check error");
			DMibSafeCheck(_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month), "Day of month range check error");
			DMibSafeCheck(_Hour >= 0 && _Hour <= 23, "Hour range check error");
			DMibSafeCheck(_Minute >= 0 && _Minute <= 59, "Minute range check error");
			DMibSafeCheck(_Second >= 0 && _Second <= 59, "Second range check error");
			DMibSafeCheck(_FractionInt >= 0 && _FractionInt < NPrivate::CConst::mc_FractionDividend, "Fraction range check error");
			uint64 Seconds = fsp_GetSecondsFromYear(_Year);
			Seconds += fsp_GetDayOfYearFromMonth(_Year, _Month) * NPrivate::CConst::mc_SecondsInDay;
			Seconds += _DayOfMonth * NPrivate::CConst::mc_SecondsInDay;
			CTime NewTime{Seconds, 0};
			fsp_AddTimeIntFrac(NewTime, _Hour, _Minute, _Second, _FractionInt);
			return NewTime;
		}

		constexpr static CTime fs_CreateTimeConstExpr(int64 _Year, aint _Month = 1, aint _DayOfMonth = 1, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _Fraction = 0)
		{
			return fsp_AddTimeIntFracConstexr
				(
					CTime
					{
						uint64
						(
							fsp_GetSecondsFromYearConstexpr(_Year)
							+ fsp_GetDayOfYearFromMonth(_Year, _Month - 1) * NPrivate::CConst::mc_SecondsInDay
							+ (_DayOfMonth - 1) * NPrivate::CConst::mc_SecondsInDay
						 )
						, 0
					}
					, _Hour
					, _Minute
					, _Second
					, _Fraction
				)
			;
		}


		static CTime fs_CreateTime(const CDateTime &_DateTime)
		{
			if (_DateTime.m_FractionInt != NPrivate::CConst::mc_InvalidFractionInt)
				return fs_CreateTimeIntFrac(_DateTime.m_Year, _DateTime.m_Month, _DateTime.m_DayOfMonth, _DateTime.m_Hour, _DateTime.m_Minute, _DateTime.m_Second, _DateTime.m_FractionInt);
			else
				return fs_CreateTime(_DateTime.m_Year, _DateTime.m_Month, _DateTime.m_DayOfMonth, _DateTime.m_Hour, _DateTime.m_Minute, _DateTime.m_Second, _DateTime.m_Fraction);
		}

		static void fs_RoundTimeToSecondDown(NTime::CTime &_Time)
		{
			_Time.f_SetFractionInt(0);
		}

		static void fs_RoundTimeToMinuteDown(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			_Time.f_SetSeconds(Seconds - (Seconds % NPrivate::CConst::mc_SecondsInMinute));
		}

		static void fs_RoundTimeToHourDown(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			_Time.f_SetSeconds(Seconds - (Seconds % NPrivate::CConst::mc_SecondsInHour));
		}

		static void fs_RoundTimeToDayDown(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			_Time.f_SetSeconds(Seconds - (Seconds % NPrivate::CConst::mc_SecondsInDay));
		}

		static void fs_RoundTimeToSecondUp(NTime::CTime &_Time)
		{
			if (_Time.f_GetFractionInt())
				_Time.f_SetSeconds(_Time.f_GetSeconds() + 1);
		}

		static void fs_RoundTimeToMinuteUp(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInMinute;
			Seconds = Seconds - ModSeconds;
			if (ModSeconds || _Time.f_GetFractionInt())
				Seconds += NPrivate::CConst::mc_SecondsInMinute;
			_Time.f_SetSeconds(Seconds);
		}

		static void fs_RoundTimeToHourUp(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInHour;
			Seconds = Seconds - ModSeconds;
			if (ModSeconds || _Time.f_GetFractionInt())
				Seconds += NPrivate::CConst::mc_SecondsInHour;
			_Time.f_SetSeconds(Seconds);
		}

		static void fs_RoundTimeToDayUp(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInDay;
			Seconds = Seconds - ModSeconds;
			if (ModSeconds || _Time.f_GetFractionInt())
				Seconds += NPrivate::CConst::mc_SecondsInDay;
			_Time.f_SetSeconds(Seconds);
		}

		static void fs_RoundTimeToSecondClosest(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			if (_Time.f_GetFractionInt() >= NPrivate::CConst::mc_FractionDividendHalf)
			{
				++Seconds;
			}
			_Time.f_SetSeconds(Seconds);
		}

		static void fs_RoundTimeToMinuteClosest(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInMinute;
			Seconds = Seconds - ModSeconds;
			if (ModSeconds >= NPrivate::CConst::mc_SecondsInMinuteHalf)
				Seconds += NPrivate::CConst::mc_SecondsInMinute;
			_Time.f_SetSeconds(Seconds);
		}

		static void fs_RoundTimeToHourClosest(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInHour;
			Seconds = Seconds - ModSeconds;
			if (ModSeconds >= NPrivate::CConst::mc_SecondsInHourHalf)
				Seconds += NPrivate::CConst::mc_SecondsInHour;
			_Time.f_SetSeconds(Seconds);

		}

		static void fs_RoundTimeToDayClosest(NTime::CTime &_Time)
		{
			uint64 Seconds = _Time.f_GetSeconds();
			uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInDay;
			Seconds = Seconds - ModSeconds;
			if (ModSeconds >= NPrivate::CConst::mc_SecondsInDayHalf)
				Seconds += NPrivate::CConst::mc_SecondsInDay;
			_Time.f_SetSeconds(Seconds);

		}
	};

	class CTimeConvert_ISOWeek : public CTimeConvert_BabylonianCommon
	{
	public:

		class CDateTime
		{
		public:
			int64 m_Year;
			aint m_Week;
			aint m_DayOfYear;
			aint m_DayOfWeek;
			aint m_Hour;
			aint m_Minute;
			aint m_Second;
			uint64 m_FractionInt = NPrivate::CConst::mc_InvalidFractionInt;
			fp64 m_Fraction;
		};

	private:

		template <aint t_StopAtStage>
		void fp_ExtractDateTime(CDateTime &_Dest) const
		{
			CTimeConvert_ProlepticGreogrian::CDateTime DateTime;
			CTimeConvert_ProlepticGreogrian Convert(*m_pTime);

			if constexpr (t_StopAtStage < 0)
				Convert.fp_ExtractDateTime<-1, false>(DateTime);
			else
			{
				if constexpr (t_StopAtStage > 1)
					Convert.fp_ExtractDateTime<4+t_StopAtStage, false>(DateTime);
				else
					Convert.fp_ExtractDateTime<2, false>(DateTime);
			}

			aint DayOfWeekAtStartOfYear = (DateTime.m_DayOfWeek - (DateTime.m_DayOfYear % 7)) % 7;
			if (DayOfWeekAtStartOfYear < 0)
				DayOfWeekAtStartOfYear += 7;

			_Dest.m_Year = DateTime.m_Year;
			_Dest.m_DayOfYear = DateTime.m_DayOfYear;
			_Dest.m_DayOfWeek = DateTime.m_DayOfWeek;
			_Dest.m_Hour = DateTime.m_Hour;
			_Dest.m_Minute = DateTime.m_Minute;
			_Dest.m_Second = DateTime.m_Second;
			_Dest.m_Fraction = DateTime.m_Fraction;
			_Dest.m_FractionInt = DateTime.m_FractionInt;

			if ((DateTime.m_DayOfYear + 1) <= (7 - DayOfWeekAtStartOfYear) && DayOfWeekAtStartOfYear > 3)
			{
				--_Dest.m_Year;
				if (DayOfWeekAtStartOfYear == 4 || (DayOfWeekAtStartOfYear == 5 && CTimeConvert_ProlepticGreogrian::fsp_IsLeapYear(_Dest.m_Year)))
					_Dest.m_Week = 53;
				else
					_Dest.m_Week = 52;
			}
			else
			{
				int DaysInYear = DateTime.m_bIsLeapYear + NPrivate::CConst::mc_DaysInMedianYear;
				if ((DaysInYear - _Dest.m_DayOfYear) < (4 - DateTime.m_DayOfWeek))
				{
					++_Dest.m_Year;
					_Dest.m_Week = 1;
				}
				else
				{
					_Dest.m_Week = ((_Dest.m_DayOfYear + (8 - _Dest.m_DayOfWeek) + DayOfWeekAtStartOfYear) / 7);
					if (DayOfWeekAtStartOfYear > 3)
						--_Dest.m_Week;
				}
			}

		}

	public:

		CTimeConvert_ISOWeek(const CTime &_Time) :
			CTimeConvert_BabylonianCommon(_Time)
		{
		}

		void f_ExtractDateTime(CDateTime &_Destination) const
		{
			fp_ExtractDateTime<-1>(_Destination);
		}

		int64 f_GetYear()
		{
			CDateTime DateTime;
			fp_ExtractDateTime<1>(DateTime);

			return DateTime.m_Year;
		}

		int64 f_GetWeek()
		{
			CDateTime DateTime;
			fp_ExtractDateTime<1>(DateTime);

			return DateTime.m_Week;
		}

		aint f_GetDayOfWeek()
		{
			return ((m_pTime->f_GetSeconds() / NPrivate::CConst::mc_SecondsInDay) + 1) % 7;
		}

		aint f_GetDayOfYear()
		{
			CDateTime DateTime;
			fp_ExtractDateTime<1>(DateTime);

			return DateTime.m_DayOfYear;
		}

		aint f_GetHour() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<2>(DateTime);
			return DateTime.m_Hour;
		}

		aint f_GetMinute() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<3>(DateTime);
			return DateTime.m_Minute;
		}

		aint f_GetSecond() const
		{
			CDateTime DateTime;
			fp_ExtractDateTime<4>(DateTime);
			return DateTime.m_Second;
		}

		fp64 f_GetFraction() const
		{
			return m_pTime->f_GetFraction();
		}

		static CTime fs_CreateTime(int64 _Year, aint _Week = 1, aint _DayOfWeek = 0, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, fp64 _Fraction = 0) // This takes ISO dates
		{
			--_Week;
			DMibSafeCheck(_Week >= 0 && _Week <= 52, "Week range check error");
			DMibSafeCheck(_DayOfWeek >= 0 && _DayOfWeek <= 6, "Day of week range check error");
			DMibSafeCheck(_Hour >= 0 && _Hour <= 23, "Hour range check error");
			DMibSafeCheck(_Minute >= 0 && _Minute <= 59, "Minute range check error");
			DMibSafeCheck(_Second >= 0 && _Second <= 59, "Second range check error");
			DMibSafeCheck(_Fraction >= 0.0 && _Fraction < 1.0, "Fraction range check error");
			CTime NewTime = CTimeConvert_ProlepticGreogrian::fs_CreateTime(_Year);

			CTimeConvert_ProlepticGreogrian::CDateTime DateTime;
			CTimeConvert_ProlepticGreogrian Convert(NewTime);
			Convert.fp_ExtractDateTime<-1, false>(DateTime);

			aint DayOfWeekAtStartOfYear = (DateTime.m_DayOfWeek - (DateTime.m_DayOfYear % 7)) % 7;
			if (DayOfWeekAtStartOfYear < 0)
				DayOfWeekAtStartOfYear += 7;

			aint DaysToAdd = _Week * 7 + _DayOfWeek;

			if (DayOfWeekAtStartOfYear <= 3)
				DaysToAdd -= DayOfWeekAtStartOfYear;
			else
				DaysToAdd += 7 - DayOfWeekAtStartOfYear;

			NewTime += CTimeSpanConvert_BabylonianCommon::fs_CreateDaySpan(DaysToAdd);
			fsp_AddTime(NewTime, _Hour, _Minute, _Second, _Fraction);

			return NewTime;
		}
	};

	// Default fo Proleptic Gregorian time
	using CTimeConvert = CTimeConvert_ProlepticGreogrian;
	using CTimeSpanConvert = CTimeSpanConvert_BabylonianCommon;

	NMib::NStr::CStr fg_GetFullTimeStr(CTime const &_Time);
	bool fg_ParseFullTimeStr(CTime &_Time, NMib::NStr::CStr const& _Str);

	// Parses a flexible date/time string in format: YYYY-MM-DD [HH:MM[:SS[.FFF]]]
	// Returns CTime on success, or error on failure
	NMib::NConcurrency::TCAsyncResult<CTime> fg_TryParseDateTimeStr(NMib::NStr::CStr const &_Str);

	NMib::NStr::CStr fg_GetAscTimeStr(CTime const &_Time); // Same format as std. C lib's asctime. NOT LOCALISED (deliberate)
	int32 fg_GetAscMonthNumber(NStr::CStr const &_Month); // Same format as std. C lib's asctime. NOT LOCALISED (deliberate)
	NMib::NStr::CStr fg_GetISO8601TimeStr(CTime const &_Time);

	NMib::NStr::CStr fg_SecondsDurationToHumanReadable(fp64 const &_Seconds);

	class CStopWatch
	{
	protected:
		CTime m_StartedAt;
		CTime m_Elapsed;

	public:
		CStopWatch()
		{
		}

		CStopWatch(bool _bStart)
		{
			if (_bStart)
				f_Start();
		}

		void f_Start()
		{
			m_StartedAt = CTime::fs_NowUTC();
		}

		CTimeSpan f_Mark()
		{
			return CTime::fs_NowUTC() - m_StartedAt;
		}

		CTimeSpan f_Stop()
		{
			CTimeSpan Elapsed = CTime::fs_NowUTC() - m_StartedAt;
			m_StartedAt = CTime();
			return Elapsed;
		}

		CTimeSpan f_StopStart()
		{
			CTime Now = CTime::fs_NowUTC();
			CTimeSpan Elapsed = Now - m_StartedAt;
			m_StartedAt = Now;
			return Elapsed;
		}
	};

	class CTimeout
	{
	protected:
		CTimeSpan m_Timeout;
		CTime m_StartedAt;

	public:
		CTimeout(fp64 _nSeconds)
			: m_Timeout(CTimeSpanConvert::fs_CreateSpanFromSeconds(_nSeconds))
		{
			f_Start();
		}

		CTimeout(fp64 _nSeconds, bool _bStart)
			: m_Timeout(CTimeSpanConvert::fs_CreateSpanFromSeconds(_nSeconds))
		{
			f_Start();
		}

		void f_Start()
		{
			m_StartedAt = CTime::fs_NowUTC();
		}

		bool f_TimedOut() const
		{
			CTimeSpan Elapsed = CTime::fs_NowUTC() - m_StartedAt;
			return Elapsed >= m_Timeout;
		}

		bool operator!() const
		{
			return !f_TimedOut();
		}
	};

	template
	<
		typename tf_COption
		, typename tf_CArgs
	>
	bool CTime::f_FormatParseOption
		(
			CFormatOptions &_Options
			, tf_COption &_Option
			, tf_CArgs &_Args
		) const
	{
		switch (_Option.m_FormatTypes.m_Format1)
		{
		case 'T':
			{
				switch (_Option.m_FormatTypes.m_Format2)
				{
				case 'F':
					_Options.m_bFullPrecision = true;
					return true;
				case 'B':
					_Options.m_bBcSuffix = true;
					return true;
				case 'C':
					_Options.m_Components = _Option.f_GetData_aint_NotSigned(_Args.m_Formatter, 0);
					return true;
				case 'D':
					_Options.m_bDateOnly = true;
					return true;
				case 'S':
					switch (*_Option.m_pDataStart)
					{
					case 'T':
					case 't':
						_Options.m_TimeSeparator = _Option.m_pDataStart[1];
						return true;
					case 'D':
					case 'd':
						_Options.m_DateSeparator = _Option.m_pDataStart[1];
						return true;
					case 'S':
					case 's':
						_Options.m_SecondSeparator = _Option.m_pDataStart[1];
						return true;
					case 'B':
					case 'b':
						_Options.m_DateTimeSeparator = _Option.m_pDataStart[1];
						return true;
					}
				}
			}
			break;
		}
		return false;
	}

	template <typename tf_CStr, typename tf_COptions>
	void CTime::f_Format(tf_CStr &_FormatInto, tf_COptions const &_Options) const
	{
		if (!f_IsValid())
		{
			_FormatInto += "INVALID";
			return;
		}

		using CChar = typename tf_CStr::CChar;
		NTime::CTimeConvert::CDateTime DateTime;
		NTime::CTimeConvert(*this).f_ExtractDateTime(DateTime);
		auto CorrectedYear = DateTime.m_Year;
		ch8 const *pYearSuffix = "";
		if (_Options.m_LocalOptions.m_bBcSuffix && CorrectedYear <= 0)
		{
			CorrectedYear = 1 - CorrectedYear;
			pYearSuffix = "_BC";
		}

		if (_Options.m_LocalOptions.m_bDateOnly)
		{
			CChar DateSeparator[] = {static_cast<CChar>(_Options.m_LocalOptions.m_DateSeparator), 0};
			_FormatInto += typename tf_CStr::CFormat("{}{}{}{sj2,sf0}{}{sj2,sf0}")
				<< CorrectedYear
				<< pYearSuffix
				<< DateSeparator
				<< DateTime.m_Month
				<< DateSeparator
				<< DateTime.m_DayOfMonth
			;
		}
		else
		{
			fp64 Scale;
			int64 ScaleInt;
			int32 Decimals;

			if (_Options.m_LocalOptions.m_bFullPrecision)
			{
				Scale = 1000000000000000.0;
				ScaleInt = constant_uint64(1000000000000000);
				Decimals = 15;
			}
			else
			{
				Scale = 1000.0;
				ScaleInt = 1000;
				Decimals = 3;
			}

			int64 Fraction = (DateTime.m_Fraction*Scale).f_ToIntRound();
			if (Fraction >= ScaleInt)
				Fraction = ScaleInt - 1;

			CChar DateSeparator[] = {static_cast<CChar>(_Options.m_LocalOptions.m_DateSeparator), 0};
			CChar TimeSeparator[] = {static_cast<CChar>(_Options.m_LocalOptions.m_TimeSeparator), 0};
			CChar SecondSeparator[] = {static_cast<CChar>(_Options.m_LocalOptions.m_SecondSeparator), 0};
			CChar DateTimeSeparator[] = {static_cast<CChar>(_Options.m_LocalOptions.m_DateTimeSeparator), 0};

			ch8 const *pFormatStr;
			switch (_Options.m_LocalOptions.m_Components)
			{
			default:
			case 0:
				pFormatStr = "{}{}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}{}{sj*,sf0}";
				break;
			case 1:
				pFormatStr = "{}{}";
				break;
			case 2:
				pFormatStr = "{}{}{}{sj2,sf0}";
				break;
			case 3:
				pFormatStr = "{}{}{}{sj2,sf0}{}{sj2,sf0}";
				break;
			case 4:
				pFormatStr = "{}{}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}";
				break;
			case 5:
				pFormatStr = "{}{}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}";
				break;
			case 6:
				pFormatStr = "{}{}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}{}{sj2,sf0}";
				break;
			}

			_FormatInto += typename tf_CStr::CFormat(pFormatStr)
				<< CorrectedYear
				<< pYearSuffix
				<< DateSeparator
				<< DateTime.m_Month
				<< DateSeparator
				<< DateTime.m_DayOfMonth
				<< DateTimeSeparator
				<< DateTime.m_Hour
				<< TimeSeparator
				<< DateTime.m_Minute
				<< TimeSeparator
				<< DateTime.m_Second
				<< SecondSeparator
				<< Fraction
				<< Decimals
			;
		}
	}
}

namespace NMib
{
	template <typename tf_CLeft, typename tf_CRight>
	NTime::CTime fg_Max(tf_CLeft &&_Left, tf_CRight &&_Right)
		requires (NTime::cIsTime<tf_CLeft> && NTime::cIsTime<tf_CRight>)
	{
		if (_Left.f_IsValid())
		{
			if (_Right.f_IsValid())
			{
				if (_Left < _Right)
					return _Right;
			}
			return _Left;
		}
		return _Right;
	}
}

namespace NMib::NStream
{
	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NTime::CTime>
	{
	public:
		static constexpr void fs_Feed(t_CStream &_Stream, NTime::CTime const &_Data)
		{
			_Stream << _Data.f_GetSeconds();
			_Stream << _Data.f_GetFractionInt();
		}

		static constexpr void fs_Consume(t_CStream &_Stream, NTime::CTime &_Data)
		{
			uint64 Temp;
			_Stream >> Temp;
			_Data.f_SetSecondsNoFraction(Temp);
			_Stream >> Temp;
			_Data.f_SetFractionInt(Temp);
		}
	};

	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NTime::CTimeSpan>
	{
	public:
		static constexpr void fs_Feed(t_CStream &_Stream, NTime::CTimeSpan const &_Data)
		{
			_Stream << _Data.f_GetSeconds();
			_Stream << _Data.f_GetFractionInt();
		}

		static constexpr void fs_Consume(t_CStream &_Stream, NTime::CTimeSpan &_Data)
		{
			uint64 Temp;
			_Stream >> Temp;
			_Data.f_SetSecondsNoFraction(Temp);
			_Stream >> Temp;
			_Data.f_SetFractionInt(Temp);
		}
	};

}

constexpr inline pfp64 operator ""_ns(long double _Value)
{
	return _Value * 0.000'000'001;
}

constexpr inline pfp64 operator ""_µs(long double _Value)
{
	return _Value * 0.000'001;
}

constexpr inline pfp64 operator ""_ms(long double _Value)
{
	return _Value * 0.001;
}

constexpr inline pfp64 operator ""_seconds(long double _Value)
{
	return _Value;
}

constexpr inline pfp64 operator ""_minutes(long double _Value)
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInMinuteFp;
}

constexpr inline pfp64 operator ""_hours(long double _Value)
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInHourFp;
}

constexpr inline pfp64 operator ""_days(long double _Value)
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInDayFp;
}

constexpr inline pfp64 operator ""_weeks(long double _Value)
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInWeekFp;
}

constexpr inline pfp64 operator ""_ns(unsigned long long _Value)
{
	return pfp64(_Value) * 0.000'000'001;
}

constexpr inline pfp64 operator ""_µs(unsigned long long _Value)
{
	return pfp64(_Value) * 0.000'001;
}

constexpr inline pfp64 operator ""_ms(unsigned long long _Value)
{
	return pfp64(_Value) * 0.001;
}

constexpr inline pfp64 operator ""_seconds(unsigned long long _Value)
{
	return pfp64(_Value);
}

constexpr inline pfp64 operator ""_minutes(unsigned long long _Value)
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInMinuteFp;
}

constexpr inline pfp64 operator ""_hours(unsigned long long _Value)
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInHourFp;
}

constexpr inline pfp64 operator ""_days(unsigned long long _Value)
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInDayFp;
}

constexpr inline pfp64 operator ""_weeks(unsigned long long _Value)
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInWeekFp;
}
