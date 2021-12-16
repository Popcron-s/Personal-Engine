#pragma once

#include "..\\Interface\\Base_Input_GyroSensor.h"

#include <android/sensor.h>

class GyroSensor : public _I_GyroSensor{
private:
	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	GyroSensor(){}
	~GyroSensor(){}

public:
	static _I_GyroSensor* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new GyroSensor;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Android){
			delete m_pSingleton;
			m_pSingleton = new GyroSensor;
		}
		return m_pSingleton;
	}

	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Android;}
	void Update(){}

	void Init(android_app* app){
		// 가속도계 모니터링을 준비합니다.
		sensorManager = ASensorManager_getInstance();
		accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
		sensorEventQueue = ASensorManager_createEventQueue(sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);
	}
	void Focusing(){
		// 앱에 포커스가 있으면 가속도계 모니터링을 시작합니다.
		if (accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
			// 초당 60개의 이벤트를 가져올 수 있습니다.
			ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L/60)*1000);
		}
	}
	void Disable(){
		// 앱에서 포커스가 사라지면 가속도계 모니터링이 중지됩니다.
		// 사용하지 않는 동안 배터리를 절약하기 위해 조치입니다.
		if (accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
		}
	}
	void GetEvent(){
		if (accelerometerSensor != NULL) {
			ASensorEvent ev;
			while(ASensorEventQueue_getEvents(sensorEventQueue, &ev, 1) > 0){
				//float3 event.acceleration
				gyro = {ev.acceleration.roll, ev.acceleration.pitch, ev.acceleration.azimuth};
				//strange need fixed
			}
		}
	}
};