#pragma once

#include "..\\Interface\\Base_FileIO.h"

#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys\\stat.h>

class _Win32FileIO : public _FileIO{
private:
	_Win32FileIO(){}
	~_Win32FileIO(){}

public:
	LIBRARY_TYPE::PLATFORM GetType(){return LIBRARY_TYPE::PLATFORM::Win32;}
	static _FileIO* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new _Win32FileIO;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::PLATFORM::Win32){
			Destroy();
			m_pSingleton = new _Win32FileIO;
		}
		return m_pSingleton;
	}
	bool FileRead(char* name, UINT& size, BYTE*& buf, void* option){
		if(_access_s(name, 0x04) != 0x00){return false;}

		int handle = 0;
		_sopen_s(&handle, name, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD);
		size = _filelength(handle);
		if(buf != nullptr){
			delete [] buf;
			buf = nullptr;
		}
		buf = new BYTE[size];
		_read(handle, buf, size);
		_close(handle);
		return true;
	}

	bool FileWrite(char* name, UINT size, BYTE* buf, void* option){
		if(_access_s(name, 0x00) == 0x00){
			if(_access_s(name, 0x02) != 0x00){return false;}
		}

		int handle = 0;
		_sopen_s(&handle, name, _O_WRONLY | _O_BINARY | _O_CREAT, _SH_DENYRD, _S_IWRITE);
		_write(handle, buf, size);
		_close(handle);

		return true;
	}
};