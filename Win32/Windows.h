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

		m_WndClass.style = 0;     					// 기본 스타일 
		m_WndClass.lpfnWndProc = WndProc;  			// window 함수
		m_WndClass.cbClsExtra = 0;    				// 추가 정보는 필요하지 않음(왼도우가 사용하는 예약 영역)
		m_WndClass.cbWndExtra = 0;    				// 추가 정보는 필요하지 않음
		m_WndClass.hInstance = hInstance;   			// 인스턴스에 대한 핸들
		m_WndClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)); // 아이콘 스타일
		m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// 커서 스타일
		m_WndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH) ; // 윈도우 배경색을 흰색으로 함
		m_WndClass.lpszClassName = Title;		// Window 클래스 이름
		m_WndClass.lpszMenuName = NULL;  			// 메뉴 사용하지 않음

		m_WndClass.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

		if(!RegisterClassEx(&m_WndClass)){return;}

		// GetSystemMextrics(nInfo)
		// nIndex
		// SM_CXSCREEN = 주 디스플레이 가로사이즈
		// SM_CYSCREEN = 주 디스플레이 세로사이즈
		// SM_CXSIZEFRAME = 가로프레임 사이즈
		// SM_CYSIZEFRAME = 세로프레임 사이즈
		// SM_CYCAPTION = 캡션바의 높이

		DWORD style = WS_POPUP;//WS_OVERLAPPED;
		DWORD ex_style = 0L;

		if(static_cast<WINDOWS_SETTING*>(m_Setting)->titlebar){style |= WS_CAPTION | WS_SYSMENU;}
		if(static_cast<WINDOWS_SETTING*>(m_Setting)->border){style |= WS_SIZEBOX;}
		if(static_cast<WINDOWS_SETTING*>(m_Setting)->layered){ex_style |= WS_EX_LAYERED;}

		int frame_x = GetSystemMetrics(SM_CXFIXEDFRAME)*2;
		int frame_y = (GetSystemMetrics(SM_CYFIXEDFRAME)*2) + GetSystemMetrics(SM_CYCAPTION);
		
		m_hwnd = CreateWindowEx(
			ex_style,
			Title,					// 윈도우 클래스 이름
			Title,					// 타이틀
			style,		// 윈도우 스타일 (normal = WS_OVERLAPPEDWINDOW)
			(GetSystemMetrics(SM_CXSCREEN) - (x + frame_x))/2,//CW_USEDEFAULT,				// X좌표 - Windows가 결정
			(GetSystemMetrics(SM_CYSCREEN) - (y + frame_y))/2,//CW_USEDEFAULT,				// Y좌표 - Windows가 결정
			x + frame_x,						// 넓이 - windows가 결정
			y + frame_y,						// 높이 - windows가 결정
			HWND_DESKTOP,				// 부모 window 없음
			NULL,						// 메뉴 사용하지 않음
			hInstance,					// 이 프로그램의 인스턴스 핸들
			(void*)this);						// 추가적인 매개변수 없음

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