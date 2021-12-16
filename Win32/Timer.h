#pragma once

#include "..\\Common\\Enum.h"
#include "..\\Interface\\Base_Timer.h"

#include <Windows.h>

class _Win32Timer : public _Timer{
private:
	_Win32Timer(){m_Current = GetTickCount();}
	~_Win32Timer(){}

public:
	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Win32;}
	static _Timer* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new _Win32Timer;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Win32){
			Destroy();
			m_pSingleton = new _Win32Timer;
		}
		return m_pSingleton;
	}

	void Update(){
		UINT now = GetTickCount();
		m_Tick = now - m_Current;
		m_Current = GetTickCount();
	}
};