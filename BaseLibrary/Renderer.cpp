#include "Renderer.h"

#include "..\\Interface\\Base_Output_Graphics.h"
#include "..\\Interface\\Base_Output_Sounds.h"
#include "..\\Interface\\Base_Timer.h"

#include "..\\Common\\variable.h"

#include "Object.h"
#include "Module.h"
#include "ObjectManager.h"

Graphic_Renderer::Graphic_Renderer(){}
Graphic_Renderer::~Graphic_Renderer(){}

struct GCHAR_UNIFORM{
	MATRIX4x4 world;
	FLOAT4 solid_color;
	FLOAT4 outline_color;
	FLOAT outline_size;
	UINT buf_size;
	UINT offset;
};

void Graphic_Renderer::Batch(Mesh** st, Mesh** ed){
	//frustum culling
	if((*st)->Type() == _MODULE::TYPE::MText){
		RENDER_OBJECT ren_obj = {};
		ren_obj.type = _MODULE::TYPE::MPolygon;//(*st)->Type();
		ren_obj.shader = (*st)->GetShader();

		UINT c_remainder = (sizeof(GCHAR_UNIFORM)%16 != 0)?16-(sizeof(GCHAR_UNIFORM)%16):0;
		UINT c_size = sizeof(GCHAR_UNIFORM)+c_remainder;

		UINT v_idx = 0;
		UINT i_idx = 0;
		UINT c_idx = 0;
		UINT g_idx = 0;

		GCHAR_UNIFORM g_uni = {};

		for(UINT i = 0 ; st[i] != *ed ; ++i){
			MATRIX4x4 obj_world = ((TextMesh*)st[i])->GetJoint()->world;
			for(UINT j = 0 ; j<((TextMesh*)st[i])->GetGlyphNum() ; ++j){
				GCHAR* tgc = ((TextMesh*)st[i])->GetGlyph(j);

				g_uni.world = MATCAL::MultipleMatrix(tgc->world, obj_world);
				g_uni.outline_size = tgc->outline_size/g_uni.world._22;
				g_uni.solid_color = tgc->solid_color;
				g_uni.outline_color = tgc->outline_color;
				g_uni.buf_size = tgc->glyph->size;
				g_uni.offset = g_idx;

				_O_Graphics::GetSingleton()->tBufferWrite(0, sizeof(VERTEX)*v_idx, sizeof(VERTEX)*tgc->glyph->vtx_num, tgc->glyph->vtx, &g_uni.outline_size);
				_O_Graphics::GetSingleton()->tBufferWrite(1, sizeof(BYTE)*v_idx, sizeof(BYTE)*tgc->glyph->vtx_num, nullptr, (void*)c_idx);
				_O_Graphics::GetSingleton()->tBufferWrite(2, sizeof(UINT)*i_idx, sizeof(UINT)*tgc->glyph->idx.num, tgc->glyph->idx.index, (void*)v_idx);

				_O_Graphics::GetSingleton()->tBufferWrite(3, c_size*c_idx, sizeof(GCHAR_UNIFORM), &g_uni);
				_O_Graphics::GetSingleton()->tBufferWrite(5, sizeof(BYTE)*g_idx, sizeof(BYTE)*tgc->glyph->size, tgc->glyph->buf);

				v_idx += tgc->glyph->vtx_num;
				i_idx += tgc->glyph->idx.num;
				c_idx += 1;
				g_idx += tgc->glyph->size;
			}
		}

		ren_obj.vertex_num = v_idx;
		ren_obj.index_num = i_idx;

		_O_Graphics::GetSingleton()->RenderObject(ren_obj);
	}
	else{
		RENDER_OBJECT ren_obj = {};
		ren_obj.type = (*st)->Type();
		ren_obj.shader = (*st)->GetShader();
		ren_obj.texture_num = 0;

		for(UINT i = 0 ; i<16 ; ++i){ren_obj.textures[i] = 0;}

		UINT vtx_idx = 0;
		UINT idx_idx = 0;
		UINT uni_idx = 0;

		for(UINT i = 0 ; st[i] != *ed ; ++i){
			if(((PolygonMesh*)st[i])->UpdateUniformBlock != nullptr){((PolygonMesh*)st[i])->UpdateUniformBlock(st[i]->parent, ren_obj.textures, &(ren_obj.texture_num));}
			
			_O_Graphics::GetSingleton()->tBufferWrite(0, sizeof(VERTEX)*vtx_idx, sizeof(VERTEX)*((PolygonMesh*)st[i])->GetMaxVertex(), ((PolygonMesh*)st[i])->GetVertex(0));
			_O_Graphics::GetSingleton()->tBufferWrite(1, sizeof(BYTE)*vtx_idx, sizeof(BYTE)*((PolygonMesh*)st[i])->GetMaxVertex(), nullptr, (void*)i);
			if(((PolygonMesh*)st[i])->GetMaxIndex() > 0){
				_O_Graphics::GetSingleton()->tBufferWrite(2, sizeof(UINT)*idx_idx, sizeof(UINT)*((PolygonMesh*)st[i])->GetMaxIndex(), ((PolygonMesh*)st[i])->GetIndex(0), (void*)vtx_idx);
			}
			_O_Graphics::GetSingleton()->tBufferWrite(3, uni_idx, ((PolygonMesh*)st[i])->GetUniformBlockSize(), ((PolygonMesh*)st[i])->GetUniformBlock());
			vtx_idx += ((PolygonMesh*)st[i])->GetMaxVertex();
			idx_idx += ((PolygonMesh*)st[i])->GetMaxIndex();
			uni_idx += ((PolygonMesh*)st[i])->GetUniformBlockSize();
			uni_idx += (uni_idx%16 != 0)?16-(uni_idx%16):0;
		}

		ren_obj.vertex_num = vtx_idx;
		ren_obj.index_num = idx_idx;

		_O_Graphics::GetSingleton()->RenderObject(ren_obj);
	}
}

