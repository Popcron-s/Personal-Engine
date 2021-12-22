#include "header.h"

FontTestPage::FontTestPage(){
	InitModuleList();

	SetID(0x01, 0x0, 0x0);

	FLOAT3 pos = {-1000.0f, 0.0f, 0.0f};

	text = new TextMesh();
	text->parent = this;
	text->CreateJoint();
	text->GetJoint()->parent = this->GetJoint();
	text->GetJoint()->position = pos;
	text->SetFont(PCS::GetResource("resources/font/NanumBarunGothic.ttf"));
	text->SetSortType(4);
	text->SetSize(64.0f);
	text->SetBox({0.0f, 0.0f});
	text->SetSolidColor({1.0f, 1.0f, 1.0f, 1.0f});
	text->SetOutlineColor({0.0f, 0.0f, 0.0f, 1.0f});
	text->SetOutlineSize(30.0f);
	wchar_t* textFile = (wchar_t*)PCS::GetResource("resources/test.txt");
	text->SetText(textFile);
	text->SetShader((SHADER*)PCS::GetResource("font.shd"));
	RegistModule((Module*)text);
}
FontTestPage::~FontTestPage(){
	RemoveModule((Module*)text);
	delete text;
	text = nullptr;
}

void FontTestPage::update(){}