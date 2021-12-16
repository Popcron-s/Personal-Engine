#include "ResourceManager.h"

#include "..\\Interface\\Base_FileIO.h"
#include "..\\Coder\\Coder.h"

INT strcmp(char* a, char* b){	//a<b = -1, a=b = 0, a>b = 1
	UINT i = 0;
	while(true){
		if(a[i] == 0x00 && b[i] == 0x00){return true;}
		if(a[i] == 0x00){return -1;}
		if(b[i] == 0x00){return 1;}

		if(a[i] < b[i]){return -1;}
		else if(a[i] > b[i]){return 1;}

		++i;
	}
}

void strcpy(char* a, char* b){
	*a = {};
	for(UINT i = 0 ; b[i] != 0x00 ; ++i){
		a[i] = b[i];
	}
}

bool ResourceManager::IsRed(_node* n){
	if(n == nullptr){return false;}
	if(!(n->color)){return false;}
	return true;
}

void ResourceManager::RotationLeft(_node* n){
	_node* p = n->p;
	_node* c = n->r;

	if(c->l != nullptr){c->l->p = n;}
	n->r = c->l;
	n->p = c;
	c->l = n;
	c->p = p;

	if(p != nullptr){
		if(p->l == n){p->l = c;}
		else{p->r = c;}
	}
}

void ResourceManager::RotationRight(_node* n){
	_node* p = n->p;
	_node* c = n->l;

	if(c->r != nullptr){c->r->p = n;}
	n->l = c->r;
	n->p = c;
	c->r = n;
	c->p = p;

	if(p != nullptr){
		if(p->r == n){p->r = c;}
		else{p->l = c;}
	}
}

template<class T> T* ResourceManager::OpenFile(char name[260]){
	if(_FileIO::GetSingleton() == nullptr){return nullptr;}

}

bool ResourceManager::InsertResource(char key[260], void* data, RESOURCEFORMAT::TYPE type){
	_node* t_node = new _node;
	*t_node = {{}, data, type, nullptr, nullptr, nullptr, 1};
	strcpy(t_node->key, key);

	std::map<char*, _node*, map_strcmp>::iterator it = t_map.begin();
	it = t_map.find(t_node->key);

	if(it == t_map.end()){
		t_map.insert(std::make_pair(t_node->key, t_node));
		return true;
	}
	else{
		delete t_node;
		return false;
	}

	if(root == nullptr){t_node->color = false; root = t_node; return true;}

	_node* prev = root;
	while(t_node->p == nullptr){
		switch(strcmp(t_node->key, prev->key)){
			case -1:	if(prev->l != nullptr){prev = prev->l;} else{prev->l = t_node; t_node->p = prev;}	break;
			case 0:		delete t_node; return false;
			case 1:		if(prev->r != nullptr){prev = prev->r;} else{prev->r = t_node; t_node->p = prev;}	break;
		}
	}

	if(t_node->p->color == 0){return true;}

	_node* g = t_node->p->p;
	_node* u = g->l;
	if(u == t_node->p){u = g->r;}

	if((u != nullptr) && (u->color == 1)){
		t_node->p->color = 0;
		u->color = 0;
		if(g->p != nullptr){g->color = 1;}
		return true;
	}

	if((t_node == t_node->p->r) && (t_node->p == g->l)){
		RotationLeft(t_node->p);
		t_node = t_node->l;
	}
	else if((t_node == t_node->p->l) && (t_node->p == g->r)){
		RotationRight(t_node->p);
		t_node = t_node->r;
	}

	t_node->p->color = 0;
	t_node->p->p->color = 1;
	if(t_node->p->l == t_node){
		RotationRight(t_node->p->p);
		if(root == t_node->p->r){root = t_node->p;}
	}
	else if(t_node->p->r == t_node){
		RotationLeft(t_node->p->p);
		if(root == t_node->p->l){root = t_node->p;}
	}

	return true;
}

