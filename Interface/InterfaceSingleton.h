#pragma once

template <class T>
class InterfaceSingleton{
protected:
	static T* m_pSingleton;

	InterfaceSingleton(){}
	~InterfaceSingleton(){}

public:
	static T* GetSingleton(){
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
T* InterfaceSingleton<T>::m_pSingleton = nullptr;