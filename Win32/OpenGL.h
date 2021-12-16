#pragma once

#include "..\\Interface\\Base_Output_Graphics.h"

#include "..\\Common\\Enum.h"
#include "..\\Common\\variable.h"

#include <Windows.h>
#include <gl\\GL.h>
#include "GLHeader\\glext.h"

#pragma comment(lib, "opengl32.lib")

class OpenGL : public _O_Graphics{
private:
	HDC m_hdc;
	HGLRC m_hrc;

	OpenGL();
	~OpenGL();

private:
	GLuint buffers[6];
	SCREEN* screen;

private:
	PFNGLGETINTEGER64VPROC glGetInteger64v;

	PFNGLBINDTEXTURESPROC glBindTextures;

	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBINDBUFFERBASEPROC glBindBufferBase;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLBUFFERSUBDATAPROC glBufferSubData;
	PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
	PFNGLUNMAPBUFFERPROC glUnmapBuffer;

	PFNGLTEXBUFFERPROC glTexBuffer;
	PFNGLTEXSTORAGE2DPROC glTexStorage2D;
	PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;

	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLDETACHSHADERPROC glDetachShader;
	
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLUSEPROGRAMPROC glUseProgram;

	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;

	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
	PFNGLUNIFORM1IPROC glUniform1i;

	PFNGLMEMORYBARRIERPROC glMemoryBarrier;

public:
	static _O_Graphics* GetSingleton();
	
	//void Init();
	//HDC GetBackBuffer();

	LIBRARY_TYPE::GRAPHIC GetType();

	bool Init();
	bool Clear();
	bool Start(SCREEN*);
	bool RenderObject(RENDER_OBJECT&);
	bool RenderText(RENDER_TEXT&);	//unrealized
	bool End();
	bool Present();
	bool Release();

	void* CreateShader(BYTE* buf, UINT size, _SHADER::TYPE type);
	void DeleteShader(void* shader){}
	void* RegistTexture(IMAGE&, void*& material);
	void RemoveTexture(void* texture){}
	bool GetBackBuffer(UINT width, UINT height, UINT* Color);

	//temp
	void SetShader(SHADER*& program, void* vtx, void* geo, void* frg, char uni_name[][32], _UNIFORM::TYPE* uni_type, UINT uni_num);
	void ReleaseShader(SHADER*& shader);
	bool TempRenderText(VECTOR4*& point, UINT point_num, FLOAT4*& sc, INDEXED& sidx, FLOAT4*& oc, INDEXED& lidx, MATRIX4x4& world, SHADER*& shd);
	void SetScreenBuffer(UINT width, UINT height, void*& buffer, void*& buffer_tex, void*& listhead, void*& counter);
	void tBufferWrite(UINT type, UINT index, UINT size, void* buffer, void* extra = nullptr);
};