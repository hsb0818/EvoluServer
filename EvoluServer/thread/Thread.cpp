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

// ���� ��ȯ ���߿� �ٸ� �����忡�� �� �Լ��� ȣ��Ǿ �������� �����ߴ�.
// Suspend ������ �� �ǵ帮�� �ʴ� ó���� �� ���̴�.
Thread::STATE Thread::GetState()
{
	return m_state;
}

// Suspend�Ǿ��� Thread�� ���� �� Run ���·� ����.
// �̷��� �ؾ� �����ϴ�. Suspend������ �� 
// �����忡 ���� ���� �� ���� ���� ����.
// ��, Run ���·� ����Ǳ� �������� ���º��� �Լ� ��� �Ұ�.

void Thread::Resume()
{
	// �翬������.. ������ �ڽ��� resume�� �� ����. 
	// �� �ڵ带 �����ϴ� ������� �ٸ� �����忩�� ��.
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

// �غ�Ǿ� �ִ� ���� ������� �ٲ��ش�
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

// ������ ���巯!
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

// �����尡 ����Ǹ� �Ҵ��� �޸� �����ϰ� ������ ����
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
	// ���� t_pInstance = pThread; �ڵ� ���� ���ε� ȣ��� ���.
	// �� ��� GetState�� ȣ���� ���� ���� CREATE���·� ǥ�õ� ���̴�.
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