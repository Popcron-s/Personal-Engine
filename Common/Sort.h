#pragma once

#include "variable.h"
#include "Singleton.h"

class Sort : public Singleton<Sort>{
private:
	struct stack{
		void *st, *ed;
		stack* next;
	}*s;

	void pop(void*& st, void*& ed){
		if(s == nullptr){return;}
		st = s->st;
		ed = s->ed;

		stack* temp = s;
		s = s->next;

		delete temp;
		temp = nullptr;
	}
	void push(void* st, void* ed){
		stack* temp = new stack;
		*temp = {st, ed, s};
		s = temp;
	}
	template<typename T> void change(T& i, T& j){
		T temp = i;
		i = j;
		j = temp;
	}

public:
	template<typename T> static bool default_comp(T i, T j){return i<j;}
	template<typename T> void Quick(T* st, T* ed, bool (*comp)(T, T) = Sort::default_comp){
		s = nullptr;
		push(st, ed);
		while(s != nullptr){
			pop((void*&)st, (void*&)ed);
			if(ed-st <= 1){continue;}
			if(ed-st == 2){
				if(!comp(*st, *(ed-1))){change(*st, *(ed-1));}
				continue;
			}

			T *i = st+1, *j = ed-1;
			T pivot = *st;

			while(i<=j){
				while(i<ed &&  comp(*i, pivot)){++i;}
				while(st<j && !comp(*j, pivot)){--j;}
				if(i<j){
					change(*i, *j);
					++i;
					--j;
				}
			}
			if(st<j){change(*st, *j);}
			
			if(st<j){push(st, j);}
			if(i<ed){push(i, ed);}
		}
	}
};