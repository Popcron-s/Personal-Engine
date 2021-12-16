#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _Platform : public InterfaceSingleton<_Platform>{
protected:
	PLATFORM_SETTING* m_Setting;
	void (*MainLoop)() = nullptr;

public:
	_Platform() : m_Setting(nullptr){}
	virtual ~_Platform(){}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;

	virtual void Setting(PLATFORM_SETTING*, void (*)()) = 0;
	virtual PLATFORM_SETTING* GetSetting(){return m_Setting;}

	virtual bool Start() = 0;
	virtual void Run() = 0;
	virtual void End() = 0;
};