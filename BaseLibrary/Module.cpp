#include "Object.h"
#include "Module.h"

#include "Font.h"
#include "..\\Interface\\Base_Output_Graphics.h"
#include "..\\Interface\\Base_Output_Sounds.h"

//joint
Joint::Joint() : 
	id(0), parent(nullptr),
	position({}), rotation({}), scale({1.0f, 1.0f, 1.0f}), 
	world(MATRIX4x4::Initialize()),
	calcul(2)
{}
Joint::~Joint(){}

void Joint::SetWorld(INT cal_state){
	if(cal_state == calcul || calcul == 3){return;}
	if(parent != nullptr){
		parent->SetWorld(cal_state);
		world = MATCAL::WorldMatrix(position, rotation, scale, parent->world);
	}
	else{
		world = MATCAL::WorldMatrix(position, rotation, scale);
	}
	calcul = cal_state;
}

//Module Base
Module::Module() : parent(nullptr), joint(nullptr), id(0), active(true){}
Module::~Module(){}

bool Module::IsActive(){
	if(parent != nullptr){
		if(parent->active == false){return false;}
		return active;
	}
	return false;
}
void Module::Active(){active = true;}
void Module::Inactive(){active = false;}

Joint* Module::GetJoint(){
	if(joint == nullptr){
		if(parent == nullptr){return nullptr;}
		return parent->GetJoint();
	}
	return joint;
}
void Module::CreateJoint(){
	if(joint != nullptr){return;}
	joint = new Joint;
	joint->position = FLOAT3::zero();
	joint->rotation = FLOAT3::zero();
	joint->scale = {1.0f, 1.0f, 1.0f};
	joint->world = MATRIX4x4::Initialize();
	joint->parent = nullptr;
}
void Module::DeleteJoint(){
	if(joint != nullptr){
		delete joint;
		joint = nullptr;
	}
}

//camera
Camera::Camera()
	: eye(FLOAT3::zero()), lookat(FLOAT3::zero()), up(FLOAT3::zero()),
	x(0), y(0), w(0), h(0),
	fovy(0.0f), z_range(FLOAT2::zero()), type(_PROJECTION::TYPE::Perspective),
	view(MATRIX4x4::Initialize()), proj(MATRIX4x4::Initialize()),
	shd(nullptr), screen_buffer(nullptr), screen_buffer_texture(nullptr), 
	screen_listhead(nullptr), screen_counter(nullptr)
{}
Camera::~Camera(){}

_MODULE::TYPE Camera::Type(){return _MODULE::TYPE::Camera;}

SCREEN Camera::GetScreen(){
	return {x, y, w, h, (INT)(z_range.y - z_range.x), view, proj, shd, vtx, idx, 
		screen_buffer, screen_buffer_texture, screen_listhead, screen_counter};
}
void Camera::SetScreen(){
	//joint.pos = eye / joint.rot = lookat / up = default {0, 1, 0} can set another
	view = MATCAL::ViewMatrix(eye, lookat, up);
	if(type == _PROJECTION::TYPE::Perspective){
		proj = MATCAL::PerspectiveProjectionMatrix((FLOAT)w/(FLOAT)h, fovy, z_range.x, z_range.y);
	}
	else if(type == _PROJECTION::TYPE::Ortho){
		FLOAT2 half = {(FLOAT)w*0.5f, (FLOAT)h*0.5f};
		/*proj = MATCAL::OrthoProjectionMatrix(
			lookat.x-half.x, lookat.x+half.x,
			lookat.y-half.y, lookat.y+half.y, 
			eye.z+z_range.x, eye.z+z_range.y);*/
		proj = MATCAL::OrthoProjectionMatrix(-half.x, half.x, -half.y, half.y, z_range.x, z_range.y);
	}
	else{
		proj = MATRIX4x4::Initialize();
	}
}

