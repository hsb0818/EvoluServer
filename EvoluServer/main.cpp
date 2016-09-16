#include "pch.h"

INT WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	cApp::GetInstance()->Create(hInstance);
	THREAD_POOL().CreateThreadPool(0);
	THREAD_POOL().Execute(Test_Loop);
	THREAD_POOL().Execute(Test_Loop);
	THREAD_POOL().Execute(Test_Loop);

	MainThread_Loop();

	THREAD_POOL().Close();
	cApp::GetInstance()->Close();
	cApp::DeleteApp();
	return 0;
}