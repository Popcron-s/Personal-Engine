#include "Timer.h"

#include <time.h>

_AndroidTimer::_AndroidTimer(){}
_AndroidTimer::~_AndroidTimer(){}

LIBRARY_TYPE::PLATFORM _AndroidTimer::GetType(){return LIBRARY_TYPE::PLATFORM::Android;}
_Timer* _AndroidTimer::GetSingleton(){
	if(m_pSingleton == nullptr){
		m_pSingleton = new _AndroidTimer;
	}
	else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Android){
		Destroy();
		m_pSingleton = new _AndroidTimer;
	}
	return m_pSingleton;
}

void _AndroidTimer::Update(){
	UINT now = 0;
	m_Tick = now - m_Current;
	m_Current = 0;
}