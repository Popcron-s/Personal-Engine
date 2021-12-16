#pragma once

#include "OpenGL.h"
#include "Windows.h"

#include <iostream>

#include <Windows.h>
#include <gl\GL.h>
//#include <gl\glext.h>
#pragma comment (lib, "OpenGL32.lib")

#define __OPERATOR_MATRIX(m) {m._11, m._12, m._13, m._14, m._21, m._22, m._23, m._24, \
						m._31, m._32, m._33, m._34, m._41, m._42, m._43, m._44}

#define GET_PROC_ADDRESS(pfn, proc) proc = (##pfn)wglGetProcAddress(#proc)

_O_Graphics* OpenGL::GetSingleton(){
	if(m_pSingleton == nullptr){
		m_pSingleton = new OpenGL;
	}
	return m_pSingleton;
}

OpenGL::OpenGL(){if(m_hdc){DeleteDC(m_hdc);}}
OpenGL::~OpenGL(){Release();}

LIBRARY_TYPE::GRAPHIC OpenGL::GetType(){return LIBRARY_TYPE::GRAPHIC::OpenGL;}

bool OpenGL::Init(){
	PIXELFORMATDESCRIPTOR pfd =                             // 픽셀 포맷 설정 구조체
	{
		sizeof(PIXELFORMATDESCRIPTOR),                          // 사이즈
		1,                                                      // 버전 숫자
		PFD_DRAW_TO_WINDOW |                                    // 윈도우를 지원
		PFD_SUPPORT_OPENGL |                                    // GL을 지원
		PFD_DOUBLEBUFFER,                                       // 더블 버퍼링 사용
		PFD_TYPE_RGBA,                                          // RGBA 포맷 사용
		24,                                                     // 색상 비트 수
		8, 0, 8, 0, 8, 0, 										// 컬러 비트 무시
		8, 0,													// Alpha bit
		0,                                                      // Accumulation Buffer 사용하지 않음
		0, 0, 0, 0,                                             // Accumulation Bits 무시
		32,                                                     // 16Bit Z-Buffer 사용 
		0,                                                      // Stencil Buffer 사용하지 않음
		0,                                                      // Auxiliary Buffer 사용하지 않음
		PFD_MAIN_PLANE,                                         // 메인 드로잉 레이어
		0,                                                      // 무시
		0, 0, 0                                                 // Layer Masks 무시
	};
	
	m_hdc = GetDC(((Windows*)Windows::GetSingleton())->Gethwnd());	// DC 획득
	unsigned int PixelFormat = ChoosePixelFormat(m_hdc, &pfd);        // pfd와 DC를 통해 픽셀포맷 설정
	SetPixelFormat(m_hdc, PixelFormat, &pfd);                 // PixelFormat값을 통해 픽셀포맷 설정
	
	m_hrc = wglCreateContext(m_hdc);                        // RC 획득
	wglMakeCurrent(m_hdc, m_hrc);                            // GL측에서 RC 지정

	//GET_PROC_ADDRESS(PFNGLGETINTEGER64VPROC, glGetInteger64v);

	glGetInteger64v = (PFNGLGETINTEGER64VPROC)wglGetProcAddress("glGetInteger64v");

	glBindTextures = (PFNGLBINDTEXTURESPROC)wglGetProcAddress("glBindTextures");

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)wglGetProcAddress("glMapBufferRange");
	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");

	glTexBuffer = (PFNGLTEXBUFFERPROC)wglGetProcAddress("glTexBuffer");
	glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D");
	glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)wglGetProcAddress("glBindImageTexture");

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");

	glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)wglGetProcAddress("glMemoryBarrier");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//depth
	glClearDepth(1.0f);
	//glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_DEPTH_TEST);

	//lighting
	glDisable(GL_LIGHTING);

	//render mode
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set buffers
	// 0 - vertex
	// 1 - obj idx
	// 2 - indexed
	// 3 - obj uniforms
	// 4 - screen uniform
	// 5 - text data buffer
	glGenBuffers(6, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*1024, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UINT)*1024, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UINT)*1024*3, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[3]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0x8000000, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[4]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MATRIX4x4)*3, nullptr, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[5]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0x8000000, nullptr, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return true;
}

