#include "../pch.h"
#include "Thread.h"

Thread * Thread::t_pInstance = nullptr;
 
Thread::Thread(int id)
: m_handle(0)
, m_id(id)
, m_state(STATE::CREATE)
, m_signal_suspend(false)
, m_signal_close(false)
{
	Create();	
}

Thread::~Thread()
{
	_Close();
}

void Thread::SetState(STATE state)
{
	m_state = state;
}

// 상태 변환 도중에 다른 스레드에서 이 함수가 호출되어도 괜찮도록 설계했다.
// Suspend 상태일 땐 건드리지 않는 처리를 할 터이다.
Thread::STATE Thread::GetState()
{
	return m_state;
}

// Suspend되었던 Thread를 깨운 후 Run 상태로 변경.
// 이렇게 해야 안전하다. Suspend상태일 땐 
// 스레드에 무슨 짓을 할 수가 없기 때문.
// 즉, Run 상태로 변경되기 전까지는 상태변경 함수 사용 불가.

void Thread::Resume()
{
	// 당연하지만.. 스스로 자신을 resume할 수 없다. 
	// 이 코드를 실행하는 스레드는 다른 스레드여야 함.
	assert (GetCurrentThreadID() != m_id);

	bool isReturn = false;
	if (IsAlive() == false)
		isReturn = true;
	else if ((m_state != STATE::SUSPEND) && (m_state != STATE::READY))
		isReturn = true;

	if (isReturn)
		return;

	SCOPE_WAKECV(m_suspend_lock);
	m_signal_suspend = false;
	SetState(STATE::RUN);
}

void Thread::Suspend()
{
	if (IsAlive() == false)
		return;
	else if (m_state != STATE::RUN)
		return;

	m_signal_suspend = true;
}

void Thread::_Suspend()
{
	if (IsAlive() == false)
		return;
	else if (m_state != STATE::RUN)
		return;
	
	SetState(STATE::SUSPEND);
	SCOPE_SLEEPCV(m_suspend_lock);
}

void Thread::Join()
{
	WaitForSingleObject(m_handle, INFINITE);
}

// 준비되어 있는 다음 쓰레드로 바꿔준다
BOOL Thread::Yielding()
{
	// look for a ready state thread.
	return SwitchToThread();
}

BOOL Thread::Interrupt()
{
	if (m_signal_suspend)
		_Suspend();
	if (m_signal_close)
		return false;

	return true;
}

void Thread::Ready()
{
	if (IsAlive() == false)
		return;
		
	SetState(STATE::READY);
	SCOPE_SLEEPCV(m_suspend_lock);
}

// 쓰레드 만드러!
void Thread::Create()
{
	UINT threadID;
	// not use CREATE_SUSPENDED. because i use condition variable.
	m_handle = (HANDLE)_beginthreadex(nullptr, 0,  &Thread::handle, (void*)this, 0, &threadID);
	
	if (m_handle == INVALID_HANDLE_VALUE)
	{
		DWORD err = ::GetLastError();		
		exit(0);
	}
}

void Thread::Close()
{
	if (m_state == STATE::DEAD)
		return;

	m_signal_close = true;
}

// 쓰레드가 종료되면 할당됬던 메모리 해제하고 데이터 정리
void Thread::_Close()
{
	if (m_handle != 0)
	{
		SetState(STATE::DEAD);
		_endthreadex(0);
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;		
	}
}

int Thread::GetCurrentThreadID()
{
	// 아직 t_pInstance = pThread; 코드 실행 전인데 호출된 경우.
	// 이 경우 GetState를 호출해 보면 아직 CREATE상태로 표시될 것이다.
	if (Thread::GetCurrent() == nullptr)
	{
		return -1;
	}

	return Thread::GetCurrent()->GetID();
}

unsigned Thread::handle(void* pData)
{
	if (pData != 0)
	{
		Thread* pThread = static_cast<Thread*>(pData);
		t_pInstance = pThread;

		while (pThread->m_signal_close == false)
		{
			pThread->Ready();
			pThread->callback(t_pInstance);
		}

		pThread->_Close();
	}
	else
	{
		//Crash
	}

	return 0;
}