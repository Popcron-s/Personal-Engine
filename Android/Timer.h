#pragma once

#include "..\\Common\\Enum.h"
#include "..\\Interface\\Base_Timer.h"

#include <time.h>

class _AndroidTimer : public _Timer{
private:
	_AndroidTimer();
	~_AndroidTimer();

public:
	LIBRARY_TYPE::PLATFORM GetType();
	static _Timer* GetSingleton();

	void Update();
};