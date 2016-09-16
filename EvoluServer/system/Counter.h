#pragma once

#include <Windows.h>

class Counter
{
public:
	Counter()
	:	m_count(0)
	{}

	Counter(LONG value)
	:	m_count(value)
	{}

	virtual ~Counter()
	{}

	LONG		Get()
	{	return m_count;	}

public:
	void operator = (LONG Value);

	LONG operator ++ ()	// prefix ++
	{	return InterlockedIncrement((ULONG *)&m_count); }
	
	LONG operator ++ (int)	// postfix ++
	{	return InterlockedIncrement((ULONG *)&m_count) - 1;	}

	LONG operator -- ()
	{	return InterlockedDecrement((ULONG *)&m_count);	}
	
	LONG operator -- (int)
	{	return InterlockedDecrement((ULONG *)&m_count) + 1;	}

	void operator += (LONG value)
	{	InterlockedExchangeAdd((ULONG *)&m_count, value);	}

	void operator -= (LONG value)
	{	InterlockedExchangeSubtract((ULONG *)&m_count, value);	}

	bool operator != (LONG value) const
	{	return (m_count != value);	}

	operator LONG () const
	{	return m_count;	}

private:
	LONG m_count;
};
