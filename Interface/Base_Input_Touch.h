#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _I_Touch : public InterfaceSingleton<_I_Touch>{
protected:
	TOUCH* m_Touch;

public:
	_I_Touch(){
		m_Touch = new TOUCH;
		*m_Touch = {nullptr, nullptr, 0};
	}
	virtual ~_I_Touch(){
		if(m_Touch->pos != nullptr){
			delete[] m_Touch->pos;
			m_Touch->pos = nullptr;
		}
		if(m_Touch->state != nullptr){
			delete[] m_Touch->state;
			m_Touch->state = nullptr;
		}
		delete m_Touch;
	}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;
	virtual void Update() = 0;

	virtual TOUCH* GetTouch(){return m_Touch;}
};