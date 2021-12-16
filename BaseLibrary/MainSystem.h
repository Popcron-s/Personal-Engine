#pragma once

#include "..\\Common\\variable.h"

#ifdef WIN32
#ifdef ENGINE_WIN32_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif

class OBJECT;
class Collider;

namespace PCS{
	DLL_API void Start(PLATFORM_SETTING* setting, void (*)());
	DLL_API void Run();
	DLL_API void End();

	DLL_API void MainLoop();

	DLL_API UINT GetTick();

	DLL_API KEYBOARD* GetKeyboard();
	DLL_API MOUSE* GetMouse();
	DLL_API TOUCH* GetTouch();
	//android-only
	DLL_API void* GetAppData();

	DLL_API UINT GetScreenX();
	DLL_API UINT GetScreenY();

	DLL_API FLOAT3 Click(SCREEN scr, FLOAT z);
	DLL_API FLOAT3 Click(SCREEN scr, FLOAT x, FLOAT y, FLOAT z);
	DLL_API void Raycast(SCREEN scr, FLOAT _x, FLOAT _y, FLOAT3& st, FLOAT3& ed);

	DLL_API void ExitProgram();

	//epa check is non-uses value.
	DLL_API FLOAT3 tCollision2D(Collider* c1, Collider* c2);
	DLL_API bool tCollision2D(Collider* c1, Collider* c2, bool epa_check);

	DLL_API FLOAT3 tCollision2D(Collider* c1, FLOAT3* p2, UINT n2);
	DLL_API bool tCollision2D(Collider* c1, FLOAT3* p2, UINT n2, bool epa_check);

	DLL_API FLOAT3 tCollision2D(FLOAT3* p1, UINT n1, FLOAT3* p2, UINT n2);
	DLL_API bool tCollision2D(FLOAT3* p1, UINT n1, FLOAT3* p2, UINT n2, bool epa_check);

	DLL_API bool AddResource(char key[260], void* data, RESOURCEFORMAT::TYPE type);
	DLL_API bool DeleteResource(char key[260]);
	DLL_API void* GetResource(char key[260]);

	DLL_API bool LoadFile(char name[260], BYTE*& buf, UINT& size);
	DLL_API bool LoadText(BYTE*, UINT, wchar_t*&, UINT&);

	DLL_API void* CreateShader(BYTE*, UINT, _SHADER::TYPE);
	DLL_API bool DeleteShader(void*);
	DLL_API void* RegistTexture(BYTE*, UINT);
	DLL_API bool ReleaseTexture(void*);

	DLL_API void* CreateSoundGraph(BYTE*, UINT);
	DLL_API bool DeleteSoundGraph(void*&);

	DLL_API void* CreateFont(BYTE*, UINT);
	DLL_API bool DeleteFont(void*);

	DLL_API bool WriteLocalStorage(char[260], BYTE*, UINT);
	DLL_API bool ReadLocalStorage(char[260], BYTE*&, UINT&);

	DLL_API bool GetZIPOffsetArray(BYTE* buf, UINT size, UINT*& LH_offset, UINT& LH_size);
	DLL_API bool DecompressionZIPData(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name, BYTE*& decomp_buf, UINT& decomp_size);
	DLL_API bool GetFilenameZIPData(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name);

	DLL_API bool SetShader(SHADER*& shd, void* v, void* g, void* f, char uni_name[][32], _UNIFORM::TYPE* uni_type, UINT uni_num);
	DLL_API bool ReleaseShader(SHADER*& shd);

	DLL_API FLOAT3 tGetSensorRotation();

	DLL_API BYTE tGetKeyState_Back();
};