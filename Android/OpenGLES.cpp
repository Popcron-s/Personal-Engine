/**************************************************************************
* 현상황
* 간이 최적화
* - 유동적인 uniform을 지원하지 못함
* - shader struct에서 고정 uniform을 일시 저장
* - 고정 uniform
* -- world matrix
* -- view matrix
* -- projection matrix
* -- texture0
* -- added_pos(font)
* -- color(font)
**************************************************************************/

#include "Android.h"
#include "OpenGLES.h"

#include <android/log.h>
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "gles debug", __VA_ARGS__))

bool OpenGLES::Init(){
	android_app* app = ((Android*)Android::GetSingleton())->GetAndroidApp();

	EGLint attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_ALPHA_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};
	EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};
	EGLint error = 0;
	EGLint w = 0, h = 0, format = 0;
	EGLint numConfigs = 0;
	EGLSurface surf = EGL_NO_SURFACE;
	EGLContext cont = EGL_NO_CONTEXT;

	EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	//const GLchar* s = eglQueryString(disp, GL_EXTENSIONS);

	EGLint major, minor;
	eglInitialize(disp, &major, &minor);
	if(eglChooseConfig(disp, attribs, &config, 1, &numConfigs) == EGL_FALSE){
		error = eglGetError();
		LOGD("error choose config : %d", error);
	}
	eglGetConfigAttrib(disp, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	cont = eglCreateContext(disp, config, EGL_NO_CONTEXT, contextAttribs);

	if(cont == EGL_NO_CONTEXT){
		error = eglGetError();
		LOGD("gles3 load false : code %d", error);
		contextAttribs[1] = 2;
		cont = eglCreateContext(disp, config, EGL_NO_CONTEXT, contextAttribs);

		if(cont == EGL_NO_CONTEXT){
			error = eglGetError();
			LOGD("gles2 load false : code %d", error);
		}
		else{LOGD("gles2 load");}
	}
	else{LOGD("gles3 load");}

	surf = eglCreateWindowSurface(disp, config, app->window, NULL);

	eglQuerySurface(disp, surf, EGL_WIDTH, &w);
	eglQuerySurface(disp, surf, EGL_HEIGHT, &h);

	LOGD("window size : %d / %d", w, h);

	display = disp;
	context = cont;
	surface = surf;
	
	((Android*)Android::GetSingleton())->SetWindowSize(w, h);

	LOGD("display : %d", display);
	LOGD("context : %d", context);
	LOGD("surface : %d", surface);

	if (eglMakeCurrent(disp, surf, surf, cont) == EGL_FALSE) {
		LOGD("Unable to eglMakeCurrent");
		return false;
	}

	// GL 상태를 초기화합니다.
	glDisable(GL_DITHER);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	//glEnable(GL_CULL_FACE);
	//glShadeModel(GL_SMOOTH);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0f);
	glClearStencil(0x00);

	glGenBuffers(3, buffers);

	return true;
}

