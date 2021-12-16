#pragma once

#include "..\\Common\\variable.h"

#include "InterfaceSingleton.h"

class _Timer : public InterfaceSingleton<_Timer>{
protected:
	UINT m_Current;
	UINT m_Tick;
	FLOAT m_Scale;

public:
	_Timer() : m_Scale(1.0f){}
	virtual ~_Timer(){}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;
	virtual void Update() = 0;

	void SetScale(FLOAT s){m_Scale = s;}
	FLOAT GetScale(){return m_Scale;}
	UINT GetTick(){return (m_Tick*m_Scale);}
};