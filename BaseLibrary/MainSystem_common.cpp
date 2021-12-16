#include "MainSystem.h"

//Object Manager
#include "ObjectManager.h"
#include "Object.h"

//Resource Manager
#include "ResourceManager.h"

//IO-System
#include "InputSystem.h"
#include "Renderer.h"

//phsics
#include "Collision.h"

//Font
#include "Font.h"

#include "..\\Interface\\Base_Platform.h"
#include "..\\Interface\\Base_FileIO.h"
#include "..\\Interface\\Base_Output_Graphics.h"
#include "..\\Interface\\Base_Output_Sounds.h"
#include "..\\Interface\\Base_Timer.h"
#include "..\\Coder\\Coder.h"

//each platform each set
/*void MainSystem::Start(PLATFORM_SETTING* setting){
	screen_x = setting->x;
	screen_y = setting->y;
	_Platform::GetSingleton()->Setting(setting, MainSystem::MainLoop);
	_Platform::GetSingleton()->Start();
}*/
void PCS::Run(){
	_Platform::GetSingleton()->Run();
}
void PCS::End(){
	Renderer::Destroy();
	ObjectManager::Destroy();
	ResourceManager::Destroy();
	_Platform::GetSingleton()->End();
	_Platform::Destroy();
}

//#include <time.h>
//#include "..\\Android\\android_native_app_glue.h"
//#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "threaded_app", __VA_ARGS__))

//#define time_check \
//clock_gettime(CLOCK_MONOTONIC, &time);	\
//prev_tick = cur_tick;	\
//cur_tick = (time.tv_sec*1000.0f)+(time.tv_nsec/1000000.0f);

void PCS::MainLoop(){
	//struct timespec time;
	//UINT64 prev_tick = 0;
	//UINT64 cur_tick = 0;
	//time_check;
	if(_Timer::GetSingleton() != nullptr){_Timer::GetSingleton()->Update();}	//timer update
	InputSystem::GetSingleton()->update();	//input check
																				//event dispatch
	//time_check;
	ObjectManager::GetSingleton()->update();	//object update
	//time_check;
	//LOGD("OBJManager tick : %d", cur_tick-prev_tick);
												//physics update
	Collision::GetSingleton()->update();	//collision update
	//time_check;
	Renderer::GetSingleton()->Render();	//render
	//time_check;
	//LOGD("Renderer tick : %d", cur_tick-prev_tick);
}

UINT PCS::GetTick(){
	if(_Timer::GetSingleton() == nullptr){return 0;} 
	return _Timer::GetSingleton()->GetTick();
}
KEYBOARD* PCS::GetKeyboard(){return InputSystem::GetSingleton()->GetKeyboard();}
MOUSE* PCS::GetMouse(){return InputSystem::GetSingleton()->GetMouse();}
TOUCH* PCS::GetTouch(){return InputSystem::GetSingleton()->GetTouch();}

UINT PCS::GetScreenX(){
	if(_Platform::GetSingleton() == nullptr){return 0;}
	return _Platform::GetSingleton()->GetSetting()->x;
}
UINT PCS::GetScreenY(){
	if(_Platform::GetSingleton() == nullptr){return 0;}
	return _Platform::GetSingleton()->GetSetting()->y;
}