bool OpenGLES::Clear(){
	if(surface == EGL_NO_SURFACE){return false;}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	return true;
}
bool OpenGLES::Start(SCREEN* scr){
	glViewport(scr->x,scr->y,scr->width,scr->height);
	screen = scr;
	return true;
}
bool OpenGLES::RenderObject(RENDER_OBJECT& r_obj){
	if(r_obj.shader != nullptr){
		glUseProgram((uintptr_t)r_obj.shader->shader);

		glUniformMatrix4fv(r_obj.shader->UniformLocation[0], 1, GL_FALSE, &(r_obj.world._11));
		glUniformMatrix4fv(r_obj.shader->UniformLocation[1], 1, GL_FALSE, &(screen->view._11));
		glUniformMatrix4fv(r_obj.shader->UniformLocation[2], 1, GL_FALSE, &(screen->proj._11));

		if(r_obj.material != nullptr){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (uintptr_t)r_obj.material->diffuse);
			glUniform1i(r_obj.shader->UniformLocation[3], 0);
		}
		else{
			glUniform1i(r_obj.shader->UniformLocation[3], -1);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*r_obj.vtx_num, r_obj.vtx, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, pos));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, diffuse));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, tex));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const void*)offsetof(VERTEX, normal));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	if(r_obj.indexed != nullptr){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UINT)*r_obj.indexed->num, r_obj.indexed->index, GL_STREAM_DRAW);

		switch(r_obj.type){
		case _MODULE::TYPE::MPoint:		glDrawElements(GL_POINTS,		r_obj.indexed->num, GL_UNSIGNED_INT, 0); break;
		case _MODULE::TYPE::MLine:		glDrawElements(GL_LINES,		r_obj.indexed->num, GL_UNSIGNED_INT, 0); break;
		case _MODULE::TYPE::MPolygon:	glDrawElements(GL_TRIANGLES,	r_obj.indexed->num, GL_UNSIGNED_INT, 0); break;
		default: return false;
		}
	}
	else{
		switch(r_obj.type){
		case _MODULE::TYPE::MPoint:		glDrawArrays(GL_POINTS,			0, r_obj.vtx_num); break;
		case _MODULE::TYPE::MLine:		glDrawArrays(GL_LINE_STRIP,		0, r_obj.vtx_num); break;
		case _MODULE::TYPE::MPolygon:	glDrawArrays(GL_TRIANGLE_STRIP, 0, r_obj.vtx_num); break;
		default: return false;
		}
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	if(r_obj.shader != nullptr){glUseProgram(0);}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool OpenGLES::TempRenderText(VECTOR4*& point, UINT point_num, 
							  FLOAT4*& sc, INDEXED& sidx, 
							  FLOAT4*& oc, INDEXED& lidx, 
							  MATRIX4x4& world, SHADER*& shd){
	if(shd != nullptr){
		glUseProgram((uintptr_t)shd->shader);

		glUniformMatrix4fv(shd->UniformLocation[0], 1, GL_FALSE, &(world._11));
		glUniformMatrix4fv(shd->UniformLocation[1], 1, GL_FALSE, &(screen->view._11));
		glUniformMatrix4fv(shd->UniformLocation[2], 1, GL_FALSE, &(screen->proj._11));
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VECTOR4)*point_num, point, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VECTOR4), 0);
	glEnableVertexAttribArray(0);

	//draw text
	if(lidx.num != 0){
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT4)*point_num, oc, GL_DYNAMIC_DRAW);
		
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FLOAT4), 0);
		glEnableVertexAttribArray(1);

		FLOAT w = ((Android*)Android::GetSingleton())->GetWindowSizeX();
		FLOAT h = ((Android*)Android::GetSingleton())->GetWindowSizeY();
		FLOAT thick = 0.0f;
		FLOAT thin = 0.0f;
		if(w/9.0f < h/16.0f){thick = w/225.0f;	thin = w/900.0f;}
		else{thick = h/400.0f;;	thin = w/1600.0f;}
		if(sidx.num != 0){glLineWidth(thick);}
		else{glLineWidth(thin);}
		RenderGlyph(lidx, _MODULE::TYPE::MLine);
		glLineWidth(thin);

		glDisableVertexAttribArray(1);
	}
	if(sidx.num != 0){
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT4)*point_num, sc, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VECTOR4), 0);
		glEnableVertexAttribArray(1);

		glEnable(GL_STENCIL_TEST);

		glStencilMask(0xFF);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glStencilFunc(GL_NOTEQUAL, 0x01, 0xFF);
		glStencilOp(GL_ZERO, GL_REPLACE, GL_REPLACE);
		RenderGlyph(sidx, _MODULE::TYPE::MPolygon);

		glStencilMask(0xFF);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilFunc(GL_EQUAL, 0x01, 0xFF);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		RenderGlyph(sidx, _MODULE::TYPE::MPolygon);

		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		glDisable(GL_STENCIL_TEST);

		glDisableVertexAttribArray(1);
	}

	glDisableVertexAttribArray(0);

	if(shd != nullptr){glUseProgram(0);}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

void OpenGLES::RenderGlyph(INDEXED indexed, _MODULE::TYPE type){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UINT)*indexed.num, indexed.index, GL_STREAM_DRAW);

	if(indexed.num != 0){
		switch(type){
		case _MODULE::TYPE::MPoint:		glDrawElements(GL_POINTS,		indexed.num, GL_UNSIGNED_INT, 0);	break;
		case _MODULE::TYPE::MLine:		glDrawElements(GL_LINES,		indexed.num, GL_UNSIGNED_INT, 0);	break;
		case _MODULE::TYPE::MPolygon:	glDrawElements(GL_TRIANGLES,	indexed.num, GL_UNSIGNED_INT, 0);	break;
		}
	}
}
bool OpenGLES::RenderText(RENDER_TEXT& r_obj){return false;}
bool OpenGLES::End(){
	//glFlush();
	screen = nullptr;
	return true;
}
bool OpenGLES::Present(){
	if(surface == EGL_NO_SURFACE){return false;}
	eglSwapBuffers(display, surface);
	return true;
}

