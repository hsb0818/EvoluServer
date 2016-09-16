#include "../pch.h"

//threadmax is 'n-1' cause have to minus mainthread count;
void ThreadPool::CreateThreadPool(int n)
{
	m_threadMax = n;

	if (m_threadMax == 0)
	{
		SYSTEM_INFO sysInfo;
		::GetSystemInfo(&sysInfo);

		m_threadMax = sysInfo.dwNumberOfProcessors;

		if (m_threadMax <= 1)
			return;

		m_threadMax -= 1; // 1 is mainthread.
	}

	for (unsigned int i = 1; i <= m_threadMax; i++)
	{
		worker * pWorker = new worker(Dummy_Loop, i);
		if (pWorker == nullptr)
		{
			continue;
		}

		m_Workers.push_back(pWorker);
	}

	WaitToReady(); // Ready상태가 됬는지 확인(thread의 handle 함수에서 ready로 만듦)
}

void ThreadPool::AllResume()
{
	for (VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++)
	{
		(*itr)->Resume();
	}
}

// 모두 준비가 되었는지 검사.
void ThreadPool::WaitToReady()
{
	bool IsAllReady = false;
	while (IsAllReady == false)
	{
		IsAllReady = true;
		for (VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++)
		{
			if ((*itr)->GetState() == Thread::READY)
				continue;

			IsAllReady = false;
			break;
		}

		Sleep(1);
	}
}

// 메인에서 작업을 요청할 수도 있지만,
// 메인이 아닌 다른 스레드에서도 메인으로 작업을 요청할 수 있다.
void ThreadPool::Execute(PFUNCTION threadFunc)
{
	if (GetCountOfRunningThread() >= GetThreadMax())
		return;

	{
		SCOPE_LOCK(m_lock);
		m_jobs.push_back(threadFunc);
	}
}

// 쉬고 있는 스레드를 찾아서 작업을 넣어 주고 실행한다.
// 알아서 루프에 넣어 줘요. 예를 들어 윈도우즈면 메시지루프쪽
// 비어 있으면 true.
bool ThreadPool::Processor()
{
	if (GetThreadMax() == 0)
		return true;

	if (m_jobs.empty() == false)
	{
		for (VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++)
		{
			if ((*itr)->GetState() != Thread::READY)
				continue;

			(*itr)->setExecute(m_jobs.front());
			m_jobs.pop_front();
			(*itr)->Resume();
			return true;
		}

		return false;
	}

	return true;
}

void ThreadPool::Close()
{
	// Close 신호를 주고,
	for ( VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++ )
	{	
		(*itr)->Close();
	}
	
	// 모든 스레드가 종료될 때까지 기다린다.
	for ( VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++ )
	{	
		(*itr)->Join();
	}
}

ULONG ThreadPool::GetCountOfRunningThread()
{
	ULONG count = 0;
	for ( VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++ )
	{
		if ((*itr)->GetState() == Thread::RUN)
			++count;
	}

	return count;
}