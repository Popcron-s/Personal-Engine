#pragma once

#include "..\\Interface\\Base_Output_Sounds.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Platform.h>
#include <SLES/OpenSLES_Android.h>

class OpenSLES : public _O_Sounds{
private:
	//engine
	SLObjectItf engine_obj;
	SLEngineItf engine;

	//mixer
	SLObjectItf mixer_obj;
	//SLVolumeItf mixer_vol;

	SLDataLocator_OutputMix mixer_locator;
	SLDataSink sink;

	//buffer
	SLDataLocator_AndroidSimpleBufferQueue locator;

	//graph
	struct Graph{
		SLAndroidDataFormat_PCM_EX format;
		BYTE* buf;
		UINT size;
	};

	//player
	struct Audio{
		Graph* graph;
		UINT loop;

		SLDataSource src;

		SLObjectItf player_obj;
		SLPlayItf player;
		SLVolumeItf player_vol;
		SLAndroidSimpleBufferQueueItf player_buf_q;
	};

	UINT global_vol;

	OpenSLES() : 
		engine_obj(nullptr), engine(nullptr), mixer_obj(nullptr), //mixer_vol(nullptr),
		mixer_locator({}), sink({}), locator({}), global_vol(100)
		{}
	~OpenSLES(){Release();}

public:
	static _O_Sounds* GetSingleton(){
		if(m_pSingleton == nullptr){
			m_pSingleton = new OpenSLES;
		}
		else if(m_pSingleton->GetType() != LIBRARY_TYPE::SOUND::OpenSL){
			delete m_pSingleton;
			m_pSingleton = new OpenSLES;
		}
		return m_pSingleton;
	}

	LIBRARY_TYPE::SOUND GetType(){return LIBRARY_TYPE::SOUND::OpenSL;}

	bool Init();
	bool Release();

	UINT GetVolume();
	void SetVolume(UINT v);

	void* CreateGraph(void*& graph, SOUND s);
	bool DeleteGraph(void*& graph);

	void* CreateAudio(void*& audio, void* graph, UINT loop = 0);
	bool DeleteAudio(void*& audio);

	_GRAPH::TYPE GetState(void* audio);
	void Start(void* audio);
	void Pause(void* audio);
	void Stop(void* audio);

	void Seek(void* audio, UINT n);

	UINT GetLoop(void* audio);
	void SetLoop(void* audio, UINT l);

	UINT GetAudioVolume(void* audio);
	void SetAudioVolume(void* audio, UINT v);

	void Run();

	static void SLAPIENTRY bufq_callback(SLAndroidSimpleBufferQueueItf buf_q, void* context);
	static void SLAPIENTRY play_callback(SLPlayItf player, void *context, SLuint32 event);
};