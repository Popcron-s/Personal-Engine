#include "header.h"

#include <Windows.h>
#include <math.h>

#define M_UNIFORM_FUNTION(num) 															\
void CollisionTestPage::UpdateUniformBlock##num##(void* s, UINT* tex, UINT* tex_idx){	\
	CollisionTestPage* self = (CollisionTestPage*)s;									\
	self->m_uni_block[num].world = self->m_mesh[num]->GetJoint()->world;				\
																						\
	UINT t_idx = 0;																		\
																						\
	for(UINT i = 0 ; i<(*tex_idx) ; ++i){												\
		if(tex[i] == (UINT)(self->texture[self->texture_idx[num]])){					\
			t_idx = i;																	\
			break;																		\
		}																				\
	}																					\
																						\
	if(t_idx == 0){																		\
		tex[*tex_idx] = (UINT)(self->texture[self->texture_idx[num]]);					\
		self->m_uni_block[num].texture_idx = (*tex_idx)++;								\
	}																					\
	else{																				\
		self->m_uni_block[num].texture_idx = t_idx;										\
	}																					\
}

CollisionTestPage::CollisionTestPage(){
	InitModuleList();

	SetID(0x01, 0x0, 0x0);

	velocity = 0.0f;
	accel = 0.0f;

	texture[0] = PCS::GetResource("resources/image/flier.png");
	texture[1] = PCS::GetResource("resources/image/rock1.png");
	texture[2] = PCS::GetResource("resources/image/rock2.png");
	texture[3] = PCS::GetResource("resources/image/rock3.png");

	for(INT i = 0 ; i<24 ; ++i){theta[i] = 15*i;}

	PlayerCreate();
	MeteorCreate();
	MeteorRebatch();

	info_text = new TextMesh();
	info_text->parent = this;
	info_text->CreateJoint();
	info_text->GetJoint()->parent = this->GetJoint();
	info_text->GetJoint()->position = {-1200.0f, 0.0f, 100.0f};
	info_text->SetFont(PCS::GetResource("resources/font/NanumBarunGothic.ttf"));
	info_text->SetSortType(4);
	info_text->SetSize(64.0f);
	info_text->SetBox({0.0f, 0.0f});
	info_text->SetSolidColor({0.0f, 0.0f, 0.0f, 1.0f});
	info_text->SetOutlineColor({1.0f, 1.0f, 1.0f, 1.0f});
	info_text->SetOutlineSize(3.0f);
	info_text->SetText(L"W - 전진\nS - 후진\nA/D - 회전\n\nSpace - 재배치");
	info_text->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)info_text);
}
CollisionTestPage::~CollisionTestPage(){
	RemoveModule((Module*)p_mesh);
	delete p_mesh;

	RemoveModule((Module*)p_col);
	delete p_col;

	for(INT i = 0 ; i<10 ; ++i){
		RemoveModule((Module*)m_mesh[i]);
		delete m_mesh[i];

		RemoveModule((Module*)m_col[i]);
		delete m_col[i];
	}

	RemoveModule((Module*)info_text);
	delete info_text;

	ReleaseModuleList();
}

