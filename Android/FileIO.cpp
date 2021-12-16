#include "FileIO.h"
#include "Android.h"

#include <android/asset_manager.h>

#include "stdio.h"

bool AndroidFileIO::AssetStorageRead(char* name, UINT& size, BYTE*& buf){
	AAssetManager* mgr = ((Android*)Android::GetSingleton())->GetAndroidApp()->activity->assetManager;
	AAsset* file = AAssetManager_open(mgr, name, AASSET_MODE_BUFFER);
	if(file == NULL){return false;}
	size = AAsset_getLength(file);
	if(buf != nullptr){
		delete [] buf;
		buf = nullptr;
	}
	buf = new BYTE[size];
	AAsset_read(file, buf, size);
	AAsset_close(file);

	return true;
}
bool AndroidFileIO::InternalStorageRead(char* name, UINT& size, BYTE*& buf){
	if(buf != nullptr){
		delete[] buf;
		buf = nullptr;
		size = 0;
	}

	const char* path = ((Android*)Android::GetSingleton())->GetAndroidApp()->activity->internalDataPath;
	char full_path[260] = "";
	
	INT idx = 0;
	for(INT i = 0 ; path[i] != 0x00 ; ++i){
		full_path[idx++] = path[i];
	}
	for(INT i = 0 ; name[i] != 0x00 ; ++i){
		full_path[idx++] = name[i];
	}

	FILE* file = fopen(full_path, "rb");
	if(file == NULL){return false;}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	buf = new BYTE[size];
	fread(buf, sizeof(BYTE), size, file);

	fclose(file);

	return true;
}
bool AndroidFileIO::InternalCashRead(char* name, UINT& size, BYTE*& buf){return false;}
bool AndroidFileIO::ExternalStorageRead(char* name, UINT& size, BYTE*& buf){return false;}

bool AndroidFileIO::InternalStorageWrite(char* name, UINT size, BYTE* buf){
	if(buf == nullptr){return false;}

	const char* path = ((Android*)Android::GetSingleton())->GetAndroidApp()->activity->internalDataPath;
	char full_path[260] = "";

	INT idx = 0;
	for(INT i = 0 ; path[i] != 0x00 ; ++i){
		full_path[idx++] = path[i];
	}
	for(INT i = 0 ; name[i] != 0x00 ; ++i){
		full_path[idx++] = name[i];
	}

	FILE* file = fopen(full_path, "wb");
	if(file == NULL){return false;}

	fwrite(buf, sizeof(BYTE), size, file);

	fclose(file);

	return true;
}
bool AndroidFileIO::InternalCashWrite(char* name, UINT size, BYTE* buf){return false;}
bool AndroidFileIO::ExternalStorageWrite(char* name, UINT size, BYTE* buf){return false;}

bool AndroidFileIO::FileRead(char* name, UINT& size, BYTE*& buf, void* option){
	UINT type = (uintptr_t)option;
	switch(type){
	case 1:		return AssetStorageRead(name, size, buf);
	case 2:		return InternalStorageRead(name, size, buf);
	case 3:		return InternalCashRead(name, size, buf);
	case 4:		return ExternalStorageRead(name, size, buf);
	default:	return false;
	}
}

bool AndroidFileIO::FileWrite(char* name, UINT size, BYTE* buf, void* option){
	UINT type = (uintptr_t)option;
	switch(type){
	case 2:		return InternalStorageWrite(name, size, buf);
	case 3:		return InternalCashWrite(name, size, buf);
	case 4:		return ExternalStorageWrite(name, size, buf);
	default:	return false;
	}
}