bool OpenGLES::Release(){
	glDeleteBuffers(3, buffers);
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (surface != EGL_NO_SURFACE) {eglDestroySurface(display, surface);}
		eglTerminate(display);
	}
	if (context != EGL_NO_CONTEXT) {eglDestroyContext(display, context);}
	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
	surface = EGL_NO_SURFACE;

	return true;
}

void OpenGLES::Focusing(bool b){
	if(display == EGL_NO_DISPLAY){return;}
	if(b){
		android_app* app = ((Android*)Android::GetSingleton())->GetAndroidApp();

		surface = eglCreateWindowSurface(display, config, app->window, NULL);
		eglMakeCurrent(display, surface, surface, context);
	}
	else{
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (surface != EGL_NO_SURFACE){eglDestroySurface(display, surface);}
		surface = EGL_NO_SURFACE;
	}
}

void* OpenGLES::CreateShader(BYTE* buf, UINT size, _SHADER::TYPE type){
	GLuint shaderID = 0;

	switch(type){
	case _SHADER::TYPE::VERTEX:	shaderID = glCreateShader(GL_VERTEX_SHADER);	break;
		//case _SHADER::TYPE::GEOMETRY:	shaderID = glCreateShader(GL_GEOMETRY_SHADER);	break;
	case _SHADER::TYPE::FRAGMENT:	shaderID = glCreateShader(GL_FRAGMENT_SHADER);	break;
	default: return nullptr;
	}

	glShaderSource(shaderID, 1, (GLchar**)&buf, (GLint*)&size);
	glCompileShader(shaderID);

	GLint r = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &r);

	GLint len = 0;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
	if(len > 0){
		GLchar* s = new GLchar[len+1];
		glGetShaderInfoLog(shaderID, len, NULL, s);
		LOGD(s);
		delete[] s;
	}

	return (void*)shaderID;
}
void OpenGLES::DeleteShader(void* shader){
	if(shader == nullptr){return;}
	GLuint shd = (uintptr_t)shader;
	glDeleteShader(shd);
}
void* OpenGLES::RegistTexture(IMAGE& img, void*& texture){
	GLuint tex = (uintptr_t)texture;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR GL_NEAREST
																	   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.buf);
	}
	texture = (void*)tex;
	return texture;
}
void OpenGLES::RemoveTexture(void* texture){
	GLuint tex = (uintptr_t)texture;
	glDeleteTextures(1, &tex);
}
bool OpenGLES::GetBackBuffer(UINT width, UINT height, UINT* Color){return false;}

void OpenGLES::SetShader(SHADER*& shader, void* vtx, void* geo, void* frg, 
						 char** uni, UINT uni_num){
	GLuint prog = glCreateProgram();

	if(vtx != nullptr){glAttachShader(prog, (uintptr_t)vtx);}
	if(geo != nullptr){}
	if(frg != nullptr){glAttachShader(prog, (uintptr_t)frg);}

	glLinkProgram(prog);
	glUseProgram(prog);

	//temp uniform set
	//precondition
	//uni_num == 3 -> world, view, proj
	//uni_num == 4 -> world, view, proj, texture0

	shader = new SHADER;
	shader->shader = (void*)prog;
	shader->UniformLocation[0] = glGetUniformLocation(prog, "world");
	shader->UniformLocation[1] = glGetUniformLocation(prog, "view");
	shader->UniformLocation[2] = glGetUniformLocation(prog, "proj");
	if(uni_num == 4){shader->UniformLocation[3] = glGetUniformLocation(prog, "texture0");}
	else{shader->UniformLocation[3] = -1;}

	glUseProgram(0);

	if(vtx != nullptr){glDetachShader(prog, (uintptr_t)vtx);}
	if(geo != nullptr){}
	if(frg != nullptr){glDetachShader(prog, (uintptr_t)frg);}
}

void OpenGLES::ReleaseShader(SHADER*& shader){
	if(shader == nullptr){return;}
	glDeleteProgram((uintptr_t)shader->shader);
	delete shader;
	shader = nullptr;
}