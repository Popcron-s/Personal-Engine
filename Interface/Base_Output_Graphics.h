#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _O_Graphics : public InterfaceSingleton<_O_Graphics>{
public:
	_O_Graphics(){}
	virtual ~_O_Graphics(){}

	virtual LIBRARY_TYPE::GRAPHIC GetType() = 0;

	virtual bool Init() = 0;
	virtual bool Clear() = 0;
	virtual bool Start(SCREEN*) = 0;
	virtual bool RenderObject(RENDER_OBJECT&) = 0;
	virtual bool RenderText(RENDER_TEXT&) = 0;
	virtual bool End() = 0;
	virtual bool Present() = 0;
	virtual bool Release() = 0;

	virtual void* CreateShader(BYTE* buf, UINT size, _SHADER::TYPE type) = 0;
	virtual void DeleteShader(void* shader) = 0;
	virtual void* RegistTexture(IMAGE&, void*& material) = 0;
	virtual void RemoveTexture(void* texture) = 0;
	virtual bool GetBackBuffer(UINT width, UINT height, UINT* Color) = 0;

	//temp
	virtual void SetShader(SHADER*& program, void* vtx, void* geo, void* frg, char uni_name[][32], _UNIFORM::TYPE* uni_type, UINT uni_num) = 0;
	virtual void ReleaseShader(SHADER*& shader) = 0;
	virtual bool TempRenderText(VECTOR4*& point, UINT point_num, FLOAT4*& sc, INDEXED& sidx, FLOAT4*& oc, INDEXED& lidx, MATRIX4x4& world, SHADER*& shd) = 0;
	virtual void SetScreenBuffer(UINT width, UINT height, void*& buffer, void*& buffer_tex, void*& listhead, void*& counter) = 0;
	//virtual void ReleaseScreenBuffer(void*& buffer, void*& buffer_tex, void*& listhead, void*& counter) = 0;
	virtual void tBufferWrite(UINT type, UINT index, UINT size, void* buffer, void* extra = nullptr) = 0;
};