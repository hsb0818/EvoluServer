#include "../pch.h"
#include "Counter.h"

void Counter::operator = (LONG Value)
{
	while (true)
	{
		ULONG temp = m_count;
		if ( InterlockedCompareExchange((ULONG *)&m_count, Value, temp) == temp )
		{
			break;
		}
		else
		{
			// Yield cpu
			YieldProcessor();
		}
	}
}