#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"
#include "..\\Common\\Singleton.h"

//#include <iostream>
#include <map>

struct map_strcmp{
	bool operator()(char const *a, char const *b) const{
		UINT i = 0;
		while(true){
			if(a[i] == 0x00 && b[i] == 0x00){return false;}
			if(a[i] == 0x00){return true;}
			if(b[i] == 0x00){return false;}

			if(a[i] < b[i]){return true;}
			else if(a[i] > b[i]){return false;}

			++i;
		}
	}
};

class ResourceManager : public Singleton<ResourceManager>{
private:
	struct _node{
		char key[260];
		void* data;
		RESOURCEFORMAT::TYPE type;
		_node* p;
		_node* l;
		_node* r;
		bool color; //0-black / 1-red
	}*root;
	UINT num;
	
	std::map<char*, _node*, map_strcmp> t_map;

	//Getgrandfather
	//getancle
	//rotationleft
	bool IsRed(_node* n);
	void RotationLeft(_node* n);
	void RotationRight(_node* n);

	void DeleteBlancing(_node* n);

public:
	ResourceManager() : root(nullptr){t_map.empty();}
	~ResourceManager(){}

	template<class T>
	T* OpenFile(char name[260]);

	bool InsertResource(char key[260], void* data, RESOURCEFORMAT::TYPE type);
	bool RemoveResource(char key[260]);

	void* GetResource(char key[260]);
	/*
	void test_render(){
		struct t{
			_node* data;
			t* next;
		}*r, *d, **last;

		r = new t;
		*r = {root, nullptr};

		last = &(r->next);

		while(r != nullptr){
			if(r->data == nullptr){std::cout<<"nullptr"<<std::endl;}
			else{
				if(r->data->p == nullptr){std::cout<<"nullptr";}
				else{std::cout<<r->data->p->key;}
				std::cout<<"->"<<r->data->key<<"/"<<r->data->color<<std::endl;

				if(r->data->l != nullptr){*last = new t; **last = {r->data->l, nullptr}; last = &((*last)->next);}
				if(r->data->r != nullptr){*last = new t; **last = {r->data->r, nullptr}; last = &((*last)->next);}
			}

			d = r;
			r = r->next;
			delete d;
		}
	}*/
};