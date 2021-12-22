#include "header.h"

#include <Windows.h>
#include <algorithm>
#include <vector>
#include <cmath>
#include <random>

#pragma comment(lib, "..\\debug\\Win32.lib")

void main(){
	HINSTANCE hInst=GetModuleHandle(NULL);
	int CmdShow = SW_SHOW;

	PLATFORM_SETTING* set = new WINDOWS_SETTING;

	set->title = L"◁ Test console ▷ ◀ 테스트 콘솔 ▶";
	set->x = 1280;
	set->y = 720;
	set->FullScreen = false;

	set->g_type = LIBRARY_TYPE::GRAPHIC::OpenGL;
	//set->s_type = LIBRARY_TYPE::SOUND::XAudio2;

	//set->g_type = LIBRARY_TYPE::GRAPHIC::NONE;
	set->s_type = LIBRARY_TYPE::SOUND::NONE;

	static_cast<WINDOWS_SETTING*>(set)->cmdShow = &CmdShow;
	static_cast<WINDOWS_SETTING*>(set)->thishInst = hInst;
	static_cast<WINDOWS_SETTING*>(set)->position = 0;
	static_cast<WINDOWS_SETTING*>(set)->titlebar = true;
	static_cast<WINDOWS_SETTING*>(set)->border = false;
	static_cast<WINDOWS_SETTING*>(set)->layered = false;

	PCS::Start(set, starter);
	PCS::Run();
	PCS::End();

	delete set;

	return;
}