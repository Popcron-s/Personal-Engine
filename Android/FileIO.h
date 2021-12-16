#pragma once

#include "..\\Interface\\Base_FileIO.h"

class AndroidFileIO : public _FileIO{
private:
	AndroidFileIO(){}
	~AndroidFileIO(){}

	bool AssetStorageRead(char* name, UINT& size, BYTE*& buf);
	bool InternalStorageRead(char* name, UINT& size, BYTE*& buf);
	bool InternalCashRead(char* name, UINT& size, BYTE*& buf);
	bool ExternalStorageRead(char* name, UINT& size, BYTE*& buf);

	bool InternalStorageWrite(char* name, UINT size, BYTE* buf);
	bool InternalCashWrite(char* name, UINT size, BYTE* buf);
	bool ExternalStorageWrite(char* name, UINT size, BYTE* buf);

	//enum designe
	//0-none
	//1-asset
	//2-internal storage
	//3-internal cash
	//4-external storage

public:
	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Win32;}
	static _FileIO* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new AndroidFileIO;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Android){
			Destroy();
			m_pSingleton = new AndroidFileIO;
		}
		return m_pSingleton;
	}
	bool FileRead(char* name, UINT& size, BYTE*& buf, void* option);

	bool FileWrite(char* name, UINT size, BYTE* buf, void* option);
};