void Camera::SetScreenRange(UINT _x, UINT _y, UINT _w, UINT _h){
	x = _x; y = _y; w = _w; h = _h;

	_O_Graphics::GetSingleton()->SetScreenBuffer(w, h, screen_buffer, screen_buffer_texture, screen_listhead, screen_counter);

	//FLOAT t_w = _w*0.5f, t_h = _h*0.5f;
	FLOAT t_w = 1.0f, t_h = 1.0f;
	vtx[0] = {{-t_w,  t_h, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 0.0f}};
	vtx[1] = {{ t_w,  t_h, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 0.0f}};
	vtx[2] = {{-t_w, -t_h, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 1.0f}};
	vtx[3] = {{ t_w, -t_h, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 1.0f}};

	idx[0] = 0;	idx[1] = 1;	idx[2] = 2;
	idx[3] = 1;	idx[4] = 2;	idx[5] = 3;
}
void Camera::SetViewMatrix(FLOAT3 _eye, FLOAT3 _lookat, FLOAT3 _up){
	eye = _eye; lookat = _lookat; up = _up;
}
void Camera::SetPerspectiveProjMatrix(FLOAT _fovy, FLOAT2 _z_range){
	fovy = _fovy; z_range = _z_range; type = _PROJECTION::TYPE::Perspective;
}
void Camera::SetOrthoProjMatrix(FLOAT _fovy, FLOAT2 _z_range){
	fovy = _fovy; z_range = _z_range; type = _PROJECTION::TYPE::Ortho;
}

SHADER* Camera::GetShader(){return shd;}
void Camera::SetShader(SHADER* shader){shd = shader;}

//Mesh
PolygonMesh::PolygonMesh() : 
	vtx(nullptr), vtx_num(0), idx(nullptr), shd(nullptr), type(_MODULE::TYPE::MPolygon),
	uniform_block(nullptr), uniform_block_size(0) {}
PolygonMesh::~PolygonMesh(){
	DeleteVertex();
	DeleteIndex();
	ReleaseShader();
}

//RENDER_OBJECT PolygonMesh::GetMesh(){return {type, (GetJoint() == nullptr)?MATRIX4x4::Initialize():GetJoint()->world, vtx, vtx_num, idx, mtr, shd};}

_MODULE::TYPE PolygonMesh::Type(){return type;}
void PolygonMesh::SetPolygon(){type = _MODULE::TYPE::MPolygon;}
void PolygonMesh::SetLine(){type = _MODULE::TYPE::MLine;}
void PolygonMesh::SetPoint(){type = _MODULE::TYPE::MPoint;}

void PolygonMesh::CreateVertex(UINT num){
	if(vtx != nullptr){
		delete[] vtx;
		vtx = nullptr;
		vtx_num = 0;
	}
	vtx = new VERTEX[num];
	vtx_num = num;

	for(UINT i = 0 ; i<num ; ++i){
		vtx[i] = {};
	}
}

void PolygonMesh::DeleteVertex(){
	if(vtx != nullptr){
		delete[] vtx;
		vtx = nullptr;
		vtx_num = 0;
	}
}

VERTEX* PolygonMesh::GetVertex(UINT i){
	if(i >= vtx_num){return nullptr;}
	return &(vtx[i]);
}

UINT PolygonMesh::GetMaxVertex(){return vtx_num;}

void PolygonMesh::CreateIndex(UINT num){
	if(idx == nullptr){
		idx = new INDEXED;
		idx->index = nullptr;
		idx->num = 0;
	}
	if(idx->index != nullptr){
		delete[] (idx->index);
		idx->num = 0;
	}
	idx->index = new UINT[num];
	idx->num = num;

	for(UINT i = 0 ; i<num ; ++i){
		*(idx->index) = 0;
	}
}

void PolygonMesh::DeleteIndex(){
	if(idx != nullptr){
		if(idx->index != nullptr){
			delete[] (idx->index);
			idx->num = 0;
		}
		delete idx;
		idx = nullptr;
	}
}

UINT PolygonMesh::GetMaxIndex(){
	if(idx == nullptr){return 0;}
	return idx->num;
}

