#ifndef _THREAD_H_
#define _THREAD_H_

#include <Windows.h>
#include "Lock.h"

// This Thread class manage Thread 'Delayed Type'
// 'Delayed Type' thread is thread-safe.
// and each threads to be possible the 'cross control'.
// MRMW Managing.
//** If someone use loop in thread, it has to use inturrupt() function.

using namespace SOC_Thread;

class Thread
{	
public:	
	enum STATE
	{
		CREATE = 0,
		DEAD,
		READY,
		SUSPEND,
		RUN,
	};

public:
	Thread(int id);
	virtual ~Thread();

public:
	void Resume(); // not delayed
	void Suspend(); // delayed
	void Join();
	BOOL Yielding();
	BOOL Interrupt();
	void Close();

	static Thread* GetCurrent() { return t_pInstance; }
	static int GetCurrentThreadID();
public:
	HANDLE GetHandle() { return m_handle; }
	int GetID() { return m_id; }
	STATE GetState();
	bool IsAlive()
	{
		switch(m_state)
		{
		case STATE::DEAD:
			return false;
		case STATE::CREATE:
		case STATE::READY:
		case STATE::SUSPEND:
		case STATE::RUN:
			return true;
		}

		return false;
	}

private:
	void _Close();
	void _Suspend(); // interrupt()에서 호출되는 함수이다.
	void SetState(STATE state);
	void Ready();
	void Create();
	static unsigned __stdcall handle(void* pData);

protected:
	virtual void callback(Thread* pCurThread) = 0;

private:
	Lock::CVCriticalSectionLock m_suspend_lock;

	HANDLE	m_handle;
	long	m_id;
	STATE	m_state;
	bool	m_signal_suspend;
	bool	m_signal_close;
	// thread-local variable
	static __declspec(thread) Thread* t_pInstance;
};

#endif