#include "../pch.h"
#include "Lock.h"

namespace SOC_Thread {
	namespace Lock {

		CriticalSectionLock::CriticalSectionLock(unsigned int spinCount)
		{
#if defined(_WIN32) || defined(_WIN64)
#if (_WIN32_WINNT >= 0x0600) // version >= vista.
			InitializeCriticalSectionEx(&m_key, spinCount, CRITICAL_SECTION_NO_DEBUG_INFO);
#else
			InitializeCriticalSectionAndSpinCount(&m_key, spinCount);
#endif
#else
			pthread_mutex_init(&m_key, nullptr);
#endif
		}

		CriticalSectionLock::~CriticalSectionLock()
		{
#if defined(_WIN32) || defined(_WIN64)
			DeleteCriticalSection(&m_key);
#else
			pthread_mutex_destroy(&m_key);
#endif
		}

		void CriticalSectionLock::Enter(const char* pFileName, int line)
		{
#if defined(_WIN32) || defined(_WIN64)
			EnterCriticalSection(&m_key);
#else
			pthread_mutex_lock(&m_key);
#endif
		}

		void CriticalSectionLock::Leave()
		{
#if defined(_WIN32) || defined(_WIN64)
			LeaveCriticalSection(&m_key);
#else
			pthread_mutex_unlock(&m_key);
#endif
		}

		CVCriticalSectionLock::CVCriticalSectionLock(unsigned int spinCount) 
			: CriticalSectionLock(spinCount)
		{
#if defined(_WIN32) || defined(_WIN64)
			InitializeConditionVariable(&m_cv);
#else
			//need mac os ver
#endif
		}

		CVCriticalSectionLock::~CVCriticalSectionLock()
		{
		}

		void CVCriticalSectionLock::WakeCV()
		{
#if defined(_WIN32) || defined(_WIN64)
			WakeConditionVariable(&m_cv); 
#else

#endif			
		}

		BOOL CVCriticalSectionLock::SleepCV(const char* pFileName, int line)
		{
#if defined(_WIN32) || defined(_WIN64)
			this->Enter(pFileName, line);
			// lock_suspend to be release in this SleepConditionVariableCS();
			BOOL ret = SleepConditionVariableCS(&m_cv, this->Getlock(), INFINITE);
			this->Leave();
			return ret;
#else
			//need mac os ver
#endif
		}

		SpinLock::SpinLock(unsigned int spincount)
#if defined(_WIN32) || defined(_WIN64)        
#else
			: m_key(OS_SPINLOCK_INIT)
#endif
		{
		}

		SpinLock::~SpinLock()
		{
		}

		void SpinLock::Enter(const char* pFileName, int line)
		{
#if defined(_WIN32) || defined(_WIN64)
			// windows
#else
			OSSpinLockLock(&m_key);
#endif
		}

		void SpinLock::Leave()
		{
#if defined(_WIN32) || defined(_WIN64)
			// windows
#else
			// if lock possible.. exactly this state is not locked.
			if (OSSpinLockTry(&m_key))
				return;

			OSSpinLockUnlock(&m_key);
#endif
		}
	}
}