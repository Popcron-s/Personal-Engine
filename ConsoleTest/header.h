#pragma once

#include <iostream>

#include "..\\Common\\variable.h"
#include "..\\BaseLibrary\\Object.h"
#include "..\\BaseLibrary\\Module.h"
#include "..\\BaseLibrary\\MainSystem.h"

class FontTestPage : public OBJECT{
private:
	TextMesh* text;

public:
	FontTestPage();
	~FontTestPage();

	void update();
};

class LightTestPage : public OBJECT{
private:
	PolygonMesh* mesh;
	TextMesh* info_text;

	FLOAT3 light_ori;
	FLOAT3 dir_light;

	struct UNI_BLOCK{
		MATRIX4x4 world;
		FLOAT3 light;
		UINT diffuse_idx;
		UINT normal_idx;
		UINT height_idx;
	}uni_block;

	void* diffuse;
	void* normal;
	void* height;

public:
	LightTestPage();
	~LightTestPage();

	void update();

	static void UpdateUniformBlock(void* self, UINT* tex, UINT* tex_idx);
};

class CollisionTestPage : public OBJECT{
private:
	PolygonMesh* p_mesh;
	PolygonCollider* p_col;

	FLOAT velocity;
	FLOAT accel;

	PolygonMesh* m_mesh[10];
	PolygonCollider* m_col[10];

	UINT texture_idx[10];
	FLOAT angluar_velocity[10];
	FLOAT theta[24];

	TextMesh* info_text;

	void* texture[4];

	struct UNI_BLOCK{
		MATRIX4x4 world;
		UINT texture_idx;
	}p_uni_block, m_uni_block[10];

public:
	CollisionTestPage();
	~CollisionTestPage();

	void PlayerCreate();
	void MeteorCreate();

	void MeteorRebatch();

	void ShuffleTheta();

	void Rebatch();
	void InputUpdate();
	void CollisionUpdate();

	void update();

	static void UpdateUniformBlockP(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock0(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock1(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock2(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock3(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock4(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock5(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock6(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock7(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock8(void* self, UINT* tex, UINT* tex_idx);
	static void UpdateUniformBlock9(void* self, UINT* tex, UINT* tex_idx);
};

class BGShape : public OBJECT{
private:
	PolygonMesh* mesh;

	struct UNI_BLOCK{
		MATRIX4x4 world;
		UINT texture_idx;
	}uni_block;

	void* texture;

public:
	BGShape();
	~BGShape();

	void update();

	static void UpdateUniformBlock(void* self, UINT* tex, UINT* tex_idx);
};

class BGCamera : public OBJECT{
private:
	Camera* cam;
	BGShape* BackGround;

public:
	BGCamera();
	~BGCamera();

	void update(){}
};

class UIShape : public OBJECT{
private:
	PolygonMesh* mesh;
	TextMesh* text[4];

	FLOAT w;
	FLOAT h;

	FLOAT3 textStart;
	FLOAT mainOL;
	FLOAT subOL;

	struct UNI_BLOCK{
		MATRIX4x4 world;
		UINT texture_idx;
	}uni_block;

	void* texture;

public:
	UIShape();
	~UIShape();

	void update();

	static void UpdateUniformBlock(void* self, UINT* tex, UINT* tex_idx);
};

class UICamera : public OBJECT{
private:
	Camera* cam;
	UIShape* ui;

	FontTestPage* ftp;
	LightTestPage* ltp;
	CollisionTestPage* ctp;

public:
	UICamera();
	~UICamera();

	void update();
};

class MainCamera : public OBJECT{
private:
	Camera* cam;

	FLOAT z;

public:
	MainCamera();
	~MainCamera();

	void update();
};

void starter();
