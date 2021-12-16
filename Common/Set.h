#pragma once

#include "variable.h"

template <class T, typename V>
class Set{
private:
	struct _node{
		V pivot;
		T data;
		_node* next;
	}*list;
	UINT num;

public:
	Set() : list(nullptr), num(0){}
	~Set(){AllReleaseNode();}

	//void (*comp)(V, V)
	T& CreateNode(T data, V pivot){
		_node* t_node = new _node;
		t_node->pivot = pivot;
		t_node->data = data;
		t_node->next = nullptr;

		if(list == nullptr){
			list = t_node;
		}
		else if(list->pivot > t_node->pivot){
			t_node->next = list;
			list = t_node;
		}
		else{
			_node* prev = list;
			while(prev->next != nullptr && prev->next->pivot<=t_node->pivot){
				prev = prev->next;
			}
			t_node->next = prev->next;
			prev->next = t_node;
		}

		++num;

		return t_node->data;
	}
	bool ReleaseNode(T data){
		_node** t_node = &list;
		while((*t_node)->data != data){
			t_node = &((*t_node)->next);
			if(*t_node == nullptr){return false;}
		}
		_node* d_node = *t_node;
		*t_node = d_node->next;
		delete d_node;

		--num;

		return true;
	}
	bool ReleaseNode(){
		if(list == nullptr){return false;}
		_node* d_node = list;
		list = d_node->next;
		delete d_node;

		--num;

		return true;
	}
	bool AllReleaseNode(){
		while(ReleaseNode()){}
		return true;
	}

	UINT GetNumber(){return num;}

	void* Begin(){return list;}
	//void* End(){return *last;}

	T* GetArray(T*& arr){
		if(num <= 0){return nullptr;}
		if(arr != nullptr){
			delete[] arr;
		}
		arr = new T[num];
		_node* t_node = list;
		UINT i = 0;
		while(t_node != nullptr){
			arr[i++] = t_node->data;
			t_node = t_node->next;
		}
		return arr;
	}

	class iterator{
	private:
		_node* point;

	public:
		iterator() : point(nullptr){}
		iterator(void* p) : point((_node*)p){}
		~iterator(){}

		void Next(){if(nullptr == point){return;} point = point->next;}
		
		void operator =(const void*& p){this->point = p;}
		void operator ++(){if(nullptr == this->point){return;} this->point = this->point->next;}
		operator void*(){return point;}
		T& operator *(){return point->data;}
		T* operator ->(){return &(point->data);}
	};
};