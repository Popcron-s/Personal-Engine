#pragma once

#include <Windows.h>
#pragma comment(lib, "winmm.lib")

#include <VersionHelpers.h>

#include "Windows.h"

#include "..\\Interface\\Base_Output_Graphics.h"
#include "..\\Interface\\Base_Output_Sounds.h"

#include "OpenGL.h"
#include "XAudio2.h"

#include "WinInput.h"
#include "Timer.h"
#include "FileIO.h"

void Windows::Setting(PLATFORM_SETTING* setting, void (*loop)()){
	m_Setting = setting;
	MainLoop = loop;
}

bool Windows::Start(){
	if(m_Setting == nullptr){return false;}
	WINDOWS_SETTING* win_set = (WINDOWS_SETTING*)m_Setting;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if(IsWindows8OrGreater()){OverWin8(win_set);}
	else{UnderWin8(win_set);}

	return true;
}
void Windows::Run(){
	while(WM_QUIT != m_msg.message){
		if(PeekMessage(&m_msg, nullptr, 0U, 0U, PM_REMOVE)){
			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
		if(MainLoop != nullptr){MainLoop();}
	}
}
void Windows::End(){
	CoUninitialize();
}
void Windows::OverWin8(WINDOWS_SETTING const* win_set){
	Create((HINSTANCE)win_set->thishInst, win_set->title, win_set->cmdShow, win_set->x+10, win_set->y+10);
	switch(win_set->g_type){
	case LIBRARY_TYPE::GRAPHIC::OpenGL:
		OpenGL::GetSingleton()->Init();
		break;
	default:
		_O_Graphics::Destroy();
		break;
	}
	switch(win_set->s_type){
	case LIBRARY_TYPE::SOUND::XAudio2:
		XAudio2::GetSingleton()->Init();
		break;
	default:
		_O_Sounds::Destroy();
		break;
	}

	_Win32Keyboard::GetSingleton();
	_Win32Mouse::GetSingleton();
	_Win32Timer::GetSingleton();
	_Win32FileIO::GetSingleton();
}
void Windows::UnderWin8(WINDOWS_SETTING const* win_set){
	Create((HINSTANCE)win_set->thishInst, win_set->title, win_set->cmdShow, win_set->x, win_set->y);
	switch(win_set->g_type){
	case LIBRARY_TYPE::GRAPHIC::OpenGL:
		OpenGL::GetSingleton()->Init();
		break;
	default:
		_O_Graphics::Destroy();
		break;
	}
	switch(win_set->s_type){
	default:
		_O_Sounds::Destroy();
		break;
	}

	_Win32Keyboard::GetSingleton();
	_Win32Mouse::GetSingleton();
	_Win32Timer::GetSingleton();
	_Win32FileIO::GetSingleton();
}

bool Windows::MessageHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lPAram){
	switch(iMsg){
	case WM_MOUSEWHEEL:
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam)/120;
		return true;
	default:
		return false;
	}
}