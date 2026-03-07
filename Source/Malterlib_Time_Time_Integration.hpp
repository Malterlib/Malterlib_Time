// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

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