void CollisionTestPage::PlayerCreate(){
	p_mesh = new PolygonMesh();
	p_mesh->parent = this;
	p_mesh->CreateJoint();
	p_mesh->GetJoint()->parent = this->GetJoint();
	p_mesh->GetJoint()->scale = {64.0f, 64.0f, 1.0f};
	p_mesh->CreateVertex(3);
	*(p_mesh->GetVertex(0)) = {{-1.0f,  1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 1.0f}};
	*(p_mesh->GetVertex(1)) = {{ 1.0f,  0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.5f, 0.0f}};
	*(p_mesh->GetVertex(2)) = {{-1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 1.0f}};

	p_mesh->CreateIndex(3);
	*(p_mesh->GetIndex(0)) = 0;
	*(p_mesh->GetIndex(1)) = 1;
	*(p_mesh->GetIndex(2)) = 2;

	p_mesh->SetShader((SHADER*)PCS::GetResource("default.shd"));
	p_uni_block = {p_mesh->GetJoint()->world, 0};
	p_mesh->SetUniformBlock(&p_uni_block, sizeof(UNI_BLOCK), &UpdateUniformBlockP);
	RegistModule((Module*)p_mesh);

	p_col = new PolygonCollider;
	p_col->parent = this;
	p_col->CreateJoint();
	p_col->GetJoint()->parent =p_mesh->GetJoint();
	p_col->CreatePoint(3);
	*(p_col->GetPoint(0)) = {-1.0f,  1.0f, 0.0f};
	*(p_col->GetPoint(1)) = { 1.0f,  0.0f, 0.0f};
	*(p_col->GetPoint(2)) = {-1.0f, -1.0f, 0.0f};
	RegistModule((Module*)p_col);
}
void CollisionTestPage::MeteorCreate(){
	for(INT i = 0 ; i<10 ; ++i){
		m_mesh[i] = new PolygonMesh();
		m_mesh[i]->parent = this;
		m_mesh[i]->CreateJoint();
		m_mesh[i]->GetJoint()->parent = this->GetJoint();

		m_mesh[i]->SetShader((SHADER*)PCS::GetResource("default.shd"));
		m_uni_block[i] = {m_mesh[i]->GetJoint()->world, 0};
		RegistModule((Module*)m_mesh[i]);

		m_col[i] = new PolygonCollider;
		m_col[i]->parent = this;
		m_col[i]->CreateJoint();
		m_col[i]->GetJoint()->parent = m_mesh[i]->GetJoint();
		RegistModule((Module*)m_col[i]);
	}

	m_mesh[0]->SetUniformBlock(&m_uni_block[0], sizeof(UNI_BLOCK), &UpdateUniformBlock0);
	m_mesh[1]->SetUniformBlock(&m_uni_block[1], sizeof(UNI_BLOCK), &UpdateUniformBlock1);
	m_mesh[2]->SetUniformBlock(&m_uni_block[2], sizeof(UNI_BLOCK), &UpdateUniformBlock2);
	m_mesh[3]->SetUniformBlock(&m_uni_block[3], sizeof(UNI_BLOCK), &UpdateUniformBlock3);
	m_mesh[4]->SetUniformBlock(&m_uni_block[4], sizeof(UNI_BLOCK), &UpdateUniformBlock4);
	m_mesh[5]->SetUniformBlock(&m_uni_block[5], sizeof(UNI_BLOCK), &UpdateUniformBlock5);
	m_mesh[6]->SetUniformBlock(&m_uni_block[6], sizeof(UNI_BLOCK), &UpdateUniformBlock6);
	m_mesh[7]->SetUniformBlock(&m_uni_block[7], sizeof(UNI_BLOCK), &UpdateUniformBlock7);
	m_mesh[8]->SetUniformBlock(&m_uni_block[8], sizeof(UNI_BLOCK), &UpdateUniformBlock8);
	m_mesh[9]->SetUniformBlock(&m_uni_block[9], sizeof(UNI_BLOCK), &UpdateUniformBlock9);
}

void CollisionTestPage::MeteorRebatch(){
	for(INT i = 0 ; i<10 ; ++i){
		FLOAT s = rand()%256+64;	//32~128

		ShuffleTheta();

		UINT angle = rand()%10+3;

		std::qsort(theta, angle, sizeof(FLOAT), 
				   [](const void* a, const void* b)->int{
					   FLOAT t1 = *(FLOAT*)a;
					   FLOAT t2 = *(FLOAT*)b;
					   if(t1 > t2){return 1;}
					   else if(t1 < t2){return -1;}
					   else{return 0;}
				   });

		m_mesh[i]->GetJoint()->position = {(FLOAT)(rand()%2400-1200), (FLOAT)(rand()%1000-500), i+1.0f};
		m_mesh[i]->GetJoint()->scale = {s, s, 1.0f};
		m_mesh[i]->CreateVertex(angle+1);
		m_mesh[i]->CreateIndex(angle*3);
		m_col[i]->CreatePoint(angle);

		*(m_mesh[i]->GetVertex(0)) = {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.5f, 0.5f}};

		for(INT j = 0 ; j<angle ; ++j){
			FLOAT2 rtheta = {cos(RADIAN(theta[j])), sin(RADIAN(theta[j]))};
			*(m_mesh[i]->GetVertex(j+1)) = {{rtheta.x, rtheta.y, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {(rtheta.x+1.0f)*0.5f, (rtheta.y+1.0f)*0.5f}};
			*(m_col[i]->GetPoint(j)) = {rtheta.x, rtheta.y, 0.0f};

			*(m_mesh[i]->GetIndex((j*3)+0)) = 0;
			*(m_mesh[i]->GetIndex((j*3)+1)) = j+1;
			*(m_mesh[i]->GetIndex((j*3)+2)) = j+2;
		}
		*(m_mesh[i]->GetIndex((angle*3)-1)) = 1;

		texture_idx[i] = rand()%3+1;//1~3
		angluar_velocity[i] = RADIAN((FLOAT)(rand()%13-6));	//-6~6
	}
}

