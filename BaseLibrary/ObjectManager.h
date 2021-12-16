#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Singleton.h"

template <class T> class Tree;
template <class T> class List;
template <class T, typename V> class Set;
class OBJECT;
class Field;

class Mesh;
class Camera;
//class PhysicsBody;
class Collider;
class Audio;
//class Animation;

class ObjectManager : public Singleton<ObjectManager>{
private:
	Set<OBJECT*, UINT64>* obj_set;
	bool state;

	const UINT stack_size;
	UINT module_not_has_parent;	//debuggin variable

public:
	ObjectManager();
	~ObjectManager();

public:
	//update
	//-object update
	//-mesh layer update
	//--Check FieldID
	//--Check Frustum Culling

	//update order
	//-object update
	//-physics update
	//-animation update
	//-graphic update
	void update();

	void RegistObject(OBJECT*);
	void RemoveObject(OBJECT*);

	void InitModuleList();
	void SortModuleList();
	void ReleaseModuleList();

	UINT GetModuleNotHasParent();	//debuggin funtion

#define ModuleGet(M, VN) \
private:	\
	M** VN##_list;	\
	UINT VN##_num;	\
public:	\
	M** Get##M(M**&);	\
	UINT Get##M##Num();

	ModuleGet(Mesh, mesh);
	ModuleGet(Audio, audio);
	ModuleGet(Camera, camera);
	ModuleGet(Collider, collider);
	
	/*
private:
	Mesh** mesh_list;
	UINT mesh_num;
public:
	Mesh** GetMesh(Mesh**&);
	UINT GetMeshNum();*/
#undef ModuleGet
};