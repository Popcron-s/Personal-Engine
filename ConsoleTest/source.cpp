#include "header.h"

void starter(){
	UINT size = 0;
	UINT extra_size = 0;
	BYTE* buf = nullptr;
	void* target = nullptr;
	void* sub_target = nullptr;
	SHADER* shd = nullptr;

	char uni_name[32][32] = {"imgAbuffer", "imgListHead", 
		"textures[0]", "textures[1]", "textures[2]", "textures[3]",
		"textures[4]", "textures[5]", "textures[6]", "textures[7]",
		"textures[8]", "textures[9]", "textures[10]", "textures[11]",
		"textures[12]", "textures[13]", "textures[14]", "textures[15]"};
	_UNIFORM::TYPE uni_type[32] = {
		_UNIFORM::TYPE::CAMERA_BUF, _UNIFORM::TYPE::CAMERA_HEAD, 
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE, 
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE,
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE, 
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE,
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE, 
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE,
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE, 
		_UNIFORM::TYPE::TEXTURE, _UNIFORM::TYPE::TEXTURE};

	PCS::LoadFile("resources/image/flier.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/flier.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/rock1.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/rock1.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/rock2.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/rock2.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/rock3.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/rock3.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/frame.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/frame.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/StoneWallDiffuse.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/StoneWallDiffuse.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/StoneWallNormal.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/StoneWallNormal.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	PCS::LoadFile("resources/image/StoneWallHeight.png", buf, size);
	target = PCS::RegistTexture(buf, size);
	PCS::AddResource("resources/image/StoneWallHeight.png", target, RESOURCEFORMAT::TYPE::TEXTURE);

	size = 0;	buf = nullptr;	target = nullptr;
	PCS::LoadFile("../resources/shader/cam_vs.txt", buf, size);
	target = PCS::CreateShader(buf, size, _SHADER::TYPE::VERTEX);

	size = 0;	buf = nullptr;	sub_target = nullptr;
	PCS::LoadFile("../resources/shader/cam_fs.txt", buf, size);
	sub_target = PCS::CreateShader(buf, size, _SHADER::TYPE::FRAGMENT);

	shd = nullptr;
	PCS::SetShader(shd, target, nullptr, sub_target, uni_name, uni_type, 2);
	PCS::AddResource("defaultCam.shd", (void*)shd, RESOURCEFORMAT::TYPE::SHADER);

	size = 0;	buf = nullptr;	target = nullptr;
	PCS::LoadFile("../resources/shader/default_vs.txt", buf, size);
	target = PCS::CreateShader(buf, size, _SHADER::TYPE::VERTEX);

	size = 0;	buf = nullptr;	sub_target = nullptr;
	PCS::LoadFile("../resources/shader/default_fs.txt", buf, size);
	sub_target = PCS::CreateShader(buf, size, _SHADER::TYPE::FRAGMENT);

	shd = nullptr;
	PCS::SetShader(shd, target, nullptr, sub_target, uni_name, uni_type, 18);
	PCS::AddResource("default.shd", (void*)shd, RESOURCEFORMAT::TYPE::SHADER);

	size = 0;	buf = nullptr;	target = nullptr;
	PCS::LoadFile("../resources/shader/light_vs.txt", buf, size);
	target = PCS::CreateShader(buf, size, _SHADER::TYPE::VERTEX);

	size = 0;	buf = nullptr;	sub_target = nullptr;
	PCS::LoadFile("../resources/shader/light_fs.txt", buf, size);
	sub_target = PCS::CreateShader(buf, size, _SHADER::TYPE::FRAGMENT);

	shd = nullptr;
	PCS::SetShader(shd, target, nullptr, sub_target, uni_name, uni_type, 18);
	PCS::AddResource("light.shd", (void*)shd, RESOURCEFORMAT::TYPE::SHADER);

	size = 0;	buf = nullptr;	target = nullptr;
	PCS::LoadFile("../resources/shader/font_vs.txt", buf, size);
	target = PCS::CreateShader(buf, size, _SHADER::TYPE::VERTEX);

	size = 0;	buf = nullptr;	sub_target = nullptr;
	PCS::LoadFile("../resources/shader/font_fs.txt", buf, size);
	sub_target = PCS::CreateShader(buf, size, _SHADER::TYPE::FRAGMENT);

	shd = nullptr;
	PCS::SetShader(shd, target, nullptr, sub_target, uni_name, uni_type, 2);
	PCS::AddResource("font.shd", (void*)shd, RESOURCEFORMAT::TYPE::SHADER);

	size = 0; buf = nullptr; target = nullptr;
	PCS::LoadFile("resources/font/NanumBarunGothicUltraLight.ttf", buf, size);
	target = PCS::CreateFont(buf, size);
	PCS::AddResource("resources/font/NanumBarunGothicUltraLight.ttf", (void*)target, RESOURCEFORMAT::TYPE::FONT);

	size = 0; buf = nullptr; target = nullptr;
	PCS::LoadFile("resources/font/NanumBarunGothic.ttf", buf, size);
	target = PCS::CreateFont(buf, size);
	PCS::AddResource("resources/font/NanumBarunGothic.ttf", (void*)target, RESOURCEFORMAT::TYPE::FONT);

	size = 0 ; buf = nullptr; target = nullptr;
	PCS::LoadFile("resources/test.txt", buf, size);
	PCS::LoadText(buf, size, (wchar_t*&)target, extra_size);
	PCS::AddResource("resources/test.txt", target, RESOURCEFORMAT::TYPE::TEXT);

	BGCamera* bgc = new BGCamera();
	MainCamera* mc = new MainCamera();
	UICamera* uic = new UICamera();
	//DefaultShape* ds = new DefaultShape();
}