//
//  Lock.h
//  Created by SungBin_Hong on 13. 10. 13..
//  Copyright (c) 2013 years Teraphonia. All rights reserved.
//

#pragma once

namespace SOC_Thread {
namespace Lock {

	// 'I' mean is interface
	class ILock
	{
	public:
		ILock() {}
		virtual ~ILock() {}
		
		virtual void Enter(const char* pFileName, int line) = 0;
		virtual void Leave() = 0;
		
		virtual void ReadLock(const char* pFileName, int line)
		{
			Enter(pFileName, line);
		}
		virtual void ReadUnlock()
		{
			Leave();
		}
		virtual void WriteLock(const char* pFileName, int line)
		{
			Enter(pFileName, line);			
		}
		virtual void WriteUnlock()
		{
			Leave();
		}
	};

	class CriticalSectionLock : public ILock
	{
	public:
		CriticalSectionLock(unsigned int spinCount = 0);
		virtual ~CriticalSectionLock();

		virtual void Enter(const char* pFileName, int line);
		virtual void Leave();

	protected:
		CRITICAL_SECTION* Getlock() { return &m_key; }
	private:
#if defined(_WIN32) || defined(_WIN64)
		CRITICAL_SECTION m_key;
#else
		pthread_mutex_t m_key;
#endif
	};

	class CVCriticalSectionLock : public CriticalSectionLock
	{
	public:
		CVCriticalSectionLock(unsigned int spinCount = 0);
		virtual ~CVCriticalSectionLock();

		void WakeCV();
		BOOL SleepCV(const char* pFileName, int line);

	private:
#if defined(_WIN32) || defined(_WIN64)
		CONDITION_VARIABLE m_cv;
#else
		pthread_mutex_t m_key;
#endif
	};

	class SpinLock
	{
	public:
		SpinLock(unsigned int spincount = 3000);
		virtual ~SpinLock();

		virtual void Enter(const char* pFileName, int line);
		virtual void Leave();

	private:
#if defined(_WIN32) || defined(_WIN64)
		// m_key;

#else
		OSSpinLock m_key;
#endif
	};
        
	namespace Helper {
		template<typename T>
		class ScopedLock
		{
		private:
			ScopedLock& operator=(const ScopedLock &);
		public:
			ScopedLock(T& lock, const char* pFileName = nullptr, int line = 0) : m_lock(lock)
			{
				m_lock.Enter(pFileName, line);
			}
			~ScopedLock()
			{
				m_lock.Leave();
			}

		private:
			T& m_lock;
		};
	}
}
}

#define SCOPE_LOCK(lock)  SOC_Thread::Lock::Helper::ScopedLock<SOC_Thread::Lock::CriticalSectionLock> __lock##__LINE__##(lock, __FILE__, __LINE__)
#define SCOPE_CVLOCK(lock)  SOC_Thread::Lock::Helper::ScopedLock<SOC_Thread::Lock:CVCriticalSectionLock> __lock##__LINE__##(lock, __FILE__, __LINE__)
#define SCOPE_SPINLOCK(lock)  SOC_Thread::Lock::Helper::ScopedLock<SOC_Thread::Lock::SpinLock> __lock##__LINE__##(lock, __FILE__, __LINE__)

#define SCOPE_SLEEPCV(cvlock) { cvlock.SleepCV(__FILE__, __LINE__); } 
#define SCOPE_WAKECV(cvlock) { cvlock.WakeCV(); }

