#pragma once

//#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

#include "..\\Interface\\Base_Output_Graphics.h"

#include "android_native_app_glue.h"

class OpenGLES : public _O_Graphics{
private:
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;

	EGLConfig config;

	GLuint buffers[3];

	SCREEN* screen;

	OpenGLES() : 
		display(EGL_NO_DISPLAY), surface(EGL_NO_SURFACE), context(EGL_NO_CONTEXT),
		config(EGL_NO_CONFIG_KHR), screen(nullptr)
	{}
	~OpenGLES(){
		Release();
	}

private:
	void RenderGlyph(INDEXED indexed, _MODULE::TYPE type);

public:
	static _O_Graphics* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new OpenGLES;
		}
		return m_pSingleton;
	}

	LIBRARY_TYPE::GRAPHIC GetType(){return LIBRARY_TYPE::GRAPHIC::OpenGL;}

	bool Init();
	bool Clear();
	bool Start(SCREEN*);
	bool RenderObject(RENDER_OBJECT&);
	bool RenderText(RENDER_TEXT&);
	bool End();
	bool Present();
	bool Release();

	void* CreateShader(BYTE* buf, UINT size, _SHADER::TYPE type);
	void DeleteShader(void* shader);
	void* RegistTexture(IMAGE&, void*& material);
	void RemoveTexture(void* texture);
	bool GetBackBuffer(UINT width, UINT height, UINT* Color);

	//public custom
	void SetShader(SHADER*& program, void* vtx, void* geo, void* frg, char** uni, UINT uni_num);
	void ReleaseShader(SHADER*& shader);
	bool TempRenderText(VECTOR4*& point, UINT point_num, FLOAT4*& sc, INDEXED& sidx, FLOAT4*& oc, INDEXED& lidx, MATRIX4x4& world, SHADER*& shd);

	void Focusing(bool);
};