bool ResourceManager::RemoveResource(char key[260]){
	std::map<char*, _node*, map_strcmp>::iterator it = t_map.begin();
	it = t_map.find(key);

	if(it == t_map.end()){
		return false;
	}
	else{
		_node* t_node = it->second;
		delete t_node;
		t_map.erase(key);
		return true;
	}

	_node* t_node = root;
	_node* d_node = nullptr;
	while(t_node != nullptr && d_node == nullptr){
		switch(strcmp(key, t_node->key)){
		case -1:	t_node = t_node->l;	break;
		case 0:		d_node = t_node;	break;
		case 1:		t_node = t_node->r;	break;
		}
	}

	if(d_node == nullptr){return false;}

	if(d_node->l != nullptr){
		t_node = d_node->l;
		while(t_node->r != nullptr){
			t_node = t_node->r;
		}

		strcpy(d_node->key, t_node->key);
		d_node->data = t_node->data;
		d_node->type = t_node->type;

		d_node = t_node;
	}

	bool d_node_color = d_node->color;

	//always d_node->l == nullptr

	_node* n = d_node->r;
	if(d_node->p != nullptr){
		if(d_node->p->l == d_node){d_node->p->l = n;}
		else if(d_node->p->r == d_node){d_node->p->r = n;}
	}
	else{root = n;}
	if(n != nullptr){n->p = d_node->p;}

	delete d_node;
	d_node = nullptr;

	if(n == nullptr || (n->l == nullptr && n->r == nullptr)){return true;}

	_node* c = (n->r != nullptr)?n->l:n->r;
	
	bool c_is_leaf = false;
	if(c == nullptr){
		c = new _node;
		*c = {{}, nullptr, RESOURCEFORMAT::TYPE::UNKNOWN, n, nullptr, nullptr, 0};
		if(n->l == nullptr){n->l = c;}
		else if(n->r == nullptr){n->r = c;}
		c_is_leaf = true;
	}

	if(!IsRed(n)){
		if(IsRed(c)){c->color = 0;}
		else{DeleteBlancing(c);}
	}

	if(c_is_leaf){
		if(c->p->l == c){n->p->l = nullptr;}
		else if(c->p->r == c){n->p->r = nullptr;}
		delete c;
		c = nullptr;
	}

	return true;
}

void ResourceManager::DeleteBlancing(_node* n){
	//start delete case
	//case1
	if(n->p == nullptr){root = n; return;}

	//case2
	_node* s = nullptr;
	if(n->p->l == n){s = n->p->r;}
	else{s = n->p->l;}

	if(s == nullptr){return;}
	if(IsRed(s)){
		s->p->color = 1;
		s->color = 0;
		if(n == n->p->l){RotationLeft(n->p);}
		else{RotationRight(n->p);}
	}

	//case3
	if(n->p->l == n){s = n->p->r;}
	else{s = n->p->l;}
	if(s == nullptr){return;}

	if((n->p->color == 0) && (s->color == 0) && 
		((s->l != nullptr && s->l->color == 0) || s->l == nullptr) && 
		((s->r != nullptr && s->r->color == 0) || s->r == nullptr)){
		s->color = 1;
		return DeleteBlancing(n->p);
	}

	//case4
	if((n->p->color == 1) && (s->color == 0) && 
		((s->l != nullptr && s->l->color == 0) || s->l == nullptr) && 
		((s->r != nullptr && s->r->color == 0) || s->r == nullptr)){
		n->p->color = 0;
		s->color = 1;
		return;
	}

	//case5
	if(s->color == 0){
		if((n == n->p->l) && 
			(s->l != nullptr && s->l->color == 1) && 
			((s->r != nullptr && s->r->color == 0) || s->r == nullptr)){
			s->color = 1;
			s->l->color = 0;
			RotationRight(s);
		}
		else if((n == n->p->r) && 
			(s->r != nullptr && s->r->color == 1) && 
			((s->l != nullptr && s->l->color == 0) || s->l == nullptr)){
			s->color = 1;
			s->r->color = 0;
			RotationLeft(s);
		}
	}

	//case6
	if(n->p->l == n){s = n->p->r;}
	else{s = n->p->l;}
	if(s == nullptr){return;}

	s->color = n->p->color;
	n->p->color = 0;

	if(n == n->p->l){
		if(s->r != nullptr){s->r->color = 0;}
		RotationLeft(n->p);
	}
	else{
		if(s->l != nullptr){s->l->color = 0;}
		RotationRight(n->p);
	}

	return;
}

void* ResourceManager::GetResource(char key[260]){
	std::map<char*, _node*, map_strcmp>::iterator it = t_map.begin();
	it = t_map.find(key);

	if(it == t_map.end()){
		return nullptr;
	}
	else{
		return it->second->data;
	}

	_node* t_node = root;
	while(t_node != nullptr){
		switch(strcmp(key, t_node->key)){
		case -1:	t_node = t_node->l;	break;
		case 0:		return t_node->data;
		case 1:		t_node = t_node->r;	break;
		}
	}
	return nullptr;
}