#pragma once

#include <Windows.h>
#pragma comment(lib, "winmm.lib")

#include "..\\Common\\Enum.h"
#include "..\\Interface\\Base_Platform.h"

#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")

// using WS_THICKFRAME = SM_CXSIZEFRAME / SM_CYSIZEFRAME
// unusing WS_THICKFRAME = SM_CXFIXEDFRAME / SM_CYFIXEDFRAME

class Windows : public _Platform{
public:
	static _Platform* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new Windows;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Win32){
			delete m_pSingleton;
			m_pSingleton = new Windows;
		}
		return m_pSingleton;
	}

public:
	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Win32;}

	void Setting(PLATFORM_SETTING*, void (*loop)());

	bool Start();
	void Run();
	void End();

	void OverWin8(WINDOWS_SETTING const*);
	void UnderWin8(WINDOWS_SETTING const*);

private:
	HWND m_hwnd;
	MSG m_msg;
	WNDCLASSEX m_WndClass;
	HINSTANCE m_hInstance;

	POINT m_WinSize;
	INT zDelta;

	Windows(){}
	~Windows(){}

private:
	void Create(HINSTANCE hInstance, LPWSTR Title, int *nCmdShow, int x, int y){
		m_WndClass.cbSize = sizeof(WNDCLASSEX);

		m_WndClass.style = 0;     					// �⺻ ��Ÿ�� 
		m_WndClass.lpfnWndProc = WndProc;  			// window �Լ�
		m_WndClass.cbClsExtra = 0;    				// �߰� ������ �ʿ����� ����(�޵��찡 ����ϴ� ���� ����)
		m_WndClass.cbWndExtra = 0;    				// �߰� ������ �ʿ����� ����
		m_WndClass.hInstance = hInstance;   			// �ν��Ͻ��� ���� �ڵ�
		m_WndClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)); // ������ ��Ÿ��
		m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// Ŀ�� ��Ÿ��
		m_WndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH) ; // ������ ������ ������� ��
		m_WndClass.lpszClassName = Title;		// Window Ŭ���� �̸�
		m_WndClass.lpszMenuName = NULL;  			// �޴� ������� ����

		m_WndClass.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

		if(!RegisterClassEx(&m_WndClass)){return;}

		// GetSystemMextrics(nInfo)
		// nIndex
		// SM_CXSCREEN = �� ���÷��� ���λ�����
		// SM_CYSCREEN = �� ���÷��� ���λ�����
		// SM_CXSIZEFRAME = ���������� ������
		// SM_CYSIZEFRAME = ���������� ������
		// SM_CYCAPTION = ĸ�ǹ��� ����

		DWORD style = WS_POPUP;//WS_OVERLAPPED;
		DWORD ex_style = 0L;

		if(static_cast<WINDOWS_SETTING*>(m_Setting)->titlebar){style |= WS_CAPTION | WS_SYSMENU;}
		if(static_cast<WINDOWS_SETTING*>(m_Setting)->border){style |= WS_SIZEBOX;}
		if(static_cast<WINDOWS_SETTING*>(m_Setting)->layered){ex_style |= WS_EX_LAYERED;}

		int frame_x = GetSystemMetrics(SM_CXFIXEDFRAME)*2;
		int frame_y = (GetSystemMetrics(SM_CYFIXEDFRAME)*2) + GetSystemMetrics(SM_CYCAPTION);
		
		m_hwnd = CreateWindowEx(
			ex_style,
			Title,					// ������ Ŭ���� �̸�
			Title,					// Ÿ��Ʋ
			style,		// ������ ��Ÿ�� (normal = WS_OVERLAPPEDWINDOW)
			(GetSystemMetrics(SM_CXSCREEN) - (x + frame_x))/2,//CW_USEDEFAULT,				// X��ǥ - Windows�� ����
			(GetSystemMetrics(SM_CYSCREEN) - (y + frame_y))/2,//CW_USEDEFAULT,				// Y��ǥ - Windows�� ����
			x + frame_x,						// ���� - windows�� ����
			y + frame_y,						// ���� - windows�� ����
			HWND_DESKTOP,				// �θ� window ����
			NULL,						// �޴� ������� ����
			hInstance,					// �� ���α׷��� �ν��Ͻ� �ڵ�
			(void*)this);						// �߰����� �Ű����� ����

		ShowWindow(m_hwnd, *nCmdShow);
		UpdateWindow(m_hwnd);

		ZeroMemory(&m_msg,sizeof(m_msg));

		m_WinSize.x = x;
		m_WinSize.y = y;

		m_hInstance = hInstance;

		if(static_cast<WINDOWS_SETTING*>(m_Setting)->layered){
			DWM_BLURBEHIND bb = {0};
			HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
			bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
			bb.hRgnBlur = hRgn;
			bb.fEnable = TRUE;
			DwmEnableBlurBehindWindow(m_hwnd, &bb);

			SetLayeredWindowAttributes(m_hwnd, 0x00, 0xFF, LWA_ALPHA);
		}
	}
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
		Windows* pWindow = (Windows*)Windows::GetSingleton();
		bool Processed = false;

		switch(iMsg){
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			Processed = pWindow->MessageHandler(hwnd, iMsg, wParam, lParam);
			if( Processed == false ){
				return DefWindowProc (hwnd, iMsg, wParam, lParam);
			}
			break;
		}

		return 0;
	}

	bool MessageHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lPAram);

public:
	POINT GetWindowSize(){return m_WinSize;}
	HWND Gethwnd(){return m_hwnd;}
	void GetzDelta(INT& zd){zd = zDelta; zDelta = 0;}
};