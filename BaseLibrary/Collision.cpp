#include "..\\BaseLibrary\\Collision.h"

#include "..\\BaseLibrary\\ObjectManager.h"
#include "..\\BaseLibrary\\Object.h"

#include "..\\BaseLibrary\\Module.h"

#include "..\\Common\\Sort.h"

Collision::Collision() : minkowski(nullptr), num(0){

}

Collision::~Collision(){}

void Collision::MinkowskiSet(FLOAT3* c1, UINT n1, FLOAT3* c2, UINT n2){
	num = n1*n2;
	if(minkowski != nullptr){
		delete[] minkowski;
	}
	minkowski = new Minkowski[num+1];
	for(UINT i = 0 ; i<num ; ++i){
		minkowski[i].p1 = i/n2;
		minkowski[i].p2 = i%n2;
		FLOAT3 tv = c2[i%n2]-c1[i/n2];
		minkowski[i].v = {tv.x, tv.y};
	}
	minkowski[num] = minkowski[0];
}

void Collision::Collider2VECTOR(Collider* c, FLOAT3*& v, UINT& n){
	if(v != nullptr){delete[] v;}
	PolygonCollider*& pc = (PolygonCollider*&)c;
	n = pc->GetMaxPoint();
	v = new FLOAT3[n];
	for(UINT i = 0 ; i<n ; ++i){
		v[i] = VECCAL::MultipleMatrix(*(pc->GetPoint(i)), pc->GetJoint()->world);
	}
}

FLOAT3 Collision::Collision2D(Collider* c1, Collider* c2){
	if(c1->Type() == _MODULE::TYPE::CPolygon && c2->Type() == _MODULE::TYPE::CPolygon){
		FLOAT3 *p1 = nullptr, *p2 = nullptr;
		UINT n1 = 0, n2 = 0;
		Collider2VECTOR(c1, p1, n1);
		Collider2VECTOR(c2, p2, n2);

		MinkowskiSet(p1, n1, p2, n2);

		FLOAT3 r = FLOAT3::zero();
		bool b = gjk2D();
		if(b){r = epa2D();}

		delete[] minkowski;
		delete[] p1;
		delete[] p2;

		minkowski = nullptr;
		num = 0;
		
		return r;
	}
	else{
		return FLOAT3::zero();
	}
}

bool Collision::Collision2D(Collider* c1, Collider* c2, bool epa_check){
	if(c1->Type() == _MODULE::TYPE::CPolygon && c2->Type() == _MODULE::TYPE::CPolygon){
		FLOAT3 *p1 = nullptr, *p2 = nullptr;
		UINT n1 = 0, n2 = 0;
		Collider2VECTOR(c1, p1, n1);
		Collider2VECTOR(c2, p2, n2);

		MinkowskiSet(p1, n1, p2, n2);

		bool b = gjk2D();

		delete[] minkowski;
		delete[] p1;
		delete[] p2;

		minkowski = nullptr;
		num = 0;

		return b;
	}
	else{
		return false;
	}
}

FLOAT3 Collision::Collision2D(Collider* c1, FLOAT3* p2, UINT n2){
	if(c1->Type() == _MODULE::TYPE::CPolygon){
		FLOAT3 *p1 = nullptr;
		UINT n1 = 0;
		Collider2VECTOR(c1, p1, n1);

		MinkowskiSet(p1, n1, p2, n2);

		FLOAT3 r = FLOAT3::zero();
		bool b = gjk2D();
		if(b){r = epa2D();}

		delete[] minkowski;
		delete[] p1;

		minkowski = nullptr;
		num = 0;

		return r;
	}
	else{
		return FLOAT3::zero();
	}
}
bool Collision::Collision2D(Collider* c1, FLOAT3* p2, UINT n2, bool epa_check){
	if(c1->Type() == _MODULE::TYPE::CPolygon){
		FLOAT3 *p1 = nullptr;
		UINT n1 = 0;
		Collider2VECTOR(c1, p1, n1);

		MinkowskiSet(p1, n1, p2, n2);

		bool b = gjk2D();

		delete[] minkowski;
		delete[] p1;

		minkowski = nullptr;
		num = 0;

		return b;
	}
	else{
		return false;
	}
}

