#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

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
class Font;

class DLL_API Joint{
public:
	UINT id;
	Joint* parent;

	FLOAT3 position;
	FLOAT3 rotation;
	FLOAT3 scale;

	//FLOAT2 shear_x
	//FLOAT2 shear_y
	//FLOAT2 shear_z
	
	MATRIX4x4 world;

	INT calcul;	//0,1 - abutment / 2 - stat : init / 3 - fixed

	Joint();
	~Joint();

	void SetWorld(INT cal_state);
};

class DLL_API Module{
private:
	Joint* joint;
	bool active;

public:
	OBJECT* parent;
	UINT32 id;

public:
	Module();
	virtual ~Module();

	bool IsActive();
	void Active();
	void Inactive();

	Joint* GetJoint();
	void CreateJoint();
	void DeleteJoint();

	virtual _MODULE::TYPE Type() = 0;
};

class DLL_API Camera : public Module{
private:
	FLOAT3 eye, lookat, up;
	UINT x, y, w, h;

	FLOAT fovy;
	FLOAT2 z_range;
	_PROJECTION::TYPE type;

	MATRIX4x4 view;
	MATRIX4x4 proj;

	VERTEX vtx[4];
	UINT idx[6];	//indexed
	SHADER* shd;

	void* screen_buffer;
	void* screen_buffer_texture;
	void* screen_listhead;
	void* screen_counter;

public:
	Camera();
	~Camera();

	_MODULE::TYPE Type();

	SCREEN GetScreen();
	void SetScreen();

	void SetScreenRange(UINT _x, UINT _y, UINT _w, UINT _h);
	void SetViewMatrix(FLOAT3 _eye, FLOAT3 _lookat, FLOAT3 _up);
	void SetPerspectiveProjMatrix(FLOAT _fovy, FLOAT2 _z_range);
	void SetOrthoProjMatrix(FLOAT _fovy, FLOAT2 _z_range);

	SHADER* GetShader();
	void SetShader(SHADER* shader);

	//live update added after
};

class DLL_API Mesh : public Module{
public:
	Mesh(){}
	virtual ~Mesh(){}

	virtual SHADER* GetShader() = 0;
};

class DLL_API PolygonMesh : public Mesh{
private:
	VERTEX* vtx;
	UINT vtx_num;
	INDEXED* idx;
	SHADER* shd;

	void* uniform_block;
	UINT uniform_block_size;
	
	_MODULE::TYPE type;

public:
	PolygonMesh();
	~PolygonMesh();

	//RENDER_OBJECT GetMesh();

	_MODULE::TYPE Type();
	void SetPolygon();
	void SetLine();
	void SetPoint();

	void CreateVertex(UINT num);
	void DeleteVertex();
	VERTEX* GetVertex(UINT i);
	UINT GetMaxVertex();

	void CreateIndex(UINT num);
	void DeleteIndex();
	UINT* GetIndex(UINT i);
	UINT GetMaxIndex();

	void SetShader(SHADER* shader);
	SHADER* GetShader();
	void ReleaseShader();

	void SetUniformBlock(void* uni_b, UINT size, void (*update)(void*, UINT*, UINT*) = nullptr);
	void* GetUniformBlock();
	UINT GetUniformBlockSize();
	void (*UpdateUniformBlock)(void* self, UINT* tex, UINT* tex_idx);
};

class DLL_API Listener : public Module{
private:

public:
	_MODULE::TYPE Type();
};

//need fixed
class DLL_API Audio : public Module{
private:
	void* audio;
	void* graph;

public:
	Audio();
	~Audio();

	void CreateAudio(void* graph);
	void ReleaseAudio();

	void Play();
	void Stop();
	void Pause();
	
	UINT GetSeek();
	void SetSeek(UINT t);

	UINT GetLoop();
	void SetLoop(UINT l);

	UINT GetVolume();
	void SetVolume(UINT v);

	_GRAPH::TYPE GetState();

	void* GetGraph();
	void* GetAudio();
	
	_MODULE::TYPE Type();
};

//unused global loop this version
class DLL_API Collider : public Module{
protected:
	_COLLISION::TYPE type;
	UINT64 layer;
	UINT64 layer_mask;

public:
	Collider();
	virtual ~Collider();

	void (*EnterCollision)(Collider*) = nullptr;
	void (*OnCollision)(Collider*) = nullptr;
	void (*EndCollision)(Collider*) = nullptr;
};

class DLL_API PolygonCollider : public Collider{
private: //private:
	FLOAT3* point;
	UINT num;

public:
	PolygonCollider();
	~PolygonCollider();

	void CreatePoint(UINT num);
	void DeletePoint();
	FLOAT3* GetPoint(UINT num);
	UINT GetMaxPoint();

	_MODULE::TYPE Type();
};

class DLL_API CircleCollider : public Collider{
private:
	FLOAT3 origin;
	FLOAT radius;

public:
	_MODULE::TYPE Type();
};

class DLL_API Rigidbody : public Module{
private:
	FLOAT mass;
	FLOAT friction;
	FLOAT elasticity;

	bool grabity;
	
public:
	_MODULE::TYPE Type();
};

class DLL_API TextMesh : public Mesh{
private:
	Font* font;
	wchar_t* text;
	UINT text_len;

	FLOAT4 solid_color;
	FLOAT4 outline_color;
	FLOAT outline_size;
	SHADER* shd;

	GCHAR* gchar;
	UINT gchar_size;

	FLOAT w;
	FLOAT h;

	FLOAT2 box;

	UINT now_sort;

	void DeleteText();

public:
	TextMesh();
	~TextMesh();

	void SetBox(FLOAT2);
	void SetFont(void* font);
	void SetText(wchar_t*);
	void SetSize(FLOAT size);
	void SetSolidColor(FLOAT4 color);
	void SetOutlineColor(FLOAT4 color);
	void SetOutlineSize(FLOAT size);
	void SetSortType(UINT);
	void SetShader(SHADER* shader);

	SHADER* GetShader();

	GCHAR* GetGlyph(UINT i);
	UINT GetGlyphNum();
	UINT GetTextLen();

	FLOAT GetMaxWidth();
	FLOAT GetMaxHeight();

	RENDER_TEXT GetMesh();

	_MODULE::TYPE Type();
};