UINT* PolygonMesh::GetIndex(UINT i){
	if(i >= idx->num){return nullptr;}
	return &(idx->index[i]);
}

void PolygonMesh::SetShader(SHADER* shader){shd = shader;}
SHADER* PolygonMesh::GetShader(){return shd;}
void PolygonMesh::ReleaseShader(){}

void PolygonMesh::SetUniformBlock(void* uni_b, UINT size, void (*update)(void*, UINT*, UINT*)){
	uniform_block = uni_b;
	uniform_block_size = size;
	UpdateUniformBlock = update;
}
void* PolygonMesh::GetUniformBlock(){return uniform_block;}
UINT PolygonMesh::GetUniformBlockSize(){return uniform_block_size;}

//Listenor
_MODULE::TYPE Listener::Type(){return _MODULE::TYPE::NONE;}

//Audio
Audio::Audio() : audio(nullptr), graph(nullptr){}
Audio::~Audio(){ReleaseAudio();}

void Audio::CreateAudio(void* g){
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	graph = g;
	_O_Sounds::GetSingleton()->CreateAudio(audio, graph, 0);
}
void Audio::ReleaseAudio(){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	graph = nullptr;
	_O_Sounds::GetSingleton()->DeleteAudio(audio);
}

void Audio::Play(){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	_O_Sounds::GetSingleton()->Start(audio);
}
void Audio::Stop(){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	_O_Sounds::GetSingleton()->Stop(audio);
}
void Audio::Pause(){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	_O_Sounds::GetSingleton()->Pause(audio);
}

UINT Audio::GetSeek(){
	if(audio == nullptr){return 0;}
	return 0;
}
void Audio::SetSeek(UINT t){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	_O_Sounds::GetSingleton()->Seek(audio, t);
}

UINT Audio::GetLoop(){
	if(audio == nullptr){return 0;}
	if(_O_Sounds::GetSingleton() == nullptr){return 0;}
	return _O_Sounds::GetSingleton()->GetLoop(audio);
}
void Audio::SetLoop(UINT l){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	_O_Sounds::GetSingleton()->SetLoop(audio, l);
}

UINT Audio::GetVolume(){
	if(audio == nullptr){return 0;}
	if(_O_Sounds::GetSingleton() == nullptr){return 0;}
	return _O_Sounds::GetSingleton()->GetAudioVolume(audio);
}
void Audio::SetVolume(UINT v){
	if(audio == nullptr){return;}
	if(_O_Sounds::GetSingleton() == nullptr){return;}
	if(v > 100){v = 100;}
	_O_Sounds::GetSingleton()->SetAudioVolume(audio, v);
}

_GRAPH::TYPE Audio::GetState(){
	if(audio == nullptr){return _GRAPH::TYPE::NONE;}
	if(_O_Sounds::GetSingleton() == nullptr){return _GRAPH::TYPE::NONE;}
	return _O_Sounds::GetSingleton()->GetState(audio);
}

void* Audio::GetGraph(){return graph;}
void* Audio::GetAudio(){return audio;}

_MODULE::TYPE Audio::Type(){return _MODULE::TYPE::Sound;}

//Collider Base
Collider::Collider() : type(_COLLISION::TYPE::Block), layer(0), layer_mask(0){}
Collider::~Collider(){}

//PloygonCollider
PolygonCollider::PolygonCollider() : point(nullptr), num(0){}
PolygonCollider::~PolygonCollider(){DeletePoint();}

void PolygonCollider::CreatePoint(UINT n){
	if(point != nullptr){
		delete[] point;
		point = nullptr;
		num = 0;
	}
	point = new FLOAT3[n];
	num = n;
}
void PolygonCollider::DeletePoint(){
	if(point != nullptr){
		delete[] point;
		point = nullptr;
		num = 0;
	}
}
FLOAT3* PolygonCollider::GetPoint(UINT n){
	if(n >= num){return nullptr;}
	return &(point[n]);
}
UINT PolygonCollider::GetMaxPoint(){return num;}

