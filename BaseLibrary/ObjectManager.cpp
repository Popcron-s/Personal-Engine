#include "ObjectManager.h"

#include "Object.h"
#include "Module.h"
#include "..\\Common\\List.h"
#include "..\\Common\\Set.h"
#include "..\\Common\\Sort.h"

//first work
//-Regist Object
//-Remove Object
//-Update and work World Set

ObjectManager::ObjectManager() : mesh_list(nullptr), mesh_num(0),
								 audio_list(nullptr), audio_num(0),
								 camera_list(nullptr), camera_num(0),
								 collider_list(nullptr), collider_num(0),
								 stack_size(1<<10),
								 module_not_has_parent(0){
	mesh_list = new Mesh*[stack_size];
	audio_list = new Audio*[stack_size];
	camera_list = new Camera*[stack_size];
	collider_list = new Collider*[stack_size];

	for(INT i = 0 ; i<stack_size ; ++i){
		mesh_list[i] = nullptr;
		audio_list[i] = nullptr;
		camera_list[i] = nullptr;
		collider_list[i] = nullptr;
	}

	obj_set = new Set<OBJECT*, UINT64>;
	InitModuleList();
}
ObjectManager::~ObjectManager(){
	Set<OBJECT*, UINT64>::iterator iter = obj_set->Begin();
	while(obj_set->GetNumber() != 0){
		delete (*iter); 
		iter = obj_set->Begin();
	}
	if(obj_set != nullptr){delete obj_set; obj_set = nullptr;}
	ReleaseModuleList();
}

void ObjectManager::RegistObject(OBJECT* obj){
	obj_set->CreateNode(obj, obj->GetIDS());
}

void ObjectManager::RemoveObject(OBJECT* obj){
	obj_set->ReleaseNode(obj);
}

bool SortAlgorithm(Module* a, Module* b){
	UINT64 aid = a->parent->GetIDS();
	UINT64 bid = b->parent->GetIDS();
	return aid<bid;
}

void ObjectManager::InitModuleList(){
	mesh_num = 0;
	audio_num = 0;
	camera_num = 0;
	collider_num = 0;
	for(INT i = 0 ; i<stack_size ; ++i){
		bool chk = true;
		if(mesh_list[i]     != nullptr){mesh_list[i]     = nullptr; chk = false;}
		if(audio_list[i]    != nullptr){audio_list[i]    = nullptr; chk = false;}
		if(camera_list[i]   != nullptr){camera_list[i]   = nullptr; chk = false;}
		if(collider_list[i] != nullptr){collider_list[i] = nullptr; chk = false;}
		if(chk){break;}
	}
}
void ObjectManager::SortModuleList(){
	Sort::GetSingleton()->Quick((Module**)mesh_list, (Module**)(mesh_list+mesh_num), SortAlgorithm);
	Sort::GetSingleton()->Quick((Module**)audio_list, (Module**)(audio_list+audio_num), SortAlgorithm);
	Sort::GetSingleton()->Quick((Module**)camera_list, (Module**)(camera_list+camera_num), SortAlgorithm);

}
void ObjectManager::ReleaseModuleList(){
	if(mesh_list != nullptr){delete[] mesh_list; mesh_list = nullptr;}
	if(audio_list != nullptr){delete[] audio_list; audio_list = nullptr;}
	if(camera_list != nullptr){delete[] camera_list; camera_list = nullptr;}
	if(collider_list != nullptr){delete[] collider_list; collider_list = nullptr;}
}

#define ModuleGet(M, VN)	\
M** ObjectManager::Get##M(M**& arr){arr = VN##_list; return arr;}	\
UINT ObjectManager::Get##M##Num(){return VN##_num;}	

ModuleGet(Mesh, mesh);
ModuleGet(Audio, audio);
ModuleGet(Camera, camera);
ModuleGet(Collider, collider);

#undef ModuleGet

UINT ObjectManager::GetModuleNotHasParent(){return module_not_has_parent;}

void ObjectManager::update(){
	Set<OBJECT*, UINT64>::iterator obj_iter = obj_set->Begin();
	UINT obj_num = obj_set->GetNumber();

	InitModuleList();

	module_not_has_parent = 0;

	for(UINT i = 0 ; i<obj_num ; ++i, ++obj_iter){
		if(!(*obj_iter)->active){continue;}
		(*obj_iter)->update();
		//world set -> move renderer after delete
		if((*obj_iter)->GetJoint() != nullptr){(*obj_iter)->GetJoint()->SetWorld(state);}

		if((*obj_iter)->GetModuleList() == nullptr){continue;}
		List<Module*>::iterator m_iter = (*obj_iter)->GetModuleList()->Begin();
		List<Module*>::iterator m_end = (*obj_iter)->GetModuleList()->End();

		for( ; m_iter != m_end ; ++m_iter){
			if((*m_iter)->parent == nullptr){++module_not_has_parent;}
			if(!(*m_iter)->IsActive() || (*m_iter)->parent == nullptr){continue;}
			switch((_MODULE::TYPE)((UINT)(*m_iter)->Type() & (UINT)_MODULE::TYPE::HIGH_ORDER)){
			case _MODULE::TYPE::Mesh:	mesh_list[mesh_num++] = (Mesh*)(*m_iter); break;
			case _MODULE::TYPE::Sound:	audio_list[audio_num++] = (Audio*)(*m_iter);  break;
			case _MODULE::TYPE::Camera:	camera_list[camera_num++] = (Camera*)(*m_iter);  break;
			}

			//world set -> move renderer after delete
			if((*m_iter)->GetJoint() != nullptr){(*m_iter)->GetJoint()->SetWorld(state);}
		}
	}

	//SortModuleList();

	state = !state;
}