#include "header.h"

BGShape::BGShape(){
	InitModuleList();

	SetID(0x00, 0x0, 0x0);

	texture = nullptr;

	FLOAT w = PCS::GetScreenX();
	FLOAT h = PCS::GetScreenY();

	mesh = new PolygonMesh();
	mesh->parent = this;
	mesh->CreateJoint();
	mesh->GetJoint()->parent = this->GetJoint();
	mesh->GetJoint()->position = {0.0f, 0.0f, -100.0f};
	mesh->GetJoint()->scale = {w, h, 1.0f};
	mesh->CreateVertex(4);
	*(mesh->GetVertex(0)) = {{-0.5f,  0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 0.0f}};
	*(mesh->GetVertex(1)) = {{ 0.5f,  0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 0.0f}};
	*(mesh->GetVertex(2)) = {{-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 1.0f}};
	*(mesh->GetVertex(3)) = {{ 0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 1.0f}};

	mesh->CreateIndex(6);
	*(mesh->GetIndex( 0)) = 0;
	*(mesh->GetIndex( 1)) = 1;
	*(mesh->GetIndex( 2)) = 2;
	*(mesh->GetIndex( 3)) = 1;
	*(mesh->GetIndex( 4)) = 2;
	*(mesh->GetIndex( 5)) = 3;

	mesh->SetShader((SHADER*)PCS::GetResource("default.shd"));
	uni_block = {mesh->GetJoint()->world, 0};
	mesh->SetUniformBlock(&uni_block, sizeof(UNI_BLOCK), &UpdateUniformBlock);
	RegistModule((Module*)mesh);
}
BGShape::~BGShape(){
	RemoveModule((Module*)mesh);
	delete mesh;
	mesh = nullptr;

	ReleaseModuleList();
}

void BGShape::update(){
	if(PCS::GetKeyboard()->KEY['1'] == 0x81){
		mesh->GetVertex(0)->diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
		mesh->GetVertex(1)->diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
		mesh->GetVertex(2)->diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
		mesh->GetVertex(3)->diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
	}
	else if(PCS::GetKeyboard()->KEY['2'] == 0x81){
		mesh->GetVertex(0)->diffuse = {0.0f, 0.0f, 0.0f, 1.0f};
		mesh->GetVertex(1)->diffuse = {0.0f, 0.0f, 0.0f, 1.0f};
		mesh->GetVertex(2)->diffuse = {0.0f, 0.0f, 0.0f, 1.0f};
		mesh->GetVertex(3)->diffuse = {0.0f, 0.0f, 0.0f, 1.0f};
	}
	else if(PCS::GetKeyboard()->KEY['3'] == 0x81){
		mesh->GetVertex(0)->diffuse = {0.125f, 0.125f, 0.125f, 1.0f};
		mesh->GetVertex(1)->diffuse = {0.125f, 0.125f, 0.125f, 1.0f};
		mesh->GetVertex(2)->diffuse = {0.125f, 0.125f, 0.125f, 1.0f};
		mesh->GetVertex(3)->diffuse = {0.125f, 0.125f, 0.125f, 1.0f};
	}
}

void BGShape::UpdateUniformBlock(void* self, UINT* tex, UINT* tex_idx){
	((BGShape*)self)->uni_block.world = ((BGShape*)self)->mesh->GetJoint()->world;

	UINT t_idx = 0;

	for(UINT i = 0 ; i<(*tex_idx) ; ++i){
		if(tex[i] == (UINT)(((BGShape*)self)->texture)){
			t_idx = i;
			break;
		}
	}

	if(t_idx == 0){
		tex[*tex_idx] = (UINT)(((BGShape*)self)->texture);
		((BGShape*)self)->uni_block.texture_idx = (*tex_idx)++;
	}
	else{
		((BGShape*)self)->uni_block.texture_idx = t_idx;
	}
}