FLOAT3 PCS::Click(SCREEN scr, FLOAT z){
	MATRIX4x4 r = MATCAL::ReverseViewProjectionMatrix(scr.view, scr.proj);
	FLOAT w = scr.width - scr.x;
	FLOAT h = scr.height - scr.y;
	FLOAT x = PCS::GetMouse()->x - w/2.0f;
	FLOAT y = h/2.0f - PCS::GetMouse()->y;
	x /= w/2.0f;
	y /= h/2.0f;
	FLOAT4 n = {x, y, -1.0f, 1.0f};
	FLOAT4 f = {x, y, 1.0f, 1.0f};

	n = VECCAL::MultipleMatrix(n, r);
	f = VECCAL::MultipleMatrix(f, r);

	n /= n.w;
	f /= f.w;

	x = (((z-f.z)/(n.z-f.z))*(n.x-f.x))+f.x;
	y = (((z-f.z)/(n.z-f.z))*(n.y-f.y))+f.y;

	//if n.z>z z>f.z out range

	return {x, y, z};
}
FLOAT3 PCS::Click(SCREEN scr, FLOAT _x, FLOAT _y, FLOAT z){
	MATRIX4x4 r = MATCAL::ReverseViewProjectionMatrix(scr.view, scr.proj);
	FLOAT w = scr.width - scr.x;
	FLOAT h = scr.height - scr.y;
	FLOAT x = _x - w/2.0f;
	FLOAT y = h/2.0f - _y;
	x /= w/2.0f;
	y /= h/2.0f;
	FLOAT4 n = {x, y, -1.0f, 1.0f};
	FLOAT4 f = {x, y, 1.0f, 1.0f};

	n = VECCAL::MultipleMatrix(n, r);
	f = VECCAL::MultipleMatrix(f, r);

	n /= n.w;
	f /= f.w;

	x = (((z-f.z)/(n.z-f.z))*(n.x-f.x))+f.x;
	y = (((z-f.z)/(n.z-f.z))*(n.y-f.y))+f.y;

	//if n.z>z z>f.z out range

	return {x, y, z};
}
void PCS::Raycast(SCREEN scr, FLOAT _x, FLOAT _y, FLOAT3& st, FLOAT3& ed){
	MATRIX4x4 r = MATCAL::ReverseViewProjectionMatrix(scr.view, scr.proj);
	FLOAT w = scr.width - scr.x;
	FLOAT h = scr.height - scr.y;
	FLOAT x = _x - w/2.0f;
	FLOAT y = h/2.0f - _y;
	x /= w/2.0f;
	y /= h/2.0f;
	FLOAT4 n = {x, y, -1.0f, 1.0f};
	FLOAT4 f = {x, y, 1.0f, 1.0f};

	n = VECCAL::MultipleMatrix(n, r);
	f = VECCAL::MultipleMatrix(f, r);

	n /= n.w;
	f /= f.w;

	st = {n.x, n.y, n.z};
	ed = {f.x, f.y, f.z};
}
/*
void PCS::Click(Camera* cam, Collider* cursor){

}*/

FLOAT3 PCS::tCollision2D(Collider* c1, Collider* c2){
	return Collision::GetSingleton()->Collision2D(c1, c2);
}
bool PCS::tCollision2D(Collider* c1, Collider* c2, bool epa_check){
	return Collision::GetSingleton()->Collision2D(c1, c2, epa_check);
}

FLOAT3 PCS::tCollision2D(Collider* c1, FLOAT3* p2, UINT n2){
	return Collision::GetSingleton()->Collision2D(c1, p2, n2);
}
bool PCS::tCollision2D(Collider* c1, FLOAT3* p2, UINT n2, bool epa_check){
	return Collision::GetSingleton()->Collision2D(c1, p2, n2, epa_check);
}

FLOAT3 PCS::tCollision2D(FLOAT3* p1, UINT n1, FLOAT3* p2, UINT n2){
	return Collision::GetSingleton()->Collision2D(p1, n1, p2, n2);
}
bool PCS::tCollision2D(FLOAT3* p1, UINT n1, FLOAT3* p2, UINT n2, bool epa_check){
	return Collision::GetSingleton()->Collision2D(p1, n1, p2, n2, epa_check);
}

bool PCS::LoadFile(char name[260], BYTE*& buf, UINT& size){
	if(_FileIO::GetSingleton() == nullptr){return false;}
	bool b = _FileIO::GetSingleton()->FileRead(name, size, buf, (void*)1);
	return b;
}

bool PCS::LoadText(BYTE* buf, UINT size, wchar_t*& text, UINT& len){
	return TXTDecoder(buf, size, text, len);
}

