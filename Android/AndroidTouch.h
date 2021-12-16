#pragma once

#include "..\\Interface\\Base_Input_Touch.h"

#include "android_native_app_glue.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidTest", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidTest", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "AndroidTest", __VA_ARGS__))

class AndroidTouch : public _I_Touch{
private:
	AndroidTouch(){
		m_Touch->pos = new FLOAT2[10];
		m_Touch->state = new BYTE[10];
	}
	~AndroidTouch(){}

public:
	static int32_t engine_handle_input(struct android_app* app, AInputEvent* event){
		_I_Touch* touch = _I_Touch::GetSingleton();
		if(touch == nullptr){return 0;}
		if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION){
			((AndroidTouch*)touch)->SetTouch(event);
			return 1;
		}
		/*else if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY){

		}*/
		return 0;
	}

	static _I_Touch* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new AndroidTouch;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Android){
			delete m_pSingleton;
			m_pSingleton = new AndroidTouch;
		}
		return m_pSingleton;
	}

	void SetTouch(AInputEvent* ev){
		UINT p = AMotionEvent_getAction(ev) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK;
		p >>= AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		UINT action = AMotionEvent_getAction(ev) & AMOTION_EVENT_ACTION_MASK;
		if(p>10){return;}
		m_Touch->pos[p] = {AMotionEvent_getRawX(ev, p), AMotionEvent_getRawY(ev, p)};
		if(action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN){
			m_Touch->state[p] = 0x82;
		}
		else if(action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP){
			m_Touch->state[p] = 0x02;
		}
		else if(m_Touch->state[p] != 0x82){
			m_Touch->state[p] = 0x80;
		}
	}

	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Android;}
	void Update(){
		m_Touch->size = 0;
		for(UINT i = 0 ; i<10 ; ++i){
			if(m_Touch->state[i] != 0x00){
				++(m_Touch->size);
				if(m_Touch->state[i] & 0x0F){
					m_Touch->state[i] -= 0x01;
				}
			}
			else{
				m_Touch->pos[i] = FLOAT2::zero();
				m_Touch->state[i] = 0x00;
			}
		}
	}
};