BGCamera::BGCamera(){
	InitModuleList();

	SetID(0x00, 0xFFFF, 0x0);

	cam = new Camera;
	cam->parent = this;
	cam->SetScreenRange(0, 0, PCS::GetScreenX(), PCS::GetScreenY());
	cam->SetViewMatrix({0.0f, 0.0f, 2000.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	cam->SetOrthoProjMatrix(RADIAN(45), {-10000.0f, 10000.0f});
	cam->SetScreen();

	cam->SetShader((SHADER*)PCS::GetResource("defaultCam.shd"));

	RegistModule((Module*)cam);

	BackGround = new BGShape();
}
BGCamera::~BGCamera(){
	//if(BackGround != nullptr){
	//	delete BackGround;
	//	BackGround = nullptr;
	//}

	RemoveModule((Module*)cam);
	delete cam;
	cam = nullptr;

	ReleaseModuleList();
}

UIShape::UIShape(){
	InitModuleList();

	SetID(0x02, 0x0, 0x0);

	texture = PCS::GetResource("resources/image/frame.png");

	FLOAT4 color = {0.0f, 0.0f, 0.0f, 1.0f};

	w = PCS::GetScreenX()*0.5f;
	h = PCS::GetScreenY()*0.5f;

	mesh = new PolygonMesh();
	mesh->parent = this;
	mesh->CreateJoint();
	mesh->GetJoint()->parent = this->GetJoint();
	mesh->GetJoint()->position = {0.0f, 0.0f, 100.0f};
	mesh->GetJoint()->scale = {1.0, 1.0, 1.0f};
	mesh->CreateVertex(32);
	*(mesh->GetVertex( 0)) = {{-w       ,  h       , 0.0f, 1.0f}, color, {}, {0.0000f, 1.0000f}};
	*(mesh->GetVertex( 1)) = {{-w+160.0f,  h       , 0.0f, 1.0f}, color, {}, {0.3125f, 1.0000f}};
	*(mesh->GetVertex( 2)) = {{-w       ,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.0000f, 0.6875f}};
	*(mesh->GetVertex( 3)) = {{-w+160.0f,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.3125f, 0.6875f}};

	*(mesh->GetVertex( 4)) = {{-w+160.0f,  h       , 0.0f, 1.0f}, color, {}, {0.3125f, 1.0000f}};
	*(mesh->GetVertex( 5)) = {{ w-160.0f,  h       , 0.0f, 1.0f}, color, {}, {0.6875f, 1.0000f}};
	*(mesh->GetVertex( 6)) = {{-w+160.0f,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.3125f, 0.6875f}};
	*(mesh->GetVertex( 7)) = {{ w-160.0f,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.6875f, 0.6875f}};

	*(mesh->GetVertex( 8)) = {{ w-160.0f,  h       , 0.0f, 1.0f}, color, {}, {0.6875f, 1.0000f}};
	*(mesh->GetVertex( 9)) = {{ w       ,  h       , 0.0f, 1.0f}, color, {}, {1.0000f, 1.0000f}};
	*(mesh->GetVertex(10)) = {{ w-160.0f,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.6875f, 0.6875f}};
	*(mesh->GetVertex(11)) = {{ w       ,  h-160.0f, 0.0f, 1.0f}, color, {}, {1.0000f, 0.6875f}};

	*(mesh->GetVertex(12)) = {{-w       ,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.0000f, 0.6875f}};
	*(mesh->GetVertex(13)) = {{-w+160.0f,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.3125f, 0.6875f}};
	*(mesh->GetVertex(14)) = {{-w       , -h+160.0f, 0.0f, 1.0f}, color, {}, {0.0000f, 0.3125f}};
	*(mesh->GetVertex(15)) = {{-w+160.0f, -h+160.0f, 0.0f, 1.0f}, color, {}, {0.3125f, 0.3125f}};

	*(mesh->GetVertex(16)) = {{ w-160.0f,  h-160.0f, 0.0f, 1.0f}, color, {}, {0.6875f, 0.6875f}};
	*(mesh->GetVertex(17)) = {{ w       ,  h-160.0f, 0.0f, 1.0f}, color, {}, {1.0000f, 0.6875f}};
	*(mesh->GetVertex(18)) = {{ w-160.0f, -h+160.0f, 0.0f, 1.0f}, color, {}, {0.6875f, 0.3125f}};
	*(mesh->GetVertex(19)) = {{ w       , -h+160.0f, 0.0f, 1.0f}, color, {}, {1.0000f, 0.3125f}};

	*(mesh->GetVertex(20)) = {{-w       , -h+160.0f, 0.0f, 1.0f}, color, {}, {0.0000f, 0.3125f}};
	*(mesh->GetVertex(21)) = {{-w+160.0f, -h+160.0f, 0.0f, 1.0f}, color, {}, {0.3125f, 0.3125f}};
	*(mesh->GetVertex(22)) = {{-w       , -h       , 0.0f, 1.0f}, color, {}, {0.0000f, 0.0000f}};
	*(mesh->GetVertex(23)) = {{-w+160.0f, -h       , 0.0f, 1.0f}, color, {}, {0.3125f, 0.0000f}};

	*(mesh->GetVertex(24)) = {{-w+160.0f, -h+160.0f, 0.0f, 1.0f}, color, {}, {0.3125f, 0.3125f}};
	*(mesh->GetVertex(25)) = {{ w-160.0f, -h+160.0f, 0.0f, 1.0f}, color, {}, {0.6875f, 0.3125f}};
	*(mesh->GetVertex(26)) = {{-w+160.0f, -h       , 0.0f, 1.0f}, color, {}, {0.3125f, 0.0000f}};
	*(mesh->GetVertex(27)) = {{ w-160.0f, -h       , 0.0f, 1.0f}, color, {}, {0.6875f, 0.0000f}};

	*(mesh->GetVertex(28)) = {{ w-160.0f, -h+160.0f, 0.0f, 1.0f}, color, {}, {0.6875f, 0.3125f}};
	*(mesh->GetVertex(29)) = {{ w       , -h+160.0f, 0.0f, 1.0f}, color, {}, {1.0000f, 0.3125f}};
	*(mesh->GetVertex(30)) = {{ w-160.0f, -h       , 0.0f, 1.0f}, color, {}, {0.6875f, 0.0000f}};
	*(mesh->GetVertex(31)) = {{ w       , -h       , 0.0f, 1.0f}, color, {}, {1.0000f, 0.0000f}};

	mesh->CreateIndex(48);
	for(INT i = 0 ; i<8 ; ++i){
		*(mesh->GetIndex((i*6)+0)) = (i*4)+0;
		*(mesh->GetIndex((i*6)+1)) = (i*4)+1;
		*(mesh->GetIndex((i*6)+2)) = (i*4)+2;
		*(mesh->GetIndex((i*6)+3)) = (i*4)+1;
		*(mesh->GetIndex((i*6)+4)) = (i*4)+2;
		*(mesh->GetIndex((i*6)+5)) = (i*4)+3;
	}

	mesh->SetShader((SHADER*)PCS::GetResource("default.shd"));
	uni_block = {mesh->GetJoint()->world, 0};
	mesh->SetUniformBlock(&uni_block, sizeof(UNI_BLOCK), &UpdateUniformBlock);
	RegistModule((Module*)mesh);

	textStart = {w-64.0f, h-64.0f, -100.0};
	mainOL = 1.0f;
	subOL = 0.875f;

	text[0] = new TextMesh();
	text[0]->parent = this;
	text[0]->CreateJoint();
	text[0]->GetJoint()->parent = this->GetJoint();
	text[0]->GetJoint()->position = textStart;
	text[0]->SetFont(PCS::GetResource("resources/font/NanumBarunGothicUltraLight.ttf"));
	text[0]->SetSortType(9);
	text[0]->SetSize(64.0f);
	text[0]->SetBox({0.0f, 0.0f});
	text[0]->SetSolidColor({1.0f, 1.0f, 1.0f, 1.0f});
	text[0]->SetOutlineColor({0.0f, 0.0f, 0.0f, 1.0f});
	text[0]->SetOutlineSize(mainOL);
	text[0]->SetText(L"현재 페이지 - ");
	text[0]->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)text[0]);

	text[1] = new TextMesh();
	text[1]->parent = this;
	text[1]->CreateJoint();
	text[1]->GetJoint()->parent = this->GetJoint();
	text[1]->GetJoint()->position = textStart;
	text[1]->SetFont(PCS::GetResource("resources/font/NanumBarunGothicUltraLight.ttf"));
	text[1]->SetSortType(9);
	text[1]->SetSize(64.0f);
	text[1]->SetBox({0.0f, 0.0f});
	text[1]->SetSolidColor({1.0f, 1.0f, 1.0f, 1.0f});
	text[1]->SetOutlineColor({0.0f, 0.0f, 0.0f, 1.0f});
	text[1]->SetOutlineSize(mainOL);
	text[1]->SetText(L"1. 안내 페이지(폰트 테스트)");
	text[1]->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)text[1]);

	text[2] = new TextMesh();
	text[2]->parent = this;
	text[2]->CreateJoint();
	text[2]->GetJoint()->parent = this->GetJoint();
	text[2]->GetJoint()->position = textStart;
	text[2]->SetFont(PCS::GetResource("resources/font/NanumBarunGothicUltraLight.ttf"));
	text[2]->SetSortType(9);
	text[2]->SetSize(48.0f);
	text[2]->SetBox({0.0f, 0.0f});
	text[2]->SetSolidColor({1.0f, 1.0f, 1.0f, 1.0f});
	text[2]->SetOutlineColor({0.0f, 0.0f, 0.0f, 1.0f});
	text[2]->SetOutlineSize(subOL);
	text[2]->SetText(L"2. 2D Light 테스트");
	text[2]->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)text[2]);

	text[3] = new TextMesh();
	text[3]->parent = this;
	text[3]->CreateJoint();
	text[3]->GetJoint()->parent = this->GetJoint();
	text[3]->GetJoint()->position = textStart;
	text[3]->SetFont(PCS::GetResource("resources/font/NanumBarunGothicUltraLight.ttf"));
	text[3]->SetSortType(9);
	text[3]->SetSize(48.0f);
	text[3]->SetBox({0.0f, 0.0f});
	text[3]->SetSolidColor({1.0f, 1.0f, 1.0f, 1.0f});
	text[3]->SetOutlineColor({0.0f, 0.0f, 0.0f, 1.0f});
	text[3]->SetOutlineSize(subOL);
	text[3]->SetText(L"3. 충돌 테스트");
	text[3]->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)text[3]);
	
	text[0]->GetJoint()->position.x = text[1]->GetJoint()->position.x-(text[1]->GetMaxWidth()*text[1]->GetJoint()->scale.x);
	text[2]->GetJoint()->position.y = text[1]->GetJoint()->position.y-(text[1]->GetMaxHeight()*text[1]->GetJoint()->scale.y);
	text[3]->GetJoint()->position.y = text[2]->GetJoint()->position.y-(text[2]->GetMaxHeight()*text[2]->GetJoint()->scale.y);
}
UIShape::~UIShape(){
	RemoveModule((Module*)mesh);
	delete mesh;
	mesh = nullptr;

	for(INT i = 0 ; i<4 ; ++i){
		RemoveModule((Module*)text[i]);
		delete text[i];
		text[i] = nullptr;
	}

	ReleaseModuleList();
}

