#pragma once

#include "..\\Common\\variable.h"
#include "..\\Common\\Singleton.h"

class Collider;
class Mesh;

class Collision : public Singleton<Collision>{
private:
	struct Minkowski{
		INT p1, p2;
		FLOAT2 v;
	}*minkowski;
	INT num;
	/*
	Mesh* testMesh_poly;
	Mesh* testMesh_line;
	*/
private:
	void MinkowskiSet(FLOAT3*, UINT, FLOAT3*, UINT);
	void Collider2VECTOR(Collider*, FLOAT3*&, UINT&);

public:
	Collision();
	~Collision();

	static FLOAT ccw(FLOAT2 p1, FLOAT2 p2, FLOAT2 p3){
		return p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
	}

	static bool comp1(Minkowski m1, Minkowski m2){
		return m1.v.x < m2.v.x || m1.v.x == m2.v.x && m1.v.y < m2.v.y;
	}

	static bool comp2(Minkowski t1, Minkowski t2){
		FLOAT t = ccw(Collision::GetSingleton()->origin(), t1.v, t2.v);
		return t>0 || (t==0) && (t1.v.x<t2.v.x || t1.v.x==t2.v.x && t1.v.y<t2.v.y);
	}

	FLOAT2 origin(){return minkowski[0].v;}

	bool gjk2D();
	FLOAT3 epa2D();
	/*
	void SetMesh(Mesh* poly, Mesh* line){testMesh_poly = poly; testMesh_line = line;}
	void RenderMesh(){
		if(testMesh_poly != nullptr){
			testMesh_poly->VertexCreate(num);
			for(INT i = 0 ; i<num ; ++i){
				*(testMesh_poly->GetVertex(i)) = 
				{{minkowski[i].v.x, minkowski[i].v.y, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, {}};
			}
		}
	}
	*/

	FLOAT3 Collision2D(Collider*, Collider*);
	bool Collision2D(Collider*, Collider*, bool epa_check);

	FLOAT3 Collision2D(Collider*, FLOAT3*, UINT);
	bool Collision2D(Collider*, FLOAT3*, UINT, bool epa_check);

	FLOAT3 Collision2D(FLOAT3*, UINT, FLOAT3*, UINT);
	bool Collision2D(FLOAT3*, UINT, FLOAT3*, UINT, bool epa_check);

	void update();
};