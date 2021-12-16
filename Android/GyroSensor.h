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
		// ���ӵ��� ����͸��� �غ��մϴ�.
		sensorManager = ASensorManager_getInstance();
		accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
		sensorEventQueue = ASensorManager_createEventQueue(sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);
	}
	void Focusing(){
		// �ۿ� ��Ŀ���� ������ ���ӵ��� ����͸��� �����մϴ�.
		if (accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
			// �ʴ� 60���� �̺�Ʈ�� ������ �� �ֽ��ϴ�.
			ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L/60)*1000);
		}
	}
	void Disable(){
		// �ۿ��� ��Ŀ���� ������� ���ӵ��� ����͸��� �����˴ϴ�.
		// ������� �ʴ� ���� ���͸��� �����ϱ� ���� ��ġ�Դϴ�.
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