bool PCS::AddResource(char key[260], void* data, RESOURCEFORMAT::TYPE type){
	return ResourceManager::GetSingleton()->InsertResource(key, data, type);
}
bool PCS::DeleteResource(char key[260]){
	return ResourceManager::GetSingleton()->RemoveResource(key);
}
void* PCS::GetResource(char key[260]){
	return ResourceManager::GetSingleton()->GetResource(key);
}

void* PCS::CreateShader(BYTE* buf, UINT size, _SHADER::TYPE type){
	return _O_Graphics::GetSingleton()->CreateShader(buf, size, type);
}
bool PCS::DeleteShader(void* shader){
	_O_Graphics::GetSingleton()->DeleteShader(shader);
	return true;
}

void* PCS::RegistTexture(BYTE* buf, UINT size){
	void* tex = nullptr;

	IMAGE* img = new IMAGE;
	*img = {};
	img->buf = nullptr;
	if(BMPDecoder(buf, size, *img) || PNGDecoder(buf, size, *img)){
		_O_Graphics::GetSingleton()->RegistTexture(*img, tex);
	}
	if(img->buf != nullptr){
		delete[] img->buf;
	}
	delete img;

	return tex;
}
bool PCS::ReleaseTexture(void* tex){
	_O_Graphics::GetSingleton()->RemoveTexture(tex);
	return true;
}

void* PCS::CreateSoundGraph(BYTE* buf, UINT size){
	void* g = nullptr;

	SOUND* s = new SOUND;
	*s = {};
	s->buf = nullptr;
	if(WAVDecoder(buf, size, *s) || OGGDecoder(buf, size, *s)){
		_O_Sounds::GetSingleton()->CreateGraph(g, *s);
	}
	if(s->buf != nullptr){
		delete[] s->buf;
	}
	delete s;

	return g;
}
bool PCS::DeleteSoundGraph(void*& g){
	return _O_Sounds::GetSingleton()->DeleteGraph(g);
}

void* PCS::CreateFont(BYTE* buf, UINT size){
	Font* font = nullptr;
	font = new Font;
	bool b = font->CreateFont(buf, size);
	if(!b){
		delete font;
		font = nullptr;
	}
	return font;
}
bool PCS::DeleteFont(void* font){
	Font* f = (Font*)font;
	delete f;
	return true;
}

bool PCS::WriteLocalStorage(char fn[260], BYTE* buf, UINT size){
	if(_FileIO::GetSingleton() == nullptr){return false;}
	return _FileIO::GetSingleton()->FileWrite(fn, size, buf, (void*)2);
}
bool PCS::ReadLocalStorage(char fn[260], BYTE*& buf, UINT& size){
	if(_FileIO::GetSingleton() == nullptr){return false;}
	return _FileIO::GetSingleton()->FileRead(fn, size, buf, (void*)2);
}

bool PCS::GetZIPOffsetArray(BYTE* buf, UINT size, UINT*& LH_offset, UINT& LH_size){
	return ZIPDecoder(buf, size, LH_offset, LH_size);
}
bool PCS::DecompressionZIPData(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name, BYTE*& decomp_buf, UINT& decomp_size){
	return Decompression(zip_buf, zip_size, offset, file_name, decomp_buf, decomp_size);
}
bool PCS::GetFilenameZIPData(BYTE* zip_buf, UINT zip_size, UINT offset, char* file_name){
	return GetFilename(zip_buf, zip_size, offset, file_name);
}

bool PCS::SetShader(SHADER*& shd, void* v, void* g, void* f, char uni_name[][32], _UNIFORM::TYPE* uni_type, UINT uni_num){
	if(_O_Graphics::GetSingleton() == nullptr){return false;}
	_O_Graphics::GetSingleton()->SetShader(shd, v, g, f, uni_name, uni_type, uni_num);
	return true;
}
bool PCS::ReleaseShader(SHADER*& shd){
	if(_O_Graphics::GetSingleton() == nullptr){return false;}
	_O_Graphics::GetSingleton()->ReleaseShader(shd);
	return true;
}

FLOAT3 PCS::tGetSensorRotation(){return InputSystem::GetSingleton()->GetGyroSensor();}