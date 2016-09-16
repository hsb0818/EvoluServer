#include "../pch.h"
#include "ThreadLoop.h"

using namespace std;

HACCEL hAccelTable;

void Dummy_Loop(Thread* pCurThread)
{
	if (pCurThread == nullptr)
		return;

	printf("%d Thread Resume\n", pCurThread->GetID());

	while(true)
	{
		pCurThread->Interrupt();

		printf("%d Run\n", pCurThread->GetID());
		Sleep(pCurThread->GetID() * 100);
	}
}

void Test_Loop(Thread* pCurThread)
{
	if (pCurThread == nullptr)
		return;

	printf("TestLoop : %d Thread Resume\n", pCurThread->GetID());

	while(pCurThread->Interrupt())
	{
		printf("%d Run\n", pCurThread->GetID());
		Sleep(pCurThread->GetID() * 100);
	}
}

void MainThread_Loop()
{
	printf("main thread running\n");

	while (cApp::GetInstance()->GetMsg().message != WM_QUIT)
	{
		if (PeekMessage(&cApp::GetInstance()->GetMsg(), NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&cApp::GetInstance()->GetMsg());
			DispatchMessage(&cApp::GetInstance()->GetMsg());
		}
		else {}

		THREAD_POOL().Processor();
		Sleep(1);
	}
/*
	while (GetMessage(&cApp::GetInstance()->GetMsg(), NULL, 0, 0))
	{
		if (!TranslateAccelerator(cApp::GetInstance()->GetMsg().hwnd, hAccelTable, &cApp::GetInstance()->GetMsg()))
		{
			TranslateMessage(&cApp::GetInstance()->GetMsg());
			DispatchMessage(&cApp::GetInstance()->GetMsg());
		}

		printf("main\n");
		THREAD_POOL().Processor();
		Sleep(1);
	}
*/
}