#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Enum.h"

#include "InterfaceSingleton.h"

class _O_Sounds : public InterfaceSingleton<_O_Sounds>{
public:
	_O_Sounds(){}
	virtual ~_O_Sounds(){}

	virtual LIBRARY_TYPE::SOUND GetType() = 0;

	virtual bool Init() = 0;
	virtual bool Release() = 0;

	virtual UINT GetVolume() = 0;
	virtual void SetVolume(UINT v) = 0;

	virtual void* CreateGraph(void*& graph, SOUND s) = 0;
	virtual bool DeleteGraph(void*& graph) = 0;

	virtual void* CreateAudio(void*& audio, void* graph, UINT loop = 0) = 0;
	virtual bool DeleteAudio(void*& audio) = 0;

	virtual _GRAPH::TYPE GetState(void* audio) = 0;
	virtual void Start(void* audio) = 0;
	virtual void Pause(void* audio) = 0;
	virtual void Stop(void* audio) = 0;

	virtual void Seek(void* audio, UINT n) = 0;

	virtual UINT GetLoop(void* audio) = 0;
	virtual void SetLoop(void* audio, UINT l) = 0;

	virtual UINT GetAudioVolume(void* audio) = 0;
	virtual void SetAudioVolume(void* audio, UINT v) = 0;

	virtual void Run() = 0;
};