#include "Android.h"
#include "OpenGLES.h"
#include "OpenSLES.h"
#include "FileIO.h"
#include "AndroidTouch.h"
#include "GyroSensor.h"

#include "..\\BaseLibrary\\Module.h"
#include "..\\BaseLibrary\\ObjectManager.h"

#include "..\\Interface\\Base_Output_Graphics.h"

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "focus test", __VA_ARGS__))

/**
* ���� �� ����� ó���մϴ�.
*/
void Android::engine_handle_cmd(struct android_app* app, int32_t cmd) {
	Android* plat = (Android*)(Android::GetSingleton());
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		LOGD("APP_CMD_SAVE_STATE");
		// �ý��ۿ��� ���� ���¸� �����ϵ��� ��û�߽��ϴ�. �����ϼ���.
		//app->savedState = malloc(sizeof(struct saved_state));
		//*((struct saved_state*)app->savedState) = plat->state;
		//app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		LOGD("APP_CMD_INIT_WINDOW");
		if(app->window != NULL){
			if(_O_Graphics::GetSingleton() != nullptr){
				LOGD("Gain Focusing OpenGL es");
				((OpenGLES*)_O_Graphics::GetSingleton())->Focusing(true);
			}
		}
		// â�� ǥ�õǾ� �غ� ���ƽ��ϴ�.
		if(plat->starter != nullptr){
			if(app->window != NULL){
				if(_O_Graphics::GetSingleton() != nullptr){
					_O_Graphics::GetSingleton()->Init();
				}
			}
			plat->starter();
			plat->starter = nullptr;
		}
		break;
	case APP_CMD_TERM_WINDOW:
		LOGD("APP_CMD_TERM_WINDOW");
		// â�� ����ų� �ݾ� �����մϴ�.
		if (_O_Graphics::GetSingleton() != nullptr){
			LOGD("Lost Focusing OpenGL es");
			((OpenGLES*)_O_Graphics::GetSingleton())->Focusing(false);
		}
		break;
	case APP_CMD_CONFIG_CHANGED:
		LOGD("APP_CMD_CONFIG_CHANGED");
		//((ANDROID_SETTING*)(app->userData))->orientation = AConfiguration_getOrientation(app->config);
		break;
	case APP_CMD_GAINED_FOCUS:
		LOGD("APP_CMD_GAINED_FOCUS");
		if(_I_GyroSensor::GetSingleton() != nullptr){
			((GyroSensor*)GyroSensor::GetSingleton())->Focusing();
		}
		if(_O_Sounds::GetSingleton() != nullptr){
			Audio** aarr = nullptr;
			aarr = ObjectManager::GetSingleton()->GetAudio(aarr);
			UINT anum = ObjectManager::GetSingleton()->GetAudioNum();
			LOGD("anum : %d", anum);

			for(UINT i = 0 ; i<anum ; ++i){
				aarr[i]->Play();
			}

			aarr = nullptr;
			anum = 0;
		}
		plat->Resume();
		break;
	case APP_CMD_LOST_FOCUS:
		LOGD("APP_CMD_LOST_FOCUS");
		if(_I_GyroSensor::GetSingleton() != nullptr){
			((GyroSensor*)GyroSensor::GetSingleton())->Disable();
		}
		if(_O_Sounds::GetSingleton() != nullptr){
			Audio** aarr = nullptr;
			aarr = ObjectManager::GetSingleton()->GetAudio(aarr);
			UINT anum = ObjectManager::GetSingleton()->GetAudioNum();
			LOGD("anum : %d", anum);

			for(UINT i = 0 ; i<anum ; ++i){
				aarr[i]->Pause();
			}

			aarr = nullptr;
			anum = 0;
		}
		// �ִϸ��̼ǵ� �����˴ϴ�.
		plat->Pause();
		break;
	}
}

