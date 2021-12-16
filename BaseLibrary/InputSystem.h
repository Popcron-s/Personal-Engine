#pragma once

#include "..\\Interface\\Base_Input_Keyboard.h"
#include "..\\Interface\\Base_Input_Mouse.h"
#include "..\\Interface\\Base_Input_Touch.h"

#include "..\\Interface\\Base_Input_GyroSensor.h"

#include "..\\Common\\Singleton.h"

class InputSystem : public Singleton<InputSystem>{
private:

public:
	void update(){
		if(_I_Keyboard::GetSingleton() != nullptr){_I_Keyboard::GetSingleton()->Update();}
		if(_I_Mouse::GetSingleton() != nullptr){_I_Mouse::GetSingleton()->Update();}
		if(_I_Touch::GetSingleton() != nullptr){_I_Touch::GetSingleton()->Update();}

		if(_I_GyroSensor::GetSingleton() != nullptr){_I_GyroSensor::GetSingleton()->Update();}
	}

	KEYBOARD* GetKeyboard(){
		if(_I_Keyboard::GetSingleton() == nullptr){return nullptr;}
		return _I_Keyboard::GetSingleton()->GetKeyboard();
	}
	MOUSE* GetMouse(){
		if(_I_Mouse::GetSingleton() == nullptr){return nullptr;}
		return _I_Mouse::GetSingleton()->GetMouse();
	}
	TOUCH* GetTouch(){
		if(_I_Touch::GetSingleton() == nullptr){return nullptr;}
		return _I_Touch::GetSingleton()->GetTouch();
	}

	FLOAT3 GetGyroSensor(){
		if(_I_GyroSensor::GetSingleton() == nullptr){return FLOAT3::zero();}
		return _I_GyroSensor::GetSingleton()->GetGyroSensor();
	}
};