void Graphic_Renderer::Render(){
	if(_O_Graphics::GetSingleton() == nullptr){return;}

	if(!_O_Graphics::GetSingleton()->Clear()){return;}

	Camera** carr = nullptr;
	ObjectManager::GetSingleton()->GetCamera(carr);
	UINT cnum = ObjectManager::GetSingleton()->GetCameraNum();

	Mesh** marr = nullptr;
	ObjectManager::GetSingleton()->GetMesh(marr);
	UINT mnum = ObjectManager::GetSingleton()->GetMeshNum();

	Mesh** batch_st = nullptr;
	Mesh** batch_ed = nullptr;

	for(UINT i = 0 ; i<cnum ; ++i){	//loop for screen num
		carr[i]->SetScreen();
		SCREEN scr = carr[i]->GetScreen();
		_O_Graphics::GetSingleton()->Start(&scr);	//start
		for(UINT j = 0 ; j<mnum ; ++j){
			//world update maybe if outside
			if(carr[i]->parent->GetMapID() > marr[j]->parent->GetMapID()){continue;}
			else if(carr[i]->parent->GetMapID() < marr[j]->parent->GetMapID()){break;}

			if(batch_st == nullptr){batch_st = &(marr[j]);}
			else{
				batch_ed = &(marr[j]);
				Mesh* st = *batch_st;
				Mesh* cur = *batch_ed;
				if(!((st->GetShader() == cur->GetShader()) && (st->Type() == cur->Type()))){
					Batch(batch_st, batch_ed);
					batch_st = &(marr[j]);
					batch_ed = nullptr;
				}
			}
		}
		if(batch_st != nullptr){
			if(batch_ed == nullptr){batch_ed = batch_st;}
			Batch(batch_st, batch_ed+1);
			batch_st = nullptr;
			batch_ed = nullptr;
		}
		_O_Graphics::GetSingleton()->End();	//end
	}
	_O_Graphics::GetSingleton()->Present();	//present

											//delete[] marr;
											//delete[] carr;
}

void Sound_Renderer::Render(){
	Audio** aarr = nullptr;
	ObjectManager::GetSingleton()->GetAudio(aarr);
	UINT anum = ObjectManager::GetSingleton()->GetAudioNum();
	for(UINT i = 0 ; i<anum ; ++i){
		/*if(aarr[i]->GetState() != _O_Sounds::GetSingleton()->GetState(aarr[i]->GetAudio())){
			switch(aarr[i]->GetState()){
			case _GRAPH::TYPE::PLAY:	_O_Sounds::GetSingleton()->Start(aarr[i]->GetAudio());	break;
			case _GRAPH::TYPE::STOP:	_O_Sounds::GetSingleton()->Stop(aarr[i]->GetAudio());	break;
			case _GRAPH::TYPE::PAUSE:	_O_Sounds::GetSingleton()->Pause(aarr[i]->GetAudio());	break;
			}
		}*/
	}
	_O_Sounds::GetSingleton()->Run();

	//delete[] aarr;
}

Renderer::Renderer(){}
Renderer::~Renderer(){
	Graphic_Renderer::Destroy();
	Sound_Renderer::Destroy();
}

void Renderer::Animation(){}
void Renderer::Render(){
	if(_O_Graphics::GetSingleton() != nullptr){
		Graphic_Renderer::GetSingleton()->Render();
	}
	if(_O_Sounds::GetSingleton() != nullptr){
		Sound_Renderer::GetSingleton()->Render();
	}
}