void UIShape::update(){
	if(PCS::GetKeyboard()->KEY['1'] == 0x81){
		text[1]->SetSize(64.0f);
		text[2]->SetSize(48.0f);
		text[3]->SetSize(48.0f);

		text[0]->GetJoint()->position = textStart;
		text[1]->GetJoint()->position = textStart;
		text[2]->GetJoint()->position = textStart;
		text[3]->GetJoint()->position = textStart;

		text[0]->GetJoint()->position.x = text[1]->GetJoint()->position.x-(text[1]->GetMaxWidth()*text[1]->GetJoint()->scale.x);
		text[2]->GetJoint()->position.y = text[1]->GetJoint()->position.y-(text[1]->GetMaxHeight()*text[1]->GetJoint()->scale.y);
		text[3]->GetJoint()->position.y = text[2]->GetJoint()->position.y-(text[2]->GetMaxHeight()*text[2]->GetJoint()->scale.y);

		text[0]->SetOutlineSize(mainOL);
		text[1]->SetOutlineSize(mainOL);
		text[2]->SetOutlineSize(subOL);
		text[3]->SetOutlineSize(subOL);

		FLOAT c = 1.0f, rc = 1.0f-c;
		text[0]->SetSolidColor({c, c, c, 1.0f});
		text[0]->SetOutlineColor({rc, rc, rc, 1.0f});
		text[1]->SetSolidColor({c, c, c, 1.0f});
		text[1]->SetOutlineColor({rc, rc, rc, 1.0f});
		text[2]->SetSolidColor({c, c, c, 1.0f});
		text[2]->SetOutlineColor({rc, rc, rc, 1.0f});
		text[3]->SetSolidColor({c, c, c, 1.0f});
		text[3]->SetOutlineColor({rc, rc, rc, 1.0f});

		for(INT i = 0 ; i<mesh->GetMaxVertex() ; ++i){mesh->GetVertex(i)->diffuse = {0.0f, 0.0f, 0.0f, 1.0f};}
	}
	else if(PCS::GetKeyboard()->KEY['2'] == 0x81){
		text[1]->SetSize(48.0f);
		text[2]->SetSize(64.0f);
		text[3]->SetSize(48.0f);

		text[0]->GetJoint()->position = textStart;
		text[1]->GetJoint()->position = textStart;
		text[2]->GetJoint()->position = textStart;
		text[3]->GetJoint()->position = textStart;

		text[0]->GetJoint()->position.x = text[1]->GetJoint()->position.x-(text[2]->GetMaxWidth()*text[2]->GetJoint()->scale.x);
		text[1]->GetJoint()->position.y = text[2]->GetJoint()->position.y-(text[2]->GetMaxHeight()*text[2]->GetJoint()->scale.y);
		text[3]->GetJoint()->position.y = text[1]->GetJoint()->position.y-(text[1]->GetMaxHeight()*text[1]->GetJoint()->scale.y);

		text[0]->SetOutlineSize(mainOL);
		text[1]->SetOutlineSize(subOL);
		text[2]->SetOutlineSize(mainOL);
		text[3]->SetOutlineSize(subOL);

		FLOAT c = 0.0f, rc = 1.0f-c;
		text[0]->SetSolidColor({c, c, c, 1.0f});
		text[0]->SetOutlineColor({rc, rc, rc, 1.0f});
		text[1]->SetSolidColor({c, c, c, 1.0f});
		text[1]->SetOutlineColor({rc, rc, rc, 1.0f});
		text[2]->SetSolidColor({c, c, c, 1.0f});
		text[2]->SetOutlineColor({rc, rc, rc, 1.0f});
		text[3]->SetSolidColor({c, c, c, 1.0f});
		text[3]->SetOutlineColor({rc, rc, rc, 1.0f});

		for(INT i = 0 ; i<mesh->GetMaxVertex() ; ++i){mesh->GetVertex(i)->diffuse = {1.0f, 1.0f, 1.0f, 1.0f};}
	}
	else if(PCS::GetKeyboard()->KEY['3'] == 0x81){
		text[1]->SetSize(48.0f);
		text[2]->SetSize(48.0f);
		text[3]->SetSize(64.0f);

		text[0]->GetJoint()->position = textStart;
		text[1]->GetJoint()->position = textStart;
		text[2]->GetJoint()->position = textStart;
		text[3]->GetJoint()->position = textStart;

		text[0]->GetJoint()->position.x = text[1]->GetJoint()->position.x-(text[3]->GetMaxWidth()*text[3]->GetJoint()->scale.x);
		text[1]->GetJoint()->position.y = text[3]->GetJoint()->position.y-(text[3]->GetMaxHeight()*text[3]->GetJoint()->scale.y);
		text[2]->GetJoint()->position.y = text[1]->GetJoint()->position.y-(text[1]->GetMaxHeight()*text[1]->GetJoint()->scale.y);

		text[0]->SetOutlineSize(mainOL);
		text[1]->SetOutlineSize(subOL);
		text[2]->SetOutlineSize(subOL);
		text[3]->SetOutlineSize(mainOL);

		FLOAT c1 = 0.125f, rc1 = 1.0f-c1, c2 = 0.875f, rc2 = 1.0f-c2;
		text[0]->SetSolidColor({c1, c2, c2, 1.0f});
		text[0]->SetOutlineColor({rc1, rc2, rc2, 1.0f});
		text[1]->SetSolidColor({c1, c2, c2, 1.0f});
		text[1]->SetOutlineColor({rc1, rc2, rc2, 1.0f});
		text[2]->SetSolidColor({c1, c2, c2, 1.0f});
		text[2]->SetOutlineColor({rc1, rc2, rc2, 1.0f});
		text[3]->SetSolidColor({c1, c2, c2, 1.0f});
		text[3]->SetOutlineColor({rc1, rc2, rc2, 1.0f});

		for(INT i = 0 ; i<mesh->GetMaxVertex() ; ++i){mesh->GetVertex(i)->diffuse = {0.75f, 0.25f, 0.25f, 1.0f};}
	}
}