FLOAT3 Collision::Collision2D(FLOAT3* p1, UINT n1, FLOAT3* p2, UINT n2){
	MinkowskiSet(p1, n1, p2, n2);

	FLOAT3 r = FLOAT3::zero();
	bool b = gjk2D();
	if(b){r = epa2D();}

	delete[] minkowski;

	minkowski = nullptr;
	num = 0;

	return r;
}
bool Collision::Collision2D(FLOAT3* p1, UINT n1, FLOAT3* p2, UINT n2, bool epa_check){
	MinkowskiSet(p1, n1, p2, n2);

	bool b = gjk2D();

	delete[] minkowski;

	minkowski = nullptr;
	num = 0;

	return b;
}

bool Collision::gjk2D(){
	Sort::GetSingleton()->Quick(minkowski, minkowski+num, comp1);
	Sort::GetSingleton()->Quick(minkowski+1, minkowski+num, comp2);

	//graham's scan
	INT* gs = new INT[num+1];
	for(INT i = 0 ; i<num+1 ; ++i){gs[i] = 0;}
	INT cnt = 0;
	for(INT i = 0 ; i<num ; ++i ){
		while(cnt > 1 && ccw(minkowski[gs[cnt-2]].v, minkowski[gs[cnt-1]].v, minkowski[i].v) <= 0){--cnt;}
		gs[cnt++] = i;
	}
	for(INT i = cnt ; i<num ; ++i){gs[i] = 0;}

	for(INT i = 0 ; i<cnt+1 ; ++i){
		minkowski[i] = minkowski[gs[i]];
	}
	num = cnt+1;

	//RenderMesh();

	//check
	bool collision = true;
	for(INT i = 0 ; i<num-1 ; ++i){
		FLOAT tccw = ccw(FLOAT2::zero(), minkowski[i].v, minkowski[i+1].v);
		if(tccw<0.0f){collision = false; break;}
	}
	delete[] gs;
	
	return collision;
}

FLOAT3 Collision::epa2D(){
	//epa
	VECTOR2 epa_result = VECTOR2::zero();
	INT epa_set = -1;

	for(int i = 0 ; i<num-1 ; ++i){
		VECTOR2 epa_p1 = minkowski[i].v;
		VECTOR2 epa_p2 = minkowski[i+1].v;

		VECTOR2 epa_v1 = epa_p2-epa_p1;
		VECTOR2 epa_v2 = VECTOR2::zero()-epa_p1;
		epa_v1 = VECCAL::normalize(epa_v1);
		FLOAT epa_v2_dot = VECCAL::dot(epa_v1, epa_v2);
		VECTOR2 rp = epa_p1 + (epa_v1*epa_v2_dot);

		VECTOR2 minp = {};
		VECTOR2 maxp = {};
		if(epa_p1.x<epa_p2.x){
			minp = epa_p1;
			maxp = epa_p2;
		}
		else{
			minp = epa_p2;
			maxp = epa_p1;
		}

		if(rp.x < minp.x){rp = minp;}
		else if(rp.x > maxp.x){rp = maxp;}
		else{
			if(epa_p1.y<epa_p2.y){
				minp = epa_p1;
				maxp = epa_p2;
			}
			else{
				minp = epa_p2;
				maxp = epa_p1;
			}

			if(rp.y < minp.y){rp = minp;}
			else if(rp.y > maxp.y){rp = maxp;}
		}

		if(epa_set == -1){epa_result = rp; epa_set = i;}
		else{
			FLOAT dist1 = VECCAL::length(epa_result);
			FLOAT dist2 = VECCAL::length(rp);

			if(dist1>dist2){
				epa_result = rp;
				epa_set = i;
			}
		}
	}
	/*
	if(testMesh_line != nullptr){
		testMesh_line->VertexCreate(num);
		*(testMesh_line->GetVertex(0)) = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}};
		*(testMesh_line->GetVertex(1)) = 
		{{epa_result.x, epa_result.y, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}};
	}
	*/
	return {-epa_result.x, -epa_result.y, 0.0f};
}

void Collision::update(){
	//get collider list
	Collider** col_arr = nullptr;
	ObjectManager::GetSingleton()->GetCollider(col_arr);
	UINT col_size = ObjectManager::GetSingleton()->GetColliderNum();

	for(UINT i = 0 ; i<col_size ; ++i){
		for(UINT j = i+1 ; j<col_size ; ++j){
			if(col_arr[i]->parent->GetMapID() > col_arr[i]->parent->GetMapID()){continue;}
			if(col_arr[i]->parent->GetMapID() < col_arr[i]->parent->GetMapID()){break;}
			//collision check
			//update collision function
		}
	}
}