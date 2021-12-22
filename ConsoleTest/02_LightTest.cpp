#include "header.h"

LightTestPage::LightTestPage(){
	InitModuleList();

	SetID(0x01, 0x0, 0x0);

	light_ori = {0.0f, 0.0f, 100.0f};
	dir_light = {0.0f, 0.0f, 1.0f};

	diffuse = PCS::GetResource("resources/image/StoneWallDiffuse.png");
	normal = PCS::GetResource("resources/image/StoneWallNormal.png");
	height = nullptr;

	mesh = new PolygonMesh();
	mesh->parent = this;
	mesh->CreateJoint();
	mesh->GetJoint()->parent = this->GetJoint();
	mesh->GetJoint()->scale = {512.0f, 512.0f, 1.0f};
	mesh->CreateVertex(4);
	*(mesh->GetVertex(0)) = {{-1.0f,  1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 0.0f}};
	*(mesh->GetVertex(1)) = {{ 1.0f,  1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 0.0f}};
	*(mesh->GetVertex(2)) = {{-1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {0.0f, 1.0f}};
	*(mesh->GetVertex(3)) = {{ 1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {1.0f, 1.0f}};

	mesh->CreateIndex(6);
	*(mesh->GetIndex(0)) = 0;
	*(mesh->GetIndex(1)) = 1;
	*(mesh->GetIndex(2)) = 2;
	*(mesh->GetIndex(3)) = 1;
	*(mesh->GetIndex(4)) = 2;
	*(mesh->GetIndex(5)) = 3;

	mesh->SetShader((SHADER*)PCS::GetResource("light.shd"));
	uni_block = {mesh->GetJoint()->world, dir_light, 0, 0, 0};
	mesh->SetUniformBlock(&uni_block, sizeof(UNI_BLOCK), &UpdateUniformBlock);
	RegistModule((Module*)mesh);

	info_text = new TextMesh();
	info_text->parent = this;
	info_text->CreateJoint();
	info_text->GetJoint()->parent = this->GetJoint();
	info_text->GetJoint()->position = {-1200.0f, 0.0f, 0.0f};
	info_text->SetFont(PCS::GetResource("resources/font/NanumBarunGothic.ttf"));
	info_text->SetSortType(4);
	info_text->SetSize(64.0f);
	info_text->SetBox({0.0f, 0.0f});
	info_text->SetSolidColor({0.0f, 0.0f, 0.0f, 1.0f});
	info_text->SetOutlineColor({1.0f, 1.0f, 1.0f, 1.0f});
	info_text->SetOutlineSize(3.0f);
	info_text->SetText(L"Ä¿¼­ : ºû À§Ä¡(Directional Light)\nQ : Diffuse Map On/Off\nW : Normal Map On/Off");
	info_text->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)info_text);
}
LightTestPage::~LightTestPage(){
	RemoveModule((Module*)mesh);
	delete mesh;

	RemoveModule((Module*)info_text);
	delete info_text;

	ReleaseModuleList();
}

void LightTestPage::update(){
	FLOAT w = PCS::GetScreenX()*0.5f;
	FLOAT h = PCS::GetScreenY()*0.5f;

	light_ori.x = -w + PCS::GetMouse()->x;
	light_ori.y =  h - PCS::GetMouse()->y;

	if(PCS::GetMouse()->z > 0){
		light_ori.z += 5.0f;
		std::cout<<"light_ori : "<<light_ori.x<<" / "<<light_ori.y<<" / "<<light_ori.z<<std::endl;
		std::cout<<"light : "<<dir_light.x<<" / "<<dir_light.y<<" / "<<dir_light.z<<std::endl;
	}
	else if(PCS::GetMouse()->z < 0){
		light_ori.z -= 5.0f;
		std::cout<<"light_ori : "<<light_ori.x<<" / "<<light_ori.y<<" / "<<light_ori.z<<std::endl;
		std::cout<<"light : "<<dir_light.x<<" / "<<dir_light.y<<" / "<<dir_light.z<<std::endl;
	}

	dir_light = light_ori/VECCAL::length(light_ori);

	if(PCS::GetKeyboard()->KEY['Q'] == 0x81){
		diffuse = (diffuse == nullptr)?PCS::GetResource("resources/image/StoneWallDiffuse.png"):nullptr;
	}
	if(PCS::GetKeyboard()->KEY['W'] == 0x81){
		normal = (normal == nullptr)?PCS::GetResource("resources/image/StoneWallNormal.png"):nullptr;
	}
	/*if(PCS::GetKeyboard()->KEY['E'] == 0x81){
		height = (height == nullptr)?PCS::GetResource("resources/image/StoneWallHeight.png"):nullptr;
	}*/
}

void LightTestPage::UpdateUniformBlock(void* self, UINT* tex, UINT* tex_idx){
	((LightTestPage*)self)->uni_block.world = ((LightTestPage*)self)->mesh->GetJoint()->world;

	((LightTestPage*)self)->uni_block.light = ((LightTestPage*)self)->dir_light;

	UINT t_idx = 0;

	//diffuse
	for(UINT i = 0 ; i<(*tex_idx) ; ++i){
		if(tex[i] == (UINT)(((LightTestPage*)self)->diffuse)){
			t_idx = i;
			break;
		}
	}
	if(t_idx == 0){
		tex[*tex_idx] = (UINT)(((LightTestPage*)self)->diffuse);
		((LightTestPage*)self)->uni_block.diffuse_idx = (*tex_idx)++;
	}
	else{
		((LightTestPage*)self)->uni_block.diffuse_idx = t_idx;
	}

	//normal
	for(UINT i = 0 ; i<(*tex_idx) ; ++i){
		if(tex[i] == (UINT)(((LightTestPage*)self)->normal)){
			t_idx = i;
			break;
		}
	}
	if(t_idx == 0){
		tex[*tex_idx] = (UINT)(((LightTestPage*)self)->normal);
		((LightTestPage*)self)->uni_block.normal_idx = (*tex_idx)++;
	}
	else{
		((LightTestPage*)self)->uni_block.normal_idx = t_idx;
	}

	//height
	/*for(UINT i = 0 ; i<(*tex_idx) ; ++i){
		if(tex[i] == (UINT)(((LightTestPage*)self)->height)){
			t_idx = i;
			break;
		}
	}
	if(t_idx == 0){
		tex[*tex_idx] = (UINT)(((LightTestPage*)self)->height);
		((LightTestPage*)self)->uni_block.height_idx = (*tex_idx)++;
	}
	else{
		((LightTestPage*)self)->uni_block.height_idx = t_idx;
	}*/
}