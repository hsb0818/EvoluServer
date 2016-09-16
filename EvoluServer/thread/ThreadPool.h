#pragma once

#include "../system/interface.h"
#include "Thread.h"
#include "ThreadLoop.h"

using namespace SOC_Thread;

class ThreadPool : public Singleton<ThreadPool>
{
public:
	typedef	void (*PFUNCTION)(Thread* pCurThread);

public:
	ThreadPool() : m_threadMax(0) {}
	virtual ~ThreadPool() {}

	void CreateThreadPool(int n);
	void Execute(PFUNCTION threadFunc);
	void AllResume();
	bool Processor();
	void Close();
	ULONG GetCountOfRunningThread();
	ULONG GetThreadMax() { return m_threadMax; }

private:
	void WaitToReady();

private:	
	class worker : public Thread
	{
	public:
		worker(PFUNCTION threadFunc, int id)
			:	Thread(id)
			,	m_threadFunc(threadFunc)
		{}

		virtual ~worker()
		{}

		void callback(Thread* pCurThread) {	m_threadFunc(pCurThread);	}
		void setExecute(PFUNCTION threadFunc) { m_threadFunc = threadFunc; }

	private:
		PFUNCTION	m_threadFunc;
	};

	typedef	std::vector<worker*> VECTOR;
	typedef std::deque<PFUNCTION> QUEUE;

private:
	Lock::CriticalSectionLock m_lock;

	VECTOR		m_Workers;
	QUEUE		m_jobs;
	ULONG		m_threadMax;
};

static ThreadPool&
THREAD_POOL()
{
	return ThreadPool::GetInstance();
}