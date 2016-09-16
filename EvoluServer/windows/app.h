#ifndef _CAPP_H
#define _CAPP_H


class cApp
{
private:
	HWND					m_hWnd;
	HINSTANCE			m_hInst;
	MSG						m_msg;
	WNDCLASSEX		m_wc;

private:
	static cApp*		m_pApp;
	
public:
	static cApp* GetInstance()
	{
		if(m_pApp == NULL)
		{
			m_pApp = new cApp;
		}
		return m_pApp;
	}

	static void DeleteApp()
	{
		SAFE_DELETE(m_pApp);
	}

public:
	cApp();
	~cApp() {};

public:
	void Create(HINSTANCE hInst);
	void Close();

	HWND	GethWnd() { return m_hWnd; }
	HINSTANCE	GethInst() { return m_hInst; }
	MSG&	GetMsg() { return m_msg; }
				
	LRESULT	ReturnProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

private:
	void	InitWindow(WCHAR* pTitle, int width = 1024, int height = 768);
	void	FullMode(WCHAR* pTitle, int width, int height);
	void	WindowMode(WCHAR* pTitle, int width, int height);	
};

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );


#endif