void CollisionTestPage::ShuffleTheta(){
	FLOAT t = 0.0f;
	for(INT i = 0 ; i<24 ; ++i){
		INT r = rand()%24;
		t = theta[i];
		theta[i] = theta[r];
		theta[r] = t;
	}
};

void CollisionTestPage::Rebatch(){
	if(PCS::GetKeyboard()->KEY[VK_SPACE] == 0x81){
		p_mesh->GetJoint()->position = {0.0f, 0.0f, 0.0f};
		velocity = 0.0f;
		accel = 0.0f;
		MeteorRebatch();
	}
}
void CollisionTestPage::InputUpdate(){
	FLOAT3 dir = FLOAT3::zero();
	FLOAT tick = PCS::GetTick()*0.001f;
	FLOAT coef = 250.0f;

	accel = 0.0f;
	if(PCS::GetKeyboard()->KEY['W'] & 0x80){accel = coef*1.0f;}
	if(PCS::GetKeyboard()->KEY['S'] & 0x80){accel = coef*-1.0f;}
	if(PCS::GetKeyboard()->KEY['A'] & 0x80){p_mesh->GetJoint()->rotation.z += RADIAN(3);}
	if(PCS::GetKeyboard()->KEY['D'] & 0x80){p_mesh->GetJoint()->rotation.z -= RADIAN(3);}

	if(accel == 0.0f && velocity != 0.0f){
		velocity += ((velocity > 0.0f)?-1.0f:1.0f)*tick*coef;
		velocity = (abs(velocity) < (0.1f*coef))?0.0f:velocity;
	}

	if(accel != 0.0f){
		velocity += accel*tick*coef;
		if(velocity > 1.0f*coef){velocity = 1.0f*coef;}
		else if(velocity < -1.0f*coef){velocity = -1.0f*coef;}
	}

	dir.x = cos(p_mesh->GetJoint()->rotation.z);
	dir.y = sin(p_mesh->GetJoint()->rotation.z);

	p_mesh->GetJoint()->position += VECCAL::normalize(dir) * velocity * tick;
}
void CollisionTestPage::CollisionUpdate(){
	for(INT i = 0 ; i<10 ; ++i){
		FLOAT3 epa = PCS::tCollision2D(p_col, m_col[i]);
		if(epa != FLOAT3::zero()){
			p_mesh->GetJoint()->position -= epa;
		}
	}
}

void CollisionTestPage::update(){
	Rebatch();
	InputUpdate();
	CollisionUpdate();
}

void CollisionTestPage::UpdateUniformBlockP(void* s, UINT* tex, UINT* tex_idx){	
	CollisionTestPage* self = (CollisionTestPage*)s;
	self->p_uni_block.world = self->p_mesh->GetJoint()->world;	

	UINT t_idx = 0;																										

	for(UINT i = 0 ; i<(*tex_idx) ; ++i){																				
		if(tex[i] == (UINT)(self->texture[0])){									
			t_idx = i;																									
			break;																										
		}																												
	}																													

	if(t_idx == 0){																										
		tex[*tex_idx] = (UINT)(self->texture[0]);									
		self->p_uni_block.texture_idx = (*tex_idx)++;										
	}																													
	else{																												
		self->p_uni_block.texture_idx = t_idx;												
	}																													
}

M_UNIFORM_FUNTION(0);
M_UNIFORM_FUNTION(1);
M_UNIFORM_FUNTION(2);
M_UNIFORM_FUNTION(3);
M_UNIFORM_FUNTION(4);
M_UNIFORM_FUNTION(5);
M_UNIFORM_FUNTION(6);
M_UNIFORM_FUNTION(7);
M_UNIFORM_FUNTION(8);
M_UNIFORM_FUNTION(9);

#undef M_UNIFORM_FUNTION