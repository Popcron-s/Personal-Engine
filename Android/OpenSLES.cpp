#include "OpenSLES.h"

bool OpenSLES::Init(){
	//create engine
	slCreateEngine(&engine_obj, 0, NULL, 0, NULL, NULL);
	(*engine_obj)->Realize(engine_obj, SL_BOOLEAN_FALSE);
	(*engine_obj)->GetInterface(engine_obj, SL_IID_ENGINE, &engine);

	//create output mixer
	(*engine)->CreateOutputMix(engine, &mixer_obj, 0, nullptr, nullptr);
	(*mixer_obj)->Realize(mixer_obj, SL_BOOLEAN_FALSE);

	//set sink
	mixer_locator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	mixer_locator.outputMix = mixer_obj;

	sink.pLocator = &mixer_locator;
	sink.pFormat = nullptr;

	//create main audio buffer
	locator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	locator.numBuffers = 32;

	//create 3d audio
	//current version without this

	return true;
}
bool OpenSLES::Release(){
	if(mixer_obj != nullptr){
		(*mixer_obj)->Destroy(mixer_obj); 
		mixer_obj = nullptr;
	}
	if(engine_obj != nullptr){
		(*engine_obj)->Destroy(engine_obj); 
		engine_obj = nullptr;
		engine = nullptr;
	}
	return true;
}

UINT OpenSLES::GetVolume(){return global_vol;}
void OpenSLES::SetVolume(UINT v){if(v>100){v = 100;} global_vol = v;}

void* OpenSLES::CreateGraph(void*& graph, SOUND s){
	if(graph != nullptr){DeleteGraph(graph);}

	SLuint32 channelMask = 0;
	if(s.wBitsPerSample != 16 && s.wBitsPerSample != 8){return nullptr;}
	if(s.nChannels == 1){channelMask = SL_SPEAKER_FRONT_CENTER;}
	else if(s.nChannels == 2){channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;}
	else{return nullptr;}

	Graph*& tg = (Graph*&)graph;
	tg = new Graph;

	//format and loop set
	tg->format.formatType = SL_DATAFORMAT_PCM;
	tg->format.numChannels = s.nChannels;
	tg->format.sampleRate = s.nSamplesPerSec*1000;
	tg->format.bitsPerSample = s.wBitsPerSample;
	tg->format.containerSize = s.wBitsPerSample;
	tg->format.channelMask = channelMask;
	tg->format.endianness = SL_BYTEORDER_LITTLEENDIAN;
	tg->format.representation = SL_ANDROID_PCM_REPRESENTATION_FLOAT;

	tg->buf = new BYTE[s.size];
	if(s.wBitsPerSample == 16){
		for(UINT i = 0 ; i<s.size ; i+=2){
			tg->buf[i+0] = s.buf[i+1];
			tg->buf[i+1] = s.buf[i+0];
		}
	}
	else{
		for(UINT i = 0 ; i<s.size ; ++i){tg->buf[i] = s.buf[i];}
	}
	tg->size = s.size;

	return graph;
}
bool OpenSLES::DeleteGraph(void*& graph){
	if(graph == nullptr){return false;}
	Graph*& tg = (Graph*&)graph;
	delete[] (tg->buf);
	delete tg;
	tg = nullptr;
	return true;
}

void* OpenSLES::CreateAudio(void*& audio, void* graph, UINT loop){
	if(engine_obj == nullptr){return nullptr;}
	
	if(audio != nullptr){DeleteAudio(audio);}
	if(graph == nullptr){return nullptr;}

	Audio*& ta = (Audio*&)audio;
	ta = new Audio;

	ta->graph = (Graph*)graph;
	ta->loop = loop;

	ta->src.pFormat = &(ta->graph->format);
	ta->src.pLocator = &locator;

	//audio set
	SLInterfaceID ids[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME};
	SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

	SLresult r = (*engine)->CreateAudioPlayer(engine, &(ta->player_obj), &(ta->src), &sink, 2, ids, req);

	if(r != SL_RESULT_SUCCESS){delete ta; ta = nullptr; return nullptr;}

	(*(ta->player_obj))->Realize(ta->player_obj, SL_BOOLEAN_FALSE);

	(*(ta->player_obj))->GetInterface(ta->player_obj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &(ta->player_buf_q));
	(*(ta->player_buf_q))->Clear(ta->player_buf_q);
	(*(ta->player_buf_q))->Enqueue(ta->player_buf_q, ta->graph->buf, ta->graph->size);
	(*(ta->player_buf_q))->RegisterCallback(ta->player_buf_q, OpenSLES::bufq_callback, ta);

	(*(ta->player_obj))->GetInterface(ta->player_obj, SL_IID_VOLUME, &(ta->player_vol));
	//(*(ta->player_vol))->EnableStereoPosition(ta->player_vol, SL_BOOLEAN_FALSE);
	(*(ta->player_vol))->SetVolumeLevel(ta->player_vol, 0);

	(*(ta->player_obj))->GetInterface(ta->player_obj, SL_IID_PLAY, &(ta->player));
	//(*(ta->player))->RegisterCallback(ta->player, OpenSLES::play_callback, ta);
	//(*(ta->player))->SetCallbackEventsMask(ta->player, SL_PLAYEVENT_HEADATEND);
	(*(ta->player))->SetPlayState(ta->player, SL_PLAYSTATE_STOPPED);
	
	return audio;
}
bool OpenSLES::DeleteAudio(void*& audio){
	if(audio == nullptr){return false;}
	Audio* ta = (Audio*)audio;
	(*(ta->player_obj))->Destroy(ta->player_obj);
	delete ta;
	audio = nullptr;

	return true;
}

