#pragma once

#include "..\\Common\\Enum.h"
#include "..\\Common\\variable.h"

#ifdef WIN32
#ifdef ENGINE_WIN32_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif

class Module;
class Joint;
template <class T> class List;

class DLL_API OBJECT{
public:
	virtual void update() = 0;

private:
	union{
		UINT64 ids;
		struct{
			UINT32 id;
			UINT16 layer;
			UINT16 map_id;
		};
	};
	Joint* joint;
	List<Module*>* Module_List;	//module

protected:
	void InitModuleList();
	void RegistModule(Module*);
	void RemoveModule(Module*);
	void ReleaseModuleList();

public:
	bool active;

public:
	OBJECT();
	virtual ~OBJECT();

	Joint* GetJoint();
	List<Module*>* GetModuleList();

	UINT16 GetMapID();
	UINT16 GetLayer();
	UINT32 GetObjectID();
	UINT64 GetIDS();
	void SetID(UINT16 map, UINT16 layer, UINT32 id);
};

#undef GetSetVariable