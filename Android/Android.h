#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"
#include "..\\Interface\\Base_Platform.h"

#include "android_native_app_glue.h"

class Android : public _Platform{
private:
	struct android_app* app;

	bool run;

public:
	void (*starter)() = nullptr;

private:
	Android() : app(nullptr), run(false){}
	~Android(){}

public:
	static void engine_handle_cmd(struct android_app* app, int32_t cmd);
	static int32_t engine_handle_input(struct android_app* app, AInputEvent* event);

public:
	static _Platform* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new Android;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Android){
			delete m_pSingleton;
			m_pSingleton = new Android;
		}
		return m_pSingleton;
	}

	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Android;}

	void Setting(PLATFORM_SETTING*, void (*)());
	
	bool Start();
	void Run();
	void End();

	void Pause(){run = false;}
	void Resume(){run = true;}

	void SetWindowSize(UINT x, UINT y){m_Setting->x = x; m_Setting->y = y;}
	UINT GetWindowSizeX(){return m_Setting->x;}
	UINT GetWindowSizeY(){return m_Setting->y;}
	android_app* GetAndroidApp(){return app;}
	void ExitProgram(){ANativeActivity_finish(GetAndroidApp()->activity);}

private:
	BYTE back_key;

public:
	void SetKeyCode_Back(BYTE state){back_key = state;}
	BYTE GetKeyCode_Back(){return back_key;}
};