_GRAPH::TYPE OpenSLES::GetState(void* audio){
	if(audio == nullptr){return _GRAPH::TYPE::NONE;}
	Audio* ta = (Audio*)audio;
	SLuint32 state = 0;
	(*(ta->player))->GetPlayState(ta->player, &state);
	switch(state){
	case SL_PLAYSTATE_PLAYING:	return _GRAPH::TYPE::PLAY;
	case SL_PLAYSTATE_STOPPED:	return _GRAPH::TYPE::STOP;
	case SL_PLAYSTATE_PAUSED:	return _GRAPH::TYPE::PAUSE;
	default:	return _GRAPH::TYPE::NONE;
	}
}
void OpenSLES::Start(void* audio){
	if(audio == nullptr){return;}
	Audio* ta = (Audio*)audio;
	/*SLAndroidSimpleBufferQueueState state;
	(*(ta->player_buf_q))->GetState(ta->player_buf_q, &state);
	if(state.count == 0){
		Graph* tg = (Graph*)(ta->graph);
		(*(ta->player_buf_q))->Enqueue(ta->player_buf_q, tg->buf, tg->size);
	}*/
	(*(ta->player))->SetPlayState(ta->player, SL_PLAYSTATE_PLAYING);
}
void OpenSLES::Pause(void* audio){
	if(audio == nullptr){return;}
	Audio* ta = (Audio*)audio;
	(*(ta->player))->SetPlayState(ta->player, SL_PLAYSTATE_PAUSED);
}
void OpenSLES::Stop(void* audio){
	if(audio == nullptr){return;}
	Audio* ta = (Audio*)audio;
	(*(ta->player))->SetPlayState(ta->player, SL_PLAYSTATE_STOPPED);
	(*(ta->player_buf_q))->Clear(ta->player_buf_q);
	(*(ta->player_buf_q))->Enqueue(ta->player_buf_q, ta->graph->buf, ta->graph->size);
}

void OpenSLES::Seek(void* audio, UINT n){
	if(audio == nullptr){return;}
}

UINT OpenSLES::GetLoop(void* audio){
	if(audio == nullptr){return 0;}
	Audio* ta = (Audio*)audio;
	return ta->loop;
}
void OpenSLES::SetLoop(void* audio, UINT l){
	if(audio == nullptr){return;}
	Audio* ta = (Audio*)audio;
	ta->loop = l;
}

UINT OpenSLES::GetAudioVolume(void* audio){
	if(audio == nullptr){return 0;}
	Audio* ta = (Audio*)audio;

	SLmillibel vol = 0;
	(*(ta->player_vol))->GetVolumeLevel(ta->player_vol, &vol);

	SLmillibel max = 0;
	(*(ta->player_vol))->GetMaxVolumeLevel(ta->player_vol, &max);
	INT range = (INT)max-SL_MILLIBEL_MIN;
	FLOAT v = (INT)vol-SL_MILLIBEL_MIN;
	v = (v/(FLOAT)range)*100.0f;
	vol = v;
	return vol;
}
void OpenSLES::SetAudioVolume(void* audio, UINT v){
	if(audio == nullptr){return;}
	Audio* ta = (Audio*)audio;

	SLmillibel max = 0;
	(*(ta->player_vol))->GetMaxVolumeLevel(ta->player_vol, &max);
	INT range = (INT)max-SL_MILLIBEL_MIN;
	FLOAT per = v;
	per /= 100.0f;

	SLmillibel vol = ((FLOAT)range*per)+SL_MILLIBEL_MIN;
	(*(ta->player_vol))->SetVolumeLevel(ta->player_vol, vol);
}

void OpenSLES::Run(){}

void SLAPIENTRY OpenSLES::bufq_callback(SLAndroidSimpleBufferQueueItf buf_q, void* context){
	Audio* ta = (Audio*)context;
	if(ta->loop > 0){--(ta->loop);}
	if(ta->loop != 0){
		(*(ta->player_buf_q))->Clear(ta->player_buf_q);
		(*(ta->player_buf_q))->Enqueue(ta->player_buf_q, ta->graph->buf, ta->graph->size);
	}
	else{
		(*(ta->player))->SetPlayState(ta->player, SL_PLAYSTATE_STOPPED);
	}
}

void SLAPIENTRY OpenSLES::play_callback(SLPlayItf player, void *context, SLuint32 event){
	if(event & SL_PLAYEVENT_HEADATEND){
		Audio* ta = (Audio*)context;
		if(ta->loop == 0){
			(*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
		}
	}
}