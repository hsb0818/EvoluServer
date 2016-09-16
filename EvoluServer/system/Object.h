#pragma once

#include "Counter.h"

class CObject
{
public:
	CObject()
	:	m_count(1U)
	{}

	explicit CObject(LONG count)
	:	m_count(count)
	{}

	virtual ~CObject()
	{}

	LONG AddRef();
	LONG AddRef(LONG value); 
	LONG Release();

	virtual void OnObjectDestroy() {}
	
private:
	Counter		m_count;
};