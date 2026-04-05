// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	template
	<
		typename tf_COption
		, typename tf_CArgs
	>
	constexpr bool CTime::f_FormatParseOption
		(
			CFormatOptions &_Options
			, tf_COption &_Option
			, tf_CArgs &_Args
		) const noexcept
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
	constexpr void CTime::f_Format(tf_CStr &_FormatInto, tf_COptions const &_Options) const noexcept
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
