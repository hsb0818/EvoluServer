#include "../pch.h"
//¼Ó¼ºµµ ÇÔ²² ¹Ù²ãÁÜ use·Î

cApp*						cApp::m_pApp = NULL;
DWORD						EndTime = GetTickCount();
const float					g_nFrameLimit = 1000.f / 60.f;

cApp::cApp()
{
	ZeroMemory(&m_msg, sizeof(m_msg));
	m_hWnd					= NULL;
	m_hInst					= NULL;
}

void cApp::Close()
{
}


void cApp::InitWindow(WCHAR* pTitle, int width, int height)
{
	m_wc.cbSize				= sizeof(m_wc);
	m_wc.cbClsExtra			= 0;
	m_wc.cbWndExtra			= 0;
	m_wc.hbrBackground		= (HBRUSH)GetStockObject(WHITE_BRUSH);
	m_wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	m_wc.hIcon				= LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
	m_wc.hIconSm			= LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
	m_wc.hInstance			= m_hInst;
	m_wc.lpszClassName		= pTitle;
	m_wc.lpfnWndProc		= WndProc;
	m_wc.lpszMenuName		= NULL;
	m_wc.style				= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&m_wc);

	//#if defined(DEBUG) || defined(_DEBUG)
	WindowMode(pTitle, width, height);
	//#else
	//FullMode(pTitle, width, height);
	//#endif
}


void cApp::Create(HINSTANCE hInst)
{
	m_hInst					= hInst;
	InitWindow(L"Calculator Server", 1024, 768);
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);	
}

void cApp::FullMode(WCHAR* pTitle, int width, int height)
{
	DWORD	dwStyle  		= WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
	// 	m_bFull				= 0;
	m_hWnd = CreateWindow(pTitle, 
		pTitle,
		dwStyle,
		0, 
		0, 
		width, 
		height, 
		NULL, 
		NULL, 
		m_hInst, 
		NULL);
}


void cApp::WindowMode(WCHAR* pTitle, int width, int height)
{
	DWORD	dwStyle;
	DWORD	dwWidth, dwHeight;
	DWORD	FrmW, FrmH, CapH, MenuH;
	dwStyle					= WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_EX_TOPMOST;
	FrmW					= GetSystemMetrics( SM_CXSIZEFRAME );
	FrmH					= GetSystemMetrics( SM_CYSIZEFRAME );
	CapH					= GetSystemMetrics( SM_CYCAPTION   );
	MenuH					= 0 ? GetSystemMetrics( SM_CYMENU ) : 0;

	dwWidth					= ((FrmW-1) * 2);
	dwHeight				= ((FrmH-1) * 2) + MenuH + CapH;

	m_hWnd = CreateWindow(pTitle,
		pTitle,
		dwStyle,
		50, 
		50, 
		width,
		height,
		NULL, 
		NULL, 
		m_hInst, 
		NULL);

	RECT rt;
	SetRect(&rt, 0,0,width, height);
	AdjustWindowRect(&rt, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, TRUE);
	SetWindowPos(m_hWnd, NULL, 0,0,  rt.right - rt.left, rt.bottom - rt.top, SWP_NOMOVE | SWP_NOZORDER);
}

LRESULT cApp::ReturnProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( msg )
	{
	case WM_CREATE:
		{
			break;
		}
	case WM_SIZE:
		{
			break;
		}
	case WM_KEYDOWN:
		{
#if defined(DEBUG) || defined(_DEBUG)
			if( wParam == VK_ESCAPE )
			{
				PostQuitMessage( 0 );
			}
#endif
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_QUIT:
	case WM_DESTROY:
		{
			struct tm fnow;
			time_t now;
			time(&now);
			localtime_s( &fnow, &now );
		}

		PostQuitMessage(0);
		break;
	default:
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return DefWindowProc( hWnd, msg, wParam ,lParam );
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return cApp::GetInstance()->ReturnProc( hWnd, msg, wParam, lParam );
}