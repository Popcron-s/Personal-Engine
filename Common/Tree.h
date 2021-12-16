#pragma once

#include "variable.h"
#include "List.h"

enum DIR{
	LEFT = 0,
	RIGHT = 1
};

template <class T>
class Tree{
private:
	struct _node{
		T m_Data;

		UINT m_Level;
		_node* m_pParent;
		List<_node*> m_Child;
	}*m_pRoot;
	UINT m_Number;

public:
	Tree() : m_pRoot(nullptr), m_Number(0){}
	~Tree(){Release(m_pRoot);}

	UINT GetNumber(){return m_Number;}

	bool CreateRoot(T data){
		if(m_pRoot != nullptr){return false;}
		m_pRoot = new _node;
		m_pRoot->m_Data = data;
		m_pRoot->m_Level = 0;
		m_pRoot->m_pParent = nullptr;
		m_Number = 1;
		return true;
	}
	bool Create(void* prnt, T data){
		if(prnt == nullptr){return false;}
		_node* p_node = (_node*)prnt;
		_node* c_node = new _node;
		c_node->m_Data = data;
		c_node->m_Level = p_node->m_Level+1;
		c_node->m_pParent = p_node;
		p_node->m_Child.CreateNode(c_node);
		++m_Number;
		return true;
	}
	bool Create(void* prnt, T data, UINT num){
		if(prnt == nullptr){return false;}
		_node* p_node = (_node*)prnt;
		_node* c_node = new _node;
		c_node->m_Data = data;
		c_node->m_Level = p_node->m_Level+1;
		c_node->m_pParent = p_node;
		p_node->m_Child.CreateNode(c_node, num);
		++m_Number;
		return true;
	}
	bool Release(void* crnt){
		if(crnt == nullptr){return false;}
		_node* t = (_node*)crnt;
		while(t->m_Child.GetNumber() != 0){
			Release(t->m_Child.GetData(0));
		}
		t->m_Child.AllReleaseNode();
		if(t->m_pParent != nullptr){
			_node* p = t->m_pParent;
			p->m_Child.ReleaseNode(t);
			delete t;
			--m_Number;
		}
		else{
			delete m_pRoot;
			m_pRoot = nullptr;
			m_Number = 0;
		}
		return true;
	}
	bool Release(void* prnt, UINT num){
		_node* t = (_node*)prnt;
		return Release(prnt->m_Child.GetData(num));
	}
	bool AllRelease(){
		return Release(m_pRoot);
	}

	void* Begin(){return m_pRoot;}

	class iterator{
	private:
		_node* point;
		List<_node*> stack;

	public:
		iterator() : point(nullptr){}
		iterator(void* p) : point(nullptr){
			stack.AllReleaseNode(); 
			//stack.CreateNode((_node*)p);
			point = (_node*)p;
			List<_node*>::iterator iter = point->m_Child.Begin();
			for( ; iter != point->m_Child.End() ; ++iter){
				stack.CreateNode(*iter);
			}
		}
		~iterator(){stack.AllReleaseNode();}

		bool isLast(){if(stack.GetNumber() == 0 && point == nullptr){return true;} else{return false;}}

		void operator ++(){
			if(stack.GetNumber() == 0){
				point = nullptr;
				return;
			}
			point = stack.GetData(0);
			stack.ReleaseNode((UINT)0);
			List<_node*>::iterator iter = point->m_Child.Begin();
			for( ; iter != point->m_Child.End() ; ++iter){
				stack.CreateNode(*iter);
			}
		}
		//bool operator ==(const void* p){if(this->point == p){return true;} else{return false;}}
		//bool operator !=(const void* p){if(this->point != p){return true;} else{return false;}}
		operator void*(){return point;}
		T& operator *(){return point->m_Data;}
		T* operator ->(){return &(point->m_Data);}
	};

	class searcher{
	private:
		_node* point;
	public:
		searcher() : point(nullptr){}
		searcher(void* p) : point((_node*)p){}
		~searcher(){}

		//T& Data(){return point->m_Data;}
		UINT GetLevel(){return point->m_Level;}
		UINT GetChildNum(){return point->m_Child->GetNumber();}

		T& GetParent(){return point->m_pParent->m_Data;}
		T& GetChild(UINT num){return point->m_Child->GetData(num)->m_Data;}

		void Parent(){if(point->m_pParent != nullptr){point = point->m_pParent;}}
		void Child(UINT num){if(point->m_Child.GetNumber() != 0){point = point->m_Child.GetData(num);}}
		
		void operator =(const void*& p){this->point = p;}
		bool operator ==(const void* p){if(this->point == p){return true;} else{return false;}}
		bool operator !=(const void* p){if(this->point != p){return true;} else{return false;}}
		operator void*(){return point;}
		T& operator *(){return point->m_Data;}
		T* operator ->(){return &(point->m_Data);}
	};
};

template <class T>
class BTree{
private:
	T data;
	UINT m_Number;

	BTree<T>* Parent;
	BTree<T>* LTree;
	BTree<T>* RTree;

private:
	void AddNumber(){++m_Number; if(Parent != nullptr){Parent->AddNumber();}}

public:
	BTree(T _data, BTree<T>* _Parent = nullptr) : 
		data(_data), Parent(_Parent), m_Number(1), LTree(nullptr), RTree(nullptr){
		if(Parent != nullptr){
			Parent->AddNumber();
		}
	}
	~BTree(){
		ReleaseLeftNode();
		ReleaseRightNode();
	}

	T& CreateLeftNode(T _data){LTree = new BTree<T>(_data, this); return LTree->GetData();}
	T& CreateRightNode(T _data){RTree = new BTree<T>(_data, this); return RTree->GetData();}
	bool ReleaseLeftNode(){if(LTree != nullptr){m_Number -= LTree->GetNumber(); delete LTree; LTree = nullptr;} return true;}
	bool ReleaseRightNode(){if(RTree != nullptr){m_Number -= RTree->GetNumber(); delete RTree; RTree = nullptr;} return true;}

	UINT GetNumber(){return m_Number;}
	T& GetData(){return data;}
	BTree<T>* GetLTree(){return LTree;}
	BTree<T>* GetRTree(){return RTree;}

	UINT Preorder(T* arr, UINT idx = 0){
		arr[idx++] = data;
		if(LTree != nullptr){idx = LTree->Preorder(arr, idx);}
		if(RTree != nullptr){idx = RTree->Preorder(arr, idx);}
		return idx;
	}	//VLR
	UINT Inorder(T* arr, UINT idx = 0){
		if(LTree != nullptr){idx = LTree->Inorder(arr, idx);}
		arr[idx++] = data;
		if(RTree != nullptr){idx = RTree->Inorder(arr, idx);}
		return idx;
	}	//LVR
	UINT Postorder(T* arr, UINT idx = 0){
		if(LTree != nullptr){idx = LTree->Postorder(arr, idx);}
		if(RTree != nullptr){idx = RTree->Postorder(arr, idx);}
		arr[idx++] = data;
		return idx;
	}	//LRV
};
