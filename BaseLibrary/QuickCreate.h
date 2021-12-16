#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Singleton.h"
#include "..\\Common\\List.h"
#include "Module.h"

class QuickCreate{
private:

public:
	static Mesh* CreateBoxMesh(List<Module*>*& Module_List, VECTOR2 size, VECTOR2 center, VECTOR4 color){
		if(Module_List == nullptr){return nullptr;}
		Module* mdl = new Mesh();
		FLOAT t = 2.0f;
		size /= t;
		dynamic_cast<Mesh*>(mdl)->VertexCreate(4);
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(0)) = {{center.x-size.x,center.y-size.y, 0.0f, 1.0f}, color, {}, {0.0f, 0.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(1)) = {{center.x-size.x,center.y+size.y, 0.0f, 1.0f}, color, {}, {0.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(3)) = {{center.x+size.x,center.y+size.y, 0.0f, 1.0f}, color, {}, {1.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(2)) = {{center.x+size.x,center.y-size.y, 0.0f, 1.0f}, color, {}, {1.0f, 0.0f}};
		dynamic_cast<Mesh*>(mdl)->SetPolygon();
		Module_List->CreateNode(mdl);

		return dynamic_cast<Mesh*>(mdl);
	}
	
	static Mesh* CreateBoxLineMesh(List<Module*>*& Module_List, VECTOR2 size, VECTOR2 center, VECTOR4 color){
		if(Module_List == nullptr){return nullptr;}
		Module* mdl = new Mesh();
		FLOAT t = 2.0f;
		size /= t;
		dynamic_cast<Mesh*>(mdl)->VertexCreate(5);
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(0)) = {{center.x-size.x,center.y-size.y, 0.0f, 1.0f}, color, {}, {0.0f, 0.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(1)) = {{center.x-size.x,center.y+size.y, 0.0f, 1.0f}, color, {}, {0.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(2)) = {{center.x+size.x,center.y+size.y, 0.0f, 1.0f}, color, {}, {1.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(3)) = {{center.x+size.x,center.y-size.y, 0.0f, 1.0f}, color, {}, {1.0f, 0.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(4)) = {{center.x-size.x,center.y-size.y, 0.0f, 1.0f}, color, {}, {0.0f, 0.0f}};
		dynamic_cast<Mesh*>(mdl)->SetLine();
		Module_List->CreateNode(mdl);

		return dynamic_cast<Mesh*>(mdl);
	}
	
	
	static Mesh* CreateCube(List<Module*>*& Module_List, VECTOR3 size, VECTOR3 center, VECTOR4 color){
		if(Module_List == nullptr){return nullptr;}
		Module* mdl = new Mesh();
		FLOAT t = 2.0f;
		size /= t;
		dynamic_cast<Mesh*>(mdl)->VertexCreate(8);
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(0)) = {{center.x-size.x,center.y-size.y,center.z-size.z, 1.0f}, color, {}, {0.0f, 0.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(1)) = {{center.x-size.x,center.y+size.y,center.z-size.z, 1.0f}, color, {}, {0.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(3)) = {{center.x+size.x,center.y+size.y,center.z-size.z, 1.0f}, color, {}, {1.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(2)) = {{center.x+size.x,center.y-size.y,center.z-size.z, 1.0f}, color, {}, {1.0f, 0.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(4)) = {{center.x-size.x,center.y-size.y,center.z+size.z, 1.0f}, color, {}, {0.0f, 0.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(5)) = {{center.x-size.x,center.y+size.y,center.z+size.z, 1.0f}, color, {}, {0.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(7)) = {{center.x+size.x,center.y+size.y,center.z+size.z, 1.0f}, color, {}, {1.0f, 1.0f}};
		*(dynamic_cast<Mesh*>(mdl)->GetVertex(6)) = {{center.x+size.x,center.y-size.y,center.z+size.z, 1.0f}, color, {}, {1.0f, 0.0f}};
		
		dynamic_cast<Mesh*>(mdl)->IndexCreate(12);
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 0)) = {0, 2, 1};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 1)) = {1, 2, 3};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 2)) = {2, 6, 3};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 3)) = {3, 6, 7};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 4)) = {6, 4, 7};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 5)) = {7, 4, 5};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 6)) = {4, 0, 5};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 7)) = {5, 0, 1};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 8)) = {4, 6, 0};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex( 9)) = {0, 6, 2};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex(10)) = {1, 3, 5};
		*(dynamic_cast<Mesh*>(mdl)->GetIndex(11)) = {5, 3, 7};

		dynamic_cast<Mesh*>(mdl)->SetPolygon();
		Module_List->CreateNode(mdl);

		return dynamic_cast<Mesh*>(mdl);
	}
};