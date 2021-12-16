#pragma once

#include "variable.h"

template <class T>
class List{
protected:
	struct node{
		T data;
		node* next;
	}*m_pList, **last;
	UINT m_Num;

public:
	List() : m_pList(nullptr), m_Num(0), last(&m_pList){}
	~List(){AllReleaseNode();}
	T& CreateNode(T data){
		node* t_node = new node;
		*last = t_node;
		t_node->data = data;
		t_node->next = nullptr;
		last = &(t_node->next);
		++m_Num;
		return t_node->data;
	}
	T& CreateNode(T data, UINT num){
		if(num > m_Num){num = m_Num;}
		node* t_node = new node;
		t_node->data = data;
		node** link = &m_pList;
		for(UINT i = 0 ; i<num ; ++i){
			link = &((*link)->next);
		}
		if(link == last){
			*last = t_node;
			t_node->next = nullptr;
			last = &(t_node->next);
		}
		else if(*link == m_pList){
			t_node->next = m_pList;
			m_pList = t_node;
		}
		else{
			t_node->next = *link;
			*link = t_node;
		}
		++m_Num;
		return t_node->data;
	}
	bool ReleaseNode(T data){
		node** t_node = &m_pList;
		while((*t_node)->data != data){
			t_node = &((*t_node)->next);
			if(*t_node == nullptr){return false;}
		}
		if(last == &((*t_node)->next)){
			last = t_node;
		}
		node* d_node = *t_node;
		*t_node = d_node->next;
		delete d_node;
		--m_Num;
		return true;
	}
	bool ReleaseNode(UINT id){
		if(id >= m_Num){return false;}
		node** t_node = &m_pList;
		for(UINT i = 0 ; i<id ; ++i){
			t_node = &((*t_node)->next);
		}
		if(last == &((*t_node)->next)){
			last = t_node;
		}
		node* d_node = *t_node;
		*t_node = d_node->next;
		delete d_node;
		--m_Num;
		return true;
	}
	void AllReleaseNode(){
		node* d_node = nullptr;
		while(m_pList != nullptr){
			d_node = m_pList;
			m_pList = m_pList->next;
			delete d_node;
		}
		last = &m_pList;
		m_Num = 0;
		return;
	}
	bool SwapNode(T* data, INT order){
		if(order == -1){order = m_Num;}
		if(order > m_Num){return false;}
		node** t_node = &m_pList;
		node** order_node = &m_pList;
		while((order > 0) || (&((*t_node)->data) != data)){
			if(&((*t_node)->data) != data){t_node = &((*t_node)->next);}
			if(order > 0){
				--order;
				order_node = &((*order_node)->next);
			}
			if(*t_node == nullptr){return false;}
		}
		if(last == &((*t_node)->next)){
			last = t_node;
		}
		node* c_node = *t_node;
		*t_node = (*t_node)->next;
		c_node->next = *order_node;
		*order_node = c_node;
		if(*last != nullptr){
			last = &((*last)->next);
		}
		return true;
	}
	bool SwapNode(UINT id, INT order){
		if(order == -1){order = m_Num;}
		if(order >= m_Num || id >= m_Num){return false;}
		node** t_node = &m_pList;
		node** order_node = &m_pList;
		while((order > 0) || (id > 0)){
			if(id > 0){
				--id;
				t_node = &((*t_node)->next);
				if(id == 0){order_node = &((*order_node)->next);}
			}
			if(order > 0){
				--order;
				order_node = &((*order_node)->next);
			}
			//if(*t_node == 0x00){return false;}
		}
		if(last == &((*t_node)->next)){
			last = t_node;
		}
		node* c_node = *t_node;
		*t_node = (*t_node)->next;
		c_node->next = *order_node;
		*order_node = c_node;
		if(*last != nullptr){
			last = &((*last)->next);
		}
		return true;
	}

	UINT GetNumber(){return m_Num;}
	T& GetData(UINT id){
		if(id >= m_Num){m_pList->data;}
		node* t_node = m_pList;
		for(UINT i = 0 ; i<id ; ++i){t_node = t_node->next;}
		return t_node->data;
	}
	T* GetArray(T*& arr){
		if(m_Num <= 0){return nullptr;}
		if(arr != nullptr){
			delete [] arr;
		}
		arr = new T[m_Num];
		node* t_node = m_pList;
		for(UINT i = 0 ; i<m_Num ; ++i){
			arr[i] = t_node->data;
			t_node = t_node->next;
		}
		return arr;
	}

	void* Begin(){return m_pList;}
	void* End(){return *last;}

	class iterator{
	private:
		node* point;

	public:
		iterator() : point(nullptr){}
		iterator(void* p) : point((node*)p){}
		~iterator(){}
		void Next(){if(nullptr == point){return;} point = point->next;}
		//T& Data(){return point->data;}

		void operator =(const void*& p){this->point = p;}
		void operator ++(){if(nullptr == point){return;} this->point = this->point->next;}
		//bool operator ==(const void* p){if(this->point == p){return true;} else{return false;}}
		//bool operator !=(const void* p){if(this->point != p){return true;} else{return false;}}
		operator void*(){return point;}
		T& operator *(){return point->data;}
		T* operator ->(){return &(point->data);}
	};
};
/*
template <class T>
class Singleton_List : public List<T>{
private:
	static Singleton_List<T>* m_pSingleton;

protected:
	Singleton_List<T>(){}
	~Singleton_List<T>(){AllReleaseNode();}

public:
	static Singleton_List<T>* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new Singleton_List<T>;
		}
		return m_pSingleton;
	}
	static void Destroy(){
		if(m_pSingleton != nullptr){
			delete m_pSingleton;
			m_pSingleton = nullptr;
		}
	}
};

template <class T> 
Singleton_List<T>* Singleton_List<T>::m_pSingleton = nullptr;*/