void UIShape::UpdateUniformBlock(void* self, UINT* tex, UINT* tex_idx){
	((UIShape*)self)->uni_block.world = ((UIShape*)self)->mesh->GetJoint()->world;

	UINT t_idx = 0;

	for(UINT i = 0 ; i<(*tex_idx) ; ++i){
		if(tex[i] == (UINT)(((UIShape*)self)->texture)){
			t_idx = i;
			break;
		}
	}

	if(t_idx == 0){
		tex[*tex_idx] = (UINT)(((UIShape*)self)->texture);
		((UIShape*)self)->uni_block.texture_idx = (*tex_idx)++;
	}
	else{
		((UIShape*)self)->uni_block.texture_idx = t_idx;
	}
}

UICamera::UICamera(){
	InitModuleList();

	SetID(0x02, 0xFFFF, 0x0);

	cam = new Camera;
	cam->parent = this;
	cam->SetScreenRange(0, 0, PCS::GetScreenX(), PCS::GetScreenY());
	cam->SetViewMatrix({0.0f, 0.0f, 2000.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	cam->SetOrthoProjMatrix(RADIAN(45), {-10000.0f, 10000.0f});
	cam->SetScreen();

	cam->SetShader((SHADER*)PCS::GetResource("defaultCam.shd"));

	RegistModule((Module*)cam);

	ui = new UIShape();

	ftp = new FontTestPage();		ftp->active = true;
	ltp = new LightTestPage();		ltp->active = false;
	ctp = new CollisionTestPage();	ctp->active = false;
}
UICamera::~UICamera(){
	//if(ui != nullptr){
	//	delete ui;
	//	ui = nullptr;
	//}

	RemoveModule((Module*)cam);
	delete cam;
	cam = nullptr;

	ReleaseModuleList();
}

void UICamera::update(){
	if(PCS::GetKeyboard()->KEY['1'] == 0x81){
		ftp->active = true;
		ltp->active = false;
		ctp->active = false;
	}
	else if(PCS::GetKeyboard()->KEY['2'] == 0x81){
		ftp->active = false;
		ltp->active = true;
		ctp->active = false;
	}
	else if(PCS::GetKeyboard()->KEY['3'] == 0x81){
		ftp->active = false;
		ltp->active = false;
		ctp->active = true;
	}
}

MainCamera::MainCamera(){
	std::cout<<"create camera object"<<std::endl;

	InitModuleList();

	SetID(0x01, 0xFFFF, 0x0);

	cam = new Camera;
	cam->parent = this;
	cam->SetScreenRange(0, 0, PCS::GetScreenX(), PCS::GetScreenY());
	//pos = {0.0f, 0.0f};
	//rot = 0.0f;
	cam->SetViewMatrix({0.0f, 0.0f, 2000.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	//cam->SetViewMatrix({cos(RADIAN(rot))*2000.0f, 0.0f, -sin(RADIAN(rot))*2000.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	cam->SetPerspectiveProjMatrix(RADIAN(45), {1.0f, 10000.0f});
	//cam->SetOrthoProjMatrix(RADIAN(45), {-10000.0f, 10000.0f});
	cam->SetScreen();

	cam->SetShader((SHADER*)PCS::GetResource("defaultCam.shd"));

	RegistModule((Module*)cam);

	z = 2000.0f;
}
MainCamera::~MainCamera(){
	RemoveModule((Module*)cam);
	delete cam;

	ReleaseModuleList();
}

void MainCamera::update(){
	/*FLOAT x = PCS::GetScreenX();	x = -x*0.5f;
	FLOAT y = PCS::GetScreenY();	y = -y*0.5f;
	x += PCS::GetMouse()->x;
	y += PCS::GetMouse()->y;

	if(PCS::GetMouse()->z > 0){z += 100.0f; std::cout<<z<<std::endl;}
	else if(PCS::GetMouse()->z < 0){z -= 100.0f; std::cout<<z<<std::endl;}

	//cam->SetViewMatrix({-x, y, z}, {100, 100.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	cam->SetViewMatrix({-x, y, z}, {-x, y, 0.0f}, {0.0f, 1.0f, 0.0f});*/
}