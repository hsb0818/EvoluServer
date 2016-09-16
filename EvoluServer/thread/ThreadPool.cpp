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

	WaitToReady(); // Ready���°� ����� Ȯ��(thread�� handle �Լ����� ready�� ����)
}

void ThreadPool::AllResume()
{
	for (VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++)
	{
		(*itr)->Resume();
	}
}

// ��� �غ� �Ǿ����� �˻�.
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

// ���ο��� �۾��� ��û�� ���� ������,
// ������ �ƴ� �ٸ� �����忡���� �������� �۾��� ��û�� �� �ִ�.
void ThreadPool::Execute(PFUNCTION threadFunc)
{
	if (GetCountOfRunningThread() >= GetThreadMax())
		return;

	{
		SCOPE_LOCK(m_lock);
		m_jobs.push_back(threadFunc);
	}
}

// ���� �ִ� �����带 ã�Ƽ� �۾��� �־� �ְ� �����Ѵ�.
// �˾Ƽ� ������ �־� ���. ���� ��� ��������� �޽���������
// ��� ������ true.
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
	// Close ��ȣ�� �ְ�,
	for ( VECTOR::iterator itr = m_Workers.begin(); itr != m_Workers.end(); itr++ )
	{	
		(*itr)->Close();
	}
	
	// ��� �����尡 ����� ������ ��ٸ���.
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