bool OpenGL::Clear(){glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); return true;}
bool OpenGL::Start(SCREEN* scr){
	glViewport(scr->x,scr->y,scr->width,scr->height);
	screen = scr;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[4]);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(MATRIX4x4), &(scr->view));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(MATRIX4x4), sizeof(MATRIX4x4), &(scr->proj));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(MATRIX4x4)*2, sizeof(UINT), &(scr->width));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(MATRIX4x4)*2+sizeof(UINT), sizeof(UINT), &(scr->height));

	return true;
}
bool OpenGL::RenderObject(RENDER_OBJECT& r_obj){
	UINT texture_count = 0;
	if(r_obj.shader != nullptr){
		glUseProgram((uintptr_t)r_obj.shader->shader);

		for(INT i = 0 ; i<r_obj.shader->uniform_size ; ++i){
			if(r_obj.shader->uniform[i].location == -1){continue;}
			switch(r_obj.shader->uniform[i].type){
			case _UNIFORM::TYPE::CAMERA_BUF:
				glBindImageTexture(0, (uintptr_t)screen->screen_buffer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32UI);
				glUniform1i(r_obj.shader->uniform[i].location, 0);
				break;
			case _UNIFORM::TYPE::CAMERA_HEAD:
				glBindImageTexture(1, (uintptr_t)screen->screen_listhead, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glUniform1i(r_obj.shader->uniform[i].location, 1);
				break;
			case _UNIFORM::TYPE::TEXTURE:
				glUniform1i(r_obj.shader->uniform[i].location, texture_count++);
				break;
			}
		}

		glBindTextures(0, r_obj.texture_num, r_obj.textures);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, (GLuint)screen->screen_counter);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[3]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[4]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffers[5]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, pos));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, diffuse));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, tex));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, normal));
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);

	glVertexAttribIPointer(4, 1, GL_UNSIGNED_BYTE, sizeof(BYTE), 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	if(r_obj.index_num > 0){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		
		switch(r_obj.type){
		case _MODULE::TYPE::MPoint:		glDrawElements(GL_POINTS,		r_obj.index_num, GL_UNSIGNED_INT, 0); break;
		case _MODULE::TYPE::MLine:		glDrawElements(GL_LINES,		r_obj.index_num, GL_UNSIGNED_INT, 0); break;
		case _MODULE::TYPE::MPolygon:	glDrawElements(GL_TRIANGLES,	r_obj.index_num, GL_UNSIGNED_INT, 0); break;
		default: return false;
		}
	}
	else{
		switch(r_obj.type){
		case _MODULE::TYPE::MPoint:		glDrawArrays(GL_POINTS,			0, r_obj.vertex_num); break;
		case _MODULE::TYPE::MLine:		glDrawArrays(GL_LINE_STRIP,		0, r_obj.vertex_num); break;
		case _MODULE::TYPE::MPolygon:	glDrawArrays(GL_TRIANGLE_STRIP, 0, r_obj.vertex_num); break;
		default: return false;
		}
	}

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	if(r_obj.shader != nullptr){glUseProgram(0);}
	/*
	for(INT i = 0 ; i<texture_count ; ++i){
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	return true;
}
bool OpenGL::RenderText(RENDER_TEXT&){return false;}
bool OpenGL::End(){
	RENDER_OBJECT scr_ren = {};
	scr_ren.type = _MODULE::TYPE::MPolygon;
	scr_ren.shader = screen->shd;
	scr_ren.vertex_num = 4;
	scr_ren.index_num = 6;

	tBufferWrite(0, 0, sizeof(VERTEX)*4, screen->vtx);
	tBufferWrite(2, 0, sizeof(UINT)*6, screen->idx, (void*)0);

	RenderObject(scr_ren);
	
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, (GLuint)screen->screen_counter);
	GLuint* userController = 
		(GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), 
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	userController[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	screen = nullptr;
	return true;
}
bool OpenGL::Present(){
	//std::cout<<"swap buffers"<<std::endl;
	SwapBuffers(m_hdc); 
	return true;
}

bool OpenGL::Release(){
	//glDeleteProgram(programID);

	wglDeleteContext(m_hrc);
	DeleteDC(m_hdc);

	return true;
}

bool OpenGL::GetBackBuffer(UINT width, UINT height, UINT* Color){
	if(Color == nullptr){return false;}
	UINT* t_c = new UINT[width*height];
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,t_c);
	glReadBuffer(GL_BACK);
	for(UINT h = 0 ; h<height ; ++h){
		for(UINT w = 0 ; w<width ; ++w){
			Color[(h*width)+w] = t_c[((height-h-1)*width)+w];
		}
	}
	return true;
}

void* OpenGL::RegistTexture(IMAGE& img, void*& tex){
	glGenTextures(1, (GLuint*)&tex);
	glBindTexture(GL_TEXTURE_2D, (GLuint)tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if(img.alpha_able){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.buf);
	}
	else
	{
		UINT t_size = img.width*img.height*4;
		BYTE* t_buf = new BYTE[t_size];
		for(UINT i = 0 ; i<t_size ; ++i){
			if(i%4 == 3){t_buf[i] = 0xFF;}
			else{t_buf[i] = img.buf[i-(i/4)];}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t_buf);
		delete [] t_buf;
	}
	return tex;
}

void* OpenGL::CreateShader(BYTE* buf, UINT size, _SHADER::TYPE type){
	GLuint shaderID = 0;

	switch(type){
	case _SHADER::TYPE::VERTEX:	shaderID = glCreateShader(GL_VERTEX_SHADER);	break;
	case _SHADER::TYPE::GEOMETRY:	shaderID = glCreateShader(GL_GEOMETRY_SHADER);	break;
	case _SHADER::TYPE::FRAGMENT:	shaderID = glCreateShader(GL_FRAGMENT_SHADER);	break;
	default: return nullptr;
	}
	
	glShaderSource(shaderID, 1, (GLchar**)&buf, (GLint*)&size);
	glCompileShader(shaderID);
	
	GLint r = 0;
	GLint len = 0;
	
	PFNGLGETSHADERIVPROC glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &r);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
	if(len > 0){
		PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
		char* s = new char[len+1];
		glGetShaderInfoLog(shaderID, len, NULL, s);
		std::cout<<s<<std::endl;
		delete[] s;
	}
	
	return (void*)shaderID;
}

void OpenGL::SetShader(SHADER*& shader, void* vtx, void* geo, void* frg, char uni_name[][32], _UNIFORM::TYPE* uni_type, UINT uni_num){
	GLuint prog = glCreateProgram();

	if(vtx != nullptr){glAttachShader(prog, (uintptr_t)vtx);}
	if(geo != nullptr){}
	if(frg != nullptr){glAttachShader(prog, (uintptr_t)frg);}

	glLinkProgram(prog);
	glUseProgram(prog);

	shader = new SHADER;
	shader->shader = (void*)prog;
	shader->uniform_size = uni_num;
	shader->uniform = new UNIFORM[uni_num];
	for(INT i = 0 ; i<uni_num ; ++i){
		shader->uniform[i].location = glGetUniformLocation(prog, uni_name[i]);
		shader->uniform[i].type = uni_type[i];
	}

	glUseProgram(0);

	if(vtx != nullptr){glDetachShader(prog, (uintptr_t)vtx);}
	if(geo != nullptr){}
	if(frg != nullptr){glDetachShader(prog, (uintptr_t)frg);}
}
void OpenGL::ReleaseShader(SHADER*& shader){
	if(shader == nullptr){return;}
	glDeleteProgram((uintptr_t)shader->shader);
	//ReleaseUniform
	delete shader;
	shader = nullptr;
}
bool OpenGL::TempRenderText(VECTOR4*& point, UINT point_num, FLOAT4*& sc, INDEXED& sidx, FLOAT4*& oc, INDEXED& lidx, MATRIX4x4& world, SHADER*& shd){
	return true;
}

void OpenGL::SetScreenBuffer(UINT width, UINT height, void*& buffer, void*& buffer_tex, void*& listhead, void*& counter){
	glGenBuffers(1, (GLuint*)&buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, (GLuint)buffer);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(GLuint)*4*width*height*16, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, (GLuint*)&buffer_tex);
	glBindTexture(GL_TEXTURE_BUFFER, (GLuint)buffer_tex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, (GLuint)buffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, (GLuint*)&listhead);
	glBindTexture(GL_TEXTURE_2D, (GLuint)listhead);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenBuffers(1, (GLuint*)&counter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, (GLuint)counter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	
	return;
}

void OpenGL::tBufferWrite(UINT type, UINT index, UINT size, void* buffer, void* extra){
	switch(type){
	case 0:	
		if(extra == nullptr){
			glBindBuffer(GL_ARRAY_BUFFER, buffers[type]);
			glBufferSubData(GL_ARRAY_BUFFER, index, size, buffer);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			VERTEX* tp_vtx = (VERTEX*)buffer;
			INT t_size = size/sizeof(VERTEX);
			FLOAT ol = *(FLOAT*)extra;

			glBindBuffer(GL_ARRAY_BUFFER, buffers[type]);
			VERTEX* userController = 
				(VERTEX*)glMapBufferRange(GL_ARRAY_BUFFER, index, size, 
										GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			for(INT i = 0 ; i<t_size ; i += 4){
				VERTEX t_vtx = tp_vtx[i+0];
				t_vtx.pos.x -= ol;	t_vtx.pos.y += ol;
				userController[i+0] = t_vtx;

				t_vtx = tp_vtx[i+1];
				t_vtx.pos.x += ol;	t_vtx.pos.y += ol;
				userController[i+1] = t_vtx;

				t_vtx = tp_vtx[i+2];
				t_vtx.pos.x -= ol;	t_vtx.pos.y -= ol;
				userController[i+2] = t_vtx;

				t_vtx = tp_vtx[i+3];
				t_vtx.pos.x += ol;	t_vtx.pos.y -= ol;
				userController[i+3] = t_vtx;
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		return;
	case 1:	{
			glBindBuffer(GL_ARRAY_BUFFER, buffers[type]);
			BYTE* userController = 
				(BYTE*)glMapBufferRange(GL_ARRAY_BUFFER, index, size, 
										  GL_MAP_WRITE_BIT/* | GL_MAP_INVALIDATE_BUFFER_BIT*/);
			for(INT i = 0 ; i<size ; ++i){userController[i] = (BYTE)extra;}
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		return;
	case 2:	{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[type]);
			UINT* ibuffer = (UINT*)buffer;
			UINT* userController = 
				(UINT*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, index, size, 
										GL_MAP_WRITE_BIT/* | GL_MAP_INVALIDATE_BUFFER_BIT*/);
			for(INT i = 0 ; i<size ; ++i){userController[i] = ibuffer[i] + (UINT)extra;}
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		return;
	case 3:	
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[type]);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, index, size, buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		return;
	case 4:	return;	//skip. this screen ssbo
	case 5:	
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[type]);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, index, size, buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		return;
	}
}

#undef __OPERATOR_MATRIX
#undef GET_PROC_ADDRESS