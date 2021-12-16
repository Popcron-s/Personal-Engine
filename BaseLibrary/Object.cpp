#include "Object.h"
#include "ObjectManager.h"

#include "Module.h"

#include "..\\Common\\List.h"

OBJECT::OBJECT() : joint(nullptr), Module_List(nullptr), ids(0), active(true){
	ObjectManager::GetSingleton()->RegistObject(this);
	joint = new Joint;
}
OBJECT::~OBJECT(){
	ObjectManager::GetSingleton()->RemoveObject(this);
	if(joint != nullptr){
		delete joint;
		joint = nullptr;
	}
}

void OBJECT::InitModuleList(){
	if(Module_List != nullptr){
		Module_List->AllReleaseNode();
	}
	else{
		Module_List = new List<Module*>;
	}
}

void OBJECT::RegistModule(Module* m){
	Module_List->CreateNode(m);
}

void OBJECT::RemoveModule(Module* m){
	Module_List->ReleaseNode(m);
}

void OBJECT::ReleaseModuleList(){
	while(Module_List->GetNumber() != 0){
		Module* m = Module_List->GetData(0);
		delete m;
		Module_List->ReleaseNode((UINT)0);
	}
	if(Module_List != nullptr){
		Module_List->AllReleaseNode();
		delete Module_List;
		Module_List = nullptr;
	}
}

List<Module*>* OBJECT::GetModuleList(){return Module_List;}

Joint* OBJECT::GetJoint(){return joint;}

void OBJECT::SetID(UINT16 _map, UINT16 _layer, UINT32 _id){
	ObjectManager::GetSingleton()->RemoveObject(this);
	map_id = _map;
	layer = _layer;
	id = _id;
	ObjectManager::GetSingleton()->RegistObject(this);
}
UINT64 OBJECT::GetIDS(){return ids;}
UINT16 OBJECT::GetMapID(){return map_id;}
UINT16 OBJECT::GetLayer(){return layer;}
UINT32 OBJECT::GetObjectID(){return id;}

//List<Module*>* OBJECT::GetModuleList(){return Module_List;}