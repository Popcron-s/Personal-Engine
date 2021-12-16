#pragma once

#include "..\\Common\\Enum.h"
#include "..\\Common\\variable.h"

class __declspec(dllexport) _Graph{
protected:
	_GRAPH::TYPE type;
	UINT time;
	UINT cur;
	bool Loop;
	bool Auto;

public:
	_Graph() : type(_GRAPH::TYPE::STOP), time(0), cur(0), Loop(false), Auto(false){}
	~_Graph(){}

	void SetLoop(bool b){Loop = b;}
	bool IsLoop(){return Loop;}

	void SetAuto(bool b){Auto = b;}
	bool IsAuto(){return Auto;}

	void Play(){type = _GRAPH::TYPE::PLAY;}
	void Stop(){time = 0; Auto = false; type = _GRAPH::TYPE::STOP;}
	void Pause(){type = _GRAPH::TYPE::PAUSE;}
	void Seek(UINT t){time = t;		cur = 0;}
	_GRAPH::TYPE GetType(){return type;}

	virtual void update(UINT tick) = 0;
};

template <typename T>
class _Graph_template : public _Graph{
private:
	T& target;
	struct _node{
		T value;
		_GRAPH::INTERPOLATION inter;
		UINT time;
	}*arr;
	const UINT arr_num;

public:
	_Graph_template(T& t, UINT n) : _Graph(), target(t), arr(new _node[n]), arr_num(n){}
	~_Graph_template(){if(arr != nullptr){delete [] arr; arr = nullptr}}

	//_node& operator [](UINT num){return arr[num];}

	void SetNode(UINT n, T v, _GRAPH::INTERPOLATION i, UINT t){
		arr[n] = {v,i,t};
	}

	void Interpolation(T prev, T next, FLOAT weight, _GRAPH::INTERPOLATION inter){
		switch(inter){
		case _GRAPH::INTERPOLATION::POINT:
			target = prev;
			return;
		case _GRAPH::INTERPOLATION::LINEAR:
			target = prev + ((next-prev)*weight);
			return;
		case _GRAPH::INTERPOLATION::SQUARED:
			target = prev + ((next-prev)*weight*weight);
			return;
		}
		return;
	}

	void update(UINT tick){
		if(type != _GRAPH::TYPE::PLAY){
			if(Auto){
				type = _GRAPH::TYPE::PLAY;
			}
			else{
				return;
			}
		}
		time += tick;
		while(true){
			if(cur == arr_num-1){
				if(Loop){
					time -= arr[cur].time;
					cur = 0;
				}
				else{
					time = 0;
					target = arr[cur].value;
					cur = 0;
					type = _GRAPH::TYPE::STOP;
					return;
				}
			}
			if(time < arr[cur+1].time){
				FLOAT w = (FLOAT)(time - arr[cur].time)/(FLOAT)(arr[cur+1].time - arr[cur].time);
				return Interpolation(arr[cur].value, arr[cur+1].value, w, arr[cur].inter);
			}
			++cur;
		}
	}
};

template <class T>
class _Graph_function : public _Graph{
private:
	T* target;
	struct _node{
		void (T::*func)();
		UINT time;
	}*arr;
	const UINT arr_num;

public:
	_Graph_function(T* t, UINT n) : _Graph(), target(t), arr(new _node[n]), arr_num(n){}
	~_Graph_function(){if(arr != nullptr){delete [] arr; arr = nullptr}}

	//_node& operator [](UINT num){return arr[num];}

	void SetNode(UINT n, void (T::*f)(), UINT t){
		arr[n] = {f,t};
	}

	void update(UINT tick){
		if(type != _GRAPH::TYPE::PLAY){
			if(Auto){
				type = _GRAPH::TYPE::PLAY;
			}
			else{
				return;
			}
		}
		time += tick;
		if(cur == arr_num){
			if(Loop){
				time -= arr[cur-1].time;
				cur = 0;
			}
			else{
				time = 0;
				cur = 0;
				type = _GRAPH::TYPE::STOP;
				return;
			}
		}
		while(arr[cur].time < time){
			(target->*(arr[cur].func))();
			++cur;
			if(cur == arr_num){
				if(Loop){
					time -= arr[cur-1].time;
					cur = 0;
				}
				else{
					time = 0;
					cur = 0;
					type = _GRAPH::TYPE::STOP;
					return;
				}
			}
			return;
		}
	}
};

class __declspec(dllexport) ANIMATION{
private:
	void*& m_Target;
	_Graph** m_Clip;
	UINT m_MaxClip;

public:
	ANIMATION(void*& target, UINT max_clip) : m_Target(target), m_Clip(new _Graph*[max_clip]), m_MaxClip(max_clip){
		for(UINT i = 0 ; i<m_MaxClip ; ++i){
			m_Clip[i] = nullptr;
		}
	}
	~ANIMATION(){
		if(m_Clip != nullptr){
			for(UINT i = 0 ; i<m_MaxClip ; ++i){
				if(m_Clip[i] != nullptr){
					delete m_Clip[i];
					m_Clip = nullptr;
				}
			}
			delete [] m_Clip;
			m_Clip = nullptr;
		}
	}

	void Play(){for(UINT i = 0 ; i<m_MaxClip ; ++i){(m_Clip[i])->Play();}}
	void Stop(){for(UINT i = 0 ; i<m_MaxClip ; ++i){(m_Clip[i])->Stop();}}
	void Pause(){for(UINT i = 0 ; i<m_MaxClip ; ++i){(m_Clip[i])->Pause();}}
	void SetAuto(bool b){for(UINT i = 0 ; i<m_MaxClip ; ++i){(m_Clip[i])->SetAuto(b);}}
	void SetLoop(bool b){for(UINT i = 0 ; i<m_MaxClip ; ++i){(m_Clip[i])->SetLoop(b);}}
	void Seek(UINT time){for(UINT i = 0 ; i<m_MaxClip ; ++i){(m_Clip[i])->Seek(time);}}
	_GRAPH::TYPE GetType(){
		bool pause = false;
		for(UINT i = 0 ; i<m_MaxClip ; ++i){
			if((m_Clip[i])->GetType() == _GRAPH::TYPE::PLAY){return _GRAPH::TYPE::PLAY;}
			else if((m_Clip[i])->GetType() == _GRAPH::TYPE::PAUSE){pause = false;}
		}
		if(pause){return _GRAPH::TYPE::PAUSE;}
		return _GRAPH::TYPE::STOP;
	}

	_Graph*& operator [] (UINT i){return m_Clip[i];}
	UINT GetNumber(){return m_MaxClip;}
	void update(UINT tick){
		for(UINT i = 0 ; i<m_MaxClip ; ++i){
			if(m_Clip[i] != nullptr){(m_Clip[i])->update(tick);}
		}
	};
};
