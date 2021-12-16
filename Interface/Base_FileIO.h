#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _FileIO : public InterfaceSingleton<_FileIO>{
public:
	_FileIO(){}
	virtual ~_FileIO(){}

	virtual LIBRARY_TYPE::PLATFORM GetType() = 0;
	virtual bool FileRead(char* name, UINT& size, BYTE*& buf, void* option) = 0;
	virtual bool FileWrite(char* name, UINT size, BYTE* buf, void* option) = 0;
};
