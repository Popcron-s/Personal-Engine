#pragma once

#include "..\\Interface\\Base_Input_Keyboard.h"
#include "..\\Interface\\Base_Input_Mouse.h"

#include "Windows.h"

class _Win32Keyboard : public _I_Keyboard{
private:
	_Win32Keyboard(){
		m_Keyboard = new KEYBOARD;
		*m_Keyboard = {};
	}
	~_Win32Keyboard(){
		if(m_Keyboard != nullptr){
			delete m_Keyboard;
			m_Keyboard = nullptr;
		}
	}

public:
	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Win32;}
	static _I_Keyboard* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new _Win32Keyboard;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Win32){
			Destroy();
			m_pSingleton = new _Win32Keyboard;
		}
		return m_pSingleton;
	}

	void Update(){
		//Keyboard
		if(GetFocus() == ((Windows*)Windows::GetSingleton())->Gethwnd()){
			for(UINT i = 0 ; i<0x100 ; ++i){
				if(GetAsyncKeyState(i) & 0x8001){
					if(m_Keyboard->KEY[i] & 0x80){
						m_Keyboard->KEY[i] = 0x80;
					}
					else{
						m_Keyboard->KEY[i] = 0x81;
					}
				}
				else{
					if(m_Keyboard->KEY[i] & 0x80){
						m_Keyboard->KEY[i] = 0x01;
					}
					else{
						m_Keyboard->KEY[i] = 0x00;
					}
				}
			}
		}
		else{
			for(UINT i = 0 ; i<0x100 ; ++i){m_Keyboard->KEY[i] = 0x00;}
		}
	}
};

class _Win32Mouse : public _I_Mouse{
private:
	unsigned long cur_time[8];

	_Win32Mouse(){
		m_Mouse = new MOUSE;
		*m_Mouse = {};
	}
	~_Win32Mouse(){
		if(m_Mouse != nullptr){
			delete m_Mouse;
			m_Mouse = nullptr;
		}
	}

public:
	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Win32;}
	static _I_Mouse* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new _Win32Mouse;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Win32){
			Destroy();
			m_pSingleton = new _Win32Mouse;
		}
		return m_pSingleton;
	}

	void Update(){
		//Mouse
		((Windows*)Windows::GetSingleton())->GetzDelta(m_Mouse->z);
		GetCursorPos((LPPOINT)m_Mouse);
		ScreenToClient(((Windows*)Windows::GetSingleton())->Gethwnd(), (LPPOINT)m_Mouse);
		if( 0 > m_Mouse->x || m_Mouse->x > (((Windows*)Windows::GetSingleton())->GetWindowSize().x-10) || 
			0 > m_Mouse->y || m_Mouse->y > (((Windows*)Windows::GetSingleton())->GetWindowSize().y-10) ){
			for(UINT i = 0 ; i<0x08 ; ++i){m_Mouse->button[i] = 0x00;}
		}
		else{
#define MOUSE_CLICK(key,num)												\
	if(GetAsyncKeyState(key) & 0x8001){									\
		if(m_Mouse->button[num] & 0x80){								\
			m_Mouse->button[num] = 0x80;								\
		}																\
		else{															\
			if(GetDoubleClickTime() >= (GetTickCount()-cur_time[num])){	\
				m_Mouse->button[num] = 0x82;							\
				cur_time[num] = GetTickCount();							\
			}															\
			else{														\
				m_Mouse->button[num] = 0x81;							\
				cur_time[num] = GetTickCount();							\
			}															\
		}																\
	}																	\
	else{																\
		if(m_Mouse->button[num] & 0x80){								\
			m_Mouse->button[num] = 0x01;								\
		}																\
		else{															\
			m_Mouse->button[num] = 0x00;								\
		}																\
	}
			MOUSE_CLICK(VK_LBUTTON,0)
			MOUSE_CLICK(VK_RBUTTON,1)
			MOUSE_CLICK(VK_MBUTTON,2)
			MOUSE_CLICK(VK_XBUTTON1,3)
			MOUSE_CLICK(VK_XBUTTON2,4)
#undef MOUSE_CLICK
		}
	}
};