int32_t Android::engine_handle_input(struct android_app* app, AInputEvent* event){
	Android* plat = (Android*)(Android::GetSingleton());
	if(AKeyEvent_getKeyCode(event) == AKEYCODE_BACK){
		if(AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN){
			plat->SetKeyCode_Back(0x80);
		}
		else if(AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP){
			plat->SetKeyCode_Back(0x00);
		}
		return 1;
	}
	else{
		return AndroidTouch::engine_handle_input(app, event);
	}
}

void Android::Setting(PLATFORM_SETTING* setter, void (*main_loop)()){
	m_Setting = setter;
	MainLoop = main_loop;

	app = (android_app*)(static_cast<ANDROID_SETTING*>(m_Setting)->app_state);
}

bool Android::Start(){
	if(app == nullptr){return false;}

	app->userData = m_Setting;
	app->onAppCmd = engine_handle_cmd;
	app->onInputEvent = Android::engine_handle_input;//engine_handle_input;
	
	//ACONFIGURATION_ORIENTATION_PORT = ����
	//ACONFIGURATION_ORIENTATION_LAND = ����
	((ANDROID_SETTING*)m_Setting)->orientation = AConfiguration_getOrientation(app->config);

	if (app->savedState != NULL) {
		// ������ ����� ���·� ���۵Ǹ�, �� �������� �����˴ϴ�.
		//state = *(struct saved_state*)app->savedState;
	}

	if(m_Setting->g_type == LIBRARY_TYPE::GRAPHIC::OpenGL){
		OpenGLES::GetSingleton();
	}

	switch(m_Setting->s_type){
	case LIBRARY_TYPE::SOUND::OpenSL: OpenSLES::GetSingleton(); break;
	}
	AndroidFileIO::GetSingleton();
	AndroidTouch::GetSingleton();
	((GyroSensor*)GyroSensor::GetSingleton())->Init(app);

	_O_Sounds::GetSingleton()->Init();

	run = true;

	return true;
}

void Android::Run(){
	while(true){// ���� ���� ��� �̺�Ʈ�� �н��ϴ�.
		int ident;
		int events;
		struct android_poll_source* source;

		// �ִϸ��̼��� �������� ������ �̺�Ʈ ��⸦ ���������� �����մϴ�.
		// �ִϸ��̼��� �����ϸ� ��� �̺�Ʈ�� ���� ������ ������ ������ ����
		// ����ؼ� �ִϸ��̼��� ���� �������� �׸��ϴ�.
		while((ident = ALooper_pollAll(run?0:-1, NULL, &events, (void**)&source)) >= 0){
			// �� �̺�Ʈ�� ó���մϴ�.
			if(source != NULL){source->process(app, source);}

			// ������ �����Ͱ� ������ �ٷ� ó���˴ϴ�.
			if(ident == LOOPER_ID_USER){
				if(_I_GyroSensor::GetSingleton() != nullptr){
					((GyroSensor*)GyroSensor::GetSingleton())->GetEvent();
				}
			}

			// ���� ������ Ȯ���մϴ�.
			if(app->destroyRequested != 0){
				//End();
				run = false;
				return;
			}
		}

		if(run){if(MainLoop != nullptr){MainLoop();}}

		UINT mp = ObjectManager::GetSingleton()->GetModuleNotHasParent();
		if(mp != 0){LOGD("Module not has parent : %d", mp);}
	}
}

void Android::End(){
	run = false;

	if(_O_Sounds::GetSingleton() != nullptr){_O_Sounds::GetSingleton()->Release();}
	LOGD("sounds module release");
	if(_O_Graphics::GetSingleton() != nullptr){_O_Graphics::GetSingleton()->Release();}
	LOGD("graphic module release");

	_FileIO::Destroy();			LOGD("fileIO module deleted");
	_I_Touch::Destroy();		LOGD("touch module deleted");
	_I_GyroSensor::Destroy();	LOGD("gyro sensor module deleted");
	_O_Sounds::Destroy();		LOGD("sound module deleted");
	_O_Graphics::Destroy();		LOGD("graphic module deleted");
}