_MODULE::TYPE PolygonCollider::Type(){return _MODULE::TYPE::CPolygon;}

//CircleCollider
_MODULE::TYPE CircleCollider::Type(){return _MODULE::TYPE::CCircle;}

//RigidBody
_MODULE::TYPE Rigidbody::Type(){return _MODULE::TYPE::Rigidbody;}

//Text
_MODULE::TYPE TextMesh::Type(){return _MODULE::TYPE::MText;}

TextMesh::TextMesh() : font(nullptr), gchar(nullptr), gchar_size(0), 
						text(nullptr), text_len(0), w(0.0f), h(0.0f), now_sort(7), box(FLOAT2::zero()),
						solid_color(FLOAT4::zero()), outline_color(FLOAT4::zero()), shd(nullptr){}
TextMesh::~TextMesh(){
	DeleteText();
}

void TextMesh::DeleteText(){
	if(gchar != nullptr){
		delete[] gchar;
		gchar = nullptr;
		gchar_size = 0;
	}
	if(text != nullptr){
		delete[] text;
		text = nullptr;
		text_len = 0;
	}

	w = 0.0f;
	h = 0.0f;
}

void TextMesh::SetBox(FLOAT2 b){box = b;}
void TextMesh::SetFont(void* f){font = (Font*)f;}
void TextMesh::SetText(wchar_t* t){
	if(font == nullptr){return;}
	DeleteText();
	
	for(UINT i = 0 ; t[i] != 0x00 ; ++i){
		if(((t[i]&0xFF80) == 0x0000) && (0x0A == (t[i]&0x7F))){
			++text_len;
			continue;
		}
		if(((t[i]&0xFF80) == 0x0000) && (32 > (t[i]&0x7F) || (t[i]&0x7F) > 126)){continue;}
		if((t[i] & 0xDC00) == 0xD800 && (t[i+1] & 0xDC00) == 0xDC00){
			text_len+=2;
			++gchar_size;
			++i;
			continue;
		}
		++text_len;
		if(t[i] != 32 && t[i] != 0x3000){++gchar_size;}
	}

	text = new wchar_t[text_len+1];
	gchar = new GCHAR[gchar_size];
	text[text_len] = 0x00;

	VECTOR2 pos = VECTOR2::zero();
	FLOAT2 size = {font->GetSpace(), font->GetVerticalAdvance()};

	FLOAT4 s_color = solid_color;
	FLOAT4 o_color = outline_color;
	FLOAT o_size = outline_size;

	pos.y -= size.y;
	for(UINT i = 0, j = 0, k = 0 ; /*i <= text_len*/t[i] != 0x00 ; ++i){
		if(((t[i]&0xFF80) == 0x0000) && (0x0A == (t[i]&0x7F))){
			text[i-k] = t[i];
			w = (pos.x>w)?pos.x:w;
			pos.y -= size.y;
			pos.x = 0.0f;
			continue;
		}
		if(((t[i]&0xFF80) == 0x0000) && (32 > (t[i]&0x7F) || (t[i]&0x7F) > 126)){++k; continue;}
		text[i-k] = t[i];
		UINT code = 0;
		if((t[i] & 0xDC00) == 0xD800 && (t[i+1] & 0xDC00) == 0xDC00){
			text[i+1-k] = t[i+1];
			code = (t[i]<<16) || t[i+1];
			++i;
			continue;
		}
		if(t[i] != 32 && t[i] != 0x3000){code = t[i];}
		else{pos.x += (t[i]==32)?size.x:(size.x*2.0f); continue;}

		gchar[j] = {nullptr, MATRIX4x4::Initialize(), o_size, s_color, o_color};
		gchar[j].world._41 = pos.x;
		gchar[j].world._42 = pos.y;
		if(font->GetGlyph(code, gchar[j].glyph)){
			if(box.x > 0.0f){
				FLOAT t_pos_x = pos.x+gchar[j].glyph->advanceWidth;
				FLOAT t_box_x = box.x/GetJoint()->scale.x;
				if(t_pos_x > t_box_x){
					w = (pos.x>w)?pos.x:w;
					pos.y -= size.y;
					pos.x = 0.0f;
					gchar[j].world._41 = pos.x;
					gchar[j].world._42 = pos.y;
				}
			}
			pos.x += gchar[j].glyph->advanceWidth;
		}
		else{
			pos.x += size.x;
		}
		++j;
	}
	
	w = (pos.x>w)?pos.x:w;
	h = -pos.y;

	SetSortType(now_sort);
}
void TextMesh::SetSize(FLOAT size){
	if(font == nullptr){GetJoint()->scale = {1.0f, 1.0f, 1.0f}; return;}
	FLOAT h = font->GetVerticalAdvance();
	FLOAT s = size/h;
	GetJoint()->scale = {s, s, s};
}
void TextMesh::SetSolidColor(FLOAT4 c){
	solid_color = c;
	for(INT i = 0 ; i<gchar_size ; ++i){gchar[i].solid_color = c;}
}
void TextMesh::SetOutlineColor(FLOAT4 c){
	outline_color = c;
	for(INT i = 0 ; i<gchar_size ; ++i){gchar[i].outline_color = c;}
}
void TextMesh::SetOutlineSize(FLOAT s){
	outline_size = s;
	for(INT i = 0 ; i<gchar_size ; ++i){gchar[i].outline_size = s;}
}
void TextMesh::SetSortType(UINT s){
	now_sort = s;

	INT vert = 0;
	INT horz = 0;

	switch(now_sort){
	case 1:	vert = -1;	horz = 1;	break;
	case 2:	vert = 0;	horz = 1;	break;
	case 3:	vert = 1;	horz = 1;	break;
	case 4:	vert = -1;	horz = 0;	break;
	case 5:	vert = 0;	horz = 0;	break;
	case 6:	vert = 1;	horz = 0;	break;
	case 7:	vert = -1;	horz = -1;	break;
	case 8:	vert = 0;	horz = -1;	break;
	case 9:	vert = 1;	horz = -1;	break;
	default: now_sort = 7; vert = -1; horz = -1; break;
	}

	if(gchar_size == 0){return;}

	VECTOR2 size = VECTOR2::zero();
	VECTOR2 pos = VECTOR2::zero();
	switch(horz){
	case -1:	size.y = 0.0f;		break;
	case  0:	size.y = h*0.5f;	break;
	case  1:	size.y = h;			break;
	}
	for(INT i = 0, j = 0 ;  ; ++i){
		if((i==gchar_size) || (gchar[i].world._42 != pos.y)){
			switch(vert){
			case -1:	size.x = 0.0f;			break;
			case  0:	size.x = -size.x*0.5f;	break;
			case  1:	size.x = -size.x;		break;
			}
			for( ; j<i ; ++j){
				//gchar[j].pos += size;
				gchar[j].world._41 += size.x;
				gchar[j].world._42 += size.y;
			}
			size.x = 0.0f;
			if(i<gchar_size){
				pos.y = gchar[i].world._42;
			}
			else{break;}
		}
		size.x = gchar[i].world._41 + gchar[i].glyph->advanceWidth;
	}
}
void TextMesh::SetShader(SHADER* shader){shd = shader;}

SHADER* TextMesh::GetShader(){return shd;}

GCHAR* TextMesh::GetGlyph(UINT i){
	if(i >= gchar_size){return nullptr;}
	return &(gchar[i]);
}
UINT TextMesh::GetGlyphNum(){return gchar_size;}
UINT TextMesh::GetTextLen(){return text_len;}

FLOAT TextMesh::GetMaxWidth(){return w;}
FLOAT TextMesh::GetMaxHeight(){return h;}

RENDER_TEXT TextMesh::GetMesh(){
	return {
		_MODULE::TYPE::MPolygon, GetJoint()->world, 
		gchar, gchar_size, shd
	};
}