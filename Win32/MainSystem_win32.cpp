#pragma once

#include "..\\BaseLibrary\\MainSystem.h"
#include "..\\Interface\\Base_Platform.h"
#include "Windows.h"

void PCS::Start(PLATFORM_SETTING* setting, void (*starter)() = nullptr){
	Windows::GetSingleton();
	_Platform::GetSingleton()->Setting(setting, PCS::MainLoop);
	_Platform::GetSingleton()->Start();
	if(starter != nullptr){starter();}
}

void* PCS::GetAppData(){return nullptr;}
BYTE PCS::tGetKeyState_Back(){return 0x00;}
void PCS::ExitProgram(){return;}