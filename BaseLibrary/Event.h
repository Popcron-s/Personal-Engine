#pragma once

#include "..\\Common\\Singleton.h"
#include "..\\Common\\variable.h"
#include "..\\Common\\List.h"
/*
struct EventNode{
	void (*event)(void *);
	UINT time;
	void* extra_info;
};

class EventDispatcher : public Singleton<EventDispatcher>{
private:
	List<EventNode> m_EventList;

public:
	void AddEvent();
	void Dispatch();
};*/