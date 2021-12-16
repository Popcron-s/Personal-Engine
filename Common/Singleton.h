#pragma once

template <class T>
class Singleton{
protected:
	static T* m_pSingleton;

	Singleton(){}
	~Singleton(){}

public:
	static T* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new T;
		}
		return m_pSingleton;
	}
	static void Destroy(){
		if(m_pSingleton != nullptr){
			delete m_pSingleton;
			m_pSingleton = nullptr;
		}
	}
};

template <class T>
T* Singleton<T>::m_pSingleton = nullptr;