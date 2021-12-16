#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _I_Keyboard : public InterfaceSingleton<_I_Keyboard>{
protected:
	KEYBOARD* m_Keyboard;

public:
	_I_Keyboard() : m_Keyboard(nullptr){}
	virtual ~_I_Keyboard(){}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;
	virtual void Update() = 0;

	virtual KEYBOARD* GetKeyboard(){return m_Keyboard;}
};