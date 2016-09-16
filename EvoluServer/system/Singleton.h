#pragma once

#include "Object.h"

template <typename TYPE>
class Singleton : public CObject
{
public:
	static TYPE& GetInstance()
	{
		if ( s_pInstance )
		{
			return *s_pInstance;
		}

		// SYNC
		static volatile LONG tvalue = 0L;
		while ( true )
		{
			if ( InterlockedCompareExchange( &tvalue, 1L, 0L ) == 0L )
			{
				break;
			}
			YieldProcessor();
		}

		if ( !s_pInstance )
		{
			initialize();
		}

		tvalue = 0L;

		return *s_pInstance;
	}
	
protected:
	virtual ~Singleton() {}
	
	static void initialize()
	{
		TYPE * pInst = new TYPE();
		s_pInstance = pInst;

		::atexit(&finalize);
	}

	static void finalize()
	{
		if ( s_pInstance )
		{
			//delete s_pInstance;
			//s_pInstance = 0;
			s_pInstance->Release();
		}
	}
	
protected:
	static TYPE * s_pInstance;
};

template <typename TYPE>
TYPE * Singleton<TYPE>::s_pInstance = 0;