#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _I_GyroSensor : public InterfaceSingleton<_I_GyroSensor>{
protected:
	FLOAT3 gyro;

public:
	_I_GyroSensor() : gyro({}){}
	virtual ~_I_GyroSensor(){}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;
	virtual void Update() = 0;

	virtual FLOAT3 GetGyroSensor(){return gyro;}
};