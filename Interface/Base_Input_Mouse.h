#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _I_Mouse : public InterfaceSingleton<_I_Mouse>{
protected:
	MOUSE* m_Mouse;

public:
	_I_Mouse() : m_Mouse(nullptr){}
	virtual ~_I_Mouse(){}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;
	virtual void Update() = 0;

	virtual MOUSE* GetMouse(){return m_Mouse;}
};