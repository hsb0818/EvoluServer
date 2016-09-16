#include "../pch.h"
#include "Object.h"

LONG CObject::AddRef()
{
	if (m_count > 0L)
	{
		++m_count;
	}
	else
	{
		exit(0);
	}

	return m_count;
}

LONG CObject::AddRef( LONG value )
{
	if (m_count > 0L)
	{
		m_count += value;
	}
	else
	{
		exit(0);
	}

	return m_count;
}

LONG CObject::Release()
{
	LONG count = --m_count;

	if (count == 0L)
	{
		OnObjectDestroy();
		delete this;
	}
	else if (count < 0L)
	{
		exit(0);
	}

	return count;
}