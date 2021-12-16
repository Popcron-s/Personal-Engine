#pragma once

#include "..\\Common\\Singleton.h"
#include "..\\Common\\variable.h"
#include "..\\Common\\List.h"

class Mesh;

class Graphic_Renderer : public Singleton<Graphic_Renderer>{
private:
	void Batch(Mesh** st, Mesh** ed);

public:
	Graphic_Renderer();
	~Graphic_Renderer();
	
	void Render();
};

class Sound_Renderer : public Singleton<Sound_Renderer>{
public:
	void Render();
};

class Renderer : public Singleton<Renderer>{
private:

public:
	Renderer();
	~Renderer();

	void Animation();
	void Render();
};
