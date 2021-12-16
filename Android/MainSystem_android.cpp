#include "..\\BaseLibrary\\MainSystem.h"
#include "..\\Interface\\Base_Platform.h"
#include "Android.h"

void PCS::Start(PLATFORM_SETTING* setting, void (*starter)() = nullptr){
	Android::GetSingleton();
	_Platform::GetSingleton()->Setting(setting, PCS::MainLoop);
	_Platform::GetSingleton()->Start();
	((Android*)(Android::GetSingleton()))->starter = starter;
}

void* PCS::GetAppData(){
	return ((Android*)(Android::GetSingleton()))->GetAndroidApp();
}

BYTE PCS::tGetKeyState_Back(){
	return ((Android*)(Android::GetSingleton()))->GetKeyCode_Back();
}

void PCS::ExitProgram(){
	((Android*)(Android::GetSingleton()))->ExitProgram();
}