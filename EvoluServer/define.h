#ifndef _DEFINE_H_
#define _DEFINE_H_

#ifdef _DEBUG
#pragma comment( linker, "/entry:WinMainCRTStartup /subsystem:console" )
#endif

#define SAFE_DELETE(p)	if (p) { delete (p); (p) = nullptr; }

#ifdef _WIN64
typedef LONGLONG LONGTYPE;
typedef ULONGLONG ULONGTYPE;
#else
typedef LONG LONGTYPE;
typedef ULONG ULONGTYPE;
#endif

#endif