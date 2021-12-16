#pragma once

#include <cmath>
#include "Enum.h"

#ifdef _M_X64
typedef __int32			INT;
typedef unsigned __int32	UINT;
typedef float				FLOAT;
#elif defined(__aarch64__) || defined(__x86_64__)
#include <cstdint>
typedef __int32_t			INT;
typedef __uint32_t			UINT;
typedef float				FLOAT;
#elif defined(__arm__) || defined(__i386__)
typedef int					INT;
typedef unsigned int		UINT;
typedef float				FLOAT;
#else
typedef int					INT;
typedef unsigned int		UINT;
typedef float				FLOAT;
#endif

#if defined(WIN32) || defined(_M_X64)
typedef __int16				INT16;
typedef unsigned __int16	UINT16;
typedef __int32				INT32;
typedef unsigned __int32	UINT32;
typedef __int64				INT64;
typedef unsigned __int64	UINT64;
#elif defined(__aarch64__) || defined(__x86_64__) || defined(__arm__) || defined(__i386__)
#include <cstdint>
typedef __int16_t	INT16;
typedef __uint16_t	UINT16;
typedef __int32_t	INT32;
typedef __uint32_t	UINT32;
typedef __int64_t	INT64;
typedef __uint64_t	UINT64;
#endif

typedef unsigned char BYTE;

#define DEGREE(r) r*(180.0f/3.141592f)
#define RADIAN(d) d*(3.141592f/180.0f)

//default
typedef class FLOAT2{
public:
	FLOAT x;
	FLOAT y;

	static FLOAT2 zero(){return {0.0f, 0.0f};}
	FLOAT2  operator + (FLOAT2 v){return {this->x+v.x, this->y+v.y};}
	FLOAT2  operator - (FLOAT2 v){return {this->x-v.x, this->y-v.y};}
	FLOAT2  operator * (FLOAT f) {return {this->x*f, this->y*f};}
	FLOAT2  operator / (FLOAT f) {return {this->x/f, this->y/f};}
	FLOAT2& operator +=(FLOAT2 v){this->x += v.x; this->y += v.y; return *this;}
	FLOAT2& operator -=(FLOAT2 v){this->x -= v.x; this->y -= v.y; return *this;}
	FLOAT2& operator *=(FLOAT f) {this->x *= f; this->y *= f; return *this;}
	FLOAT2& operator /=(FLOAT f) {this->x /= f; this->y /= f; return *this;}
	bool operator == (FLOAT2 v){
		if(this->x == v.x && this->y == v.y){return true;}
		else {return false;}
	}
	bool operator != (FLOAT2 v){
		if(this->x == v.x && this->y == v.y){return false;}
		else {return true;}
	}
}VECTOR2;

typedef class FLOAT3{
public:
	FLOAT x;
	FLOAT y;
	FLOAT z;

	static FLOAT3 zero(){return {0.0f, 0.0f, 0.0f};}
	FLOAT3  operator + (FLOAT3 v){return {this->x+v.x, this->y+v.y, this->z+v.z};}
	FLOAT3  operator - (FLOAT3 v){return {this->x-v.x, this->y-v.y, this->z-v.z};}
	FLOAT3  operator * (FLOAT f) {return {this->x*f, this->y*f, this->z*f};}
	FLOAT3  operator / (FLOAT f) {return {this->x/f, this->y/f, this->z/f};}
	FLOAT3& operator +=(FLOAT3 v){this->x += v.x; this->y += v.y; this->z += v.z; return *this;}
	FLOAT3& operator -=(FLOAT3 v){this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this;}
	FLOAT3& operator *=(FLOAT f) {this->x *= f; this->y *= f; this->z *= f; return *this;}
	FLOAT3& operator /=(FLOAT f) {this->x /= f; this->y /= f; this->z /= f; return *this;}
	bool operator == (FLOAT3 v){
		if(this->x == v.x && this->y == v.y && this->z == v.z){return true;}
		else {return false;}
	}
	bool operator != (FLOAT3 v){
		if(this->x == v.x && this->y == v.y && this->z == v.z){return false;}
		else {return true;}
	}
}VECTOR3;

typedef class FLOAT4{
public:
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT w;

	static FLOAT4 zero(){return {0.0f, 0.0f, 0.0f, 0.0f};}
	static FLOAT4 Initialize(){return {0.0f, 0.0f, 0.0f, 1.0f};}
	FLOAT4  operator + (FLOAT4 v){return {this->x+v.x, this->y+v.y, this->z+v.z, this->w+v.w};}
	FLOAT4  operator - (FLOAT4 v){return {this->x-v.x, this->y-v.y, this->z-v.z, this->w-v.w};}
	FLOAT4  operator * (FLOAT f) {return {this->x*f, this->y*f, this->z*f, this->w*f};}
	FLOAT4  operator / (FLOAT f) {return {this->x/f, this->y/f, this->z/f, this->w/f};}
	FLOAT4& operator +=(FLOAT4 v){this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this;}
	FLOAT4& operator -=(FLOAT4 v){this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this;}
	FLOAT4& operator *=(FLOAT f) {this->x *= f; this->y *= f; this->z *= f; this->w *= f; return *this;}
	FLOAT4& operator /=(FLOAT f) {this->x /= f; this->y /= f; this->z /= f; this->w /= f; return *this;}
	bool operator == (FLOAT4 v){
		if(this->x == v.x && this->y == v.y && this->z == v.z && this->w == v.w){return true;}
		else {return false;}
	}
	bool operator != (FLOAT4 v){
		if(this->x == v.x && this->y == v.y && this->z == v.z && this->w == v.w){return false;}
		else {return true;}
	}
}VECTOR4;

typedef class FLOAT2x2{
public:
	FLOAT _11, _12;
	FLOAT _21, _22;

	static FLOAT2x2 Initialize(){
		return {1,0,
				0,1};
	};
}MATRIX2x2;

typedef class FLOAT3x3{
public:
	FLOAT _11, _12, _13;
	FLOAT _21, _22, _23;
	FLOAT _31, _32, _33;

	static FLOAT3x3 Initialize(){
		return {1,0,0,
				0,1,0,
				0,0,1};
	};
}MATRIX3x3;

typedef class FLOAT4x4{
public:
	FLOAT _11, _12, _13, _14;
	FLOAT _21, _22, _23, _24;
	FLOAT _31, _32, _33, _34;
	FLOAT _41, _42, _43, _44;

	static FLOAT4x4 Initialize(){
		return {1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1};
	};
}MATRIX4x4;

typedef class QuaternionCalculator{
public:
	static FLOAT4 Quternion(FLOAT3 rot){
		FLOAT4 q = {};

		q.x = (sin(rot.x*0.5f)*cos(rot.y*0.5f)*cos(rot.z*0.5f))-(cos(rot.x*0.5f)*sin(rot.y*0.5f)*sin(rot.z*0.5f));
		q.y = (cos(rot.x*0.5f)*sin(rot.y*0.5f)*cos(rot.z*0.5f))+(sin(rot.x*0.5f)*cos(rot.y*0.5f)*sin(rot.z*0.5f));
		q.z = (cos(rot.x*0.5f)*cos(rot.y*0.5f)*sin(rot.z*0.5f))-(sin(rot.x*0.5f)*sin(rot.y*0.5f)*cos(rot.z*0.5f));
		q.w = (cos(rot.x*0.5f)*cos(rot.y*0.5f)*cos(rot.z*0.5f))+(sin(rot.x*0.5f)*sin(rot.y*0.5f)*sin(rot.z*0.5f));

		return q;
	}
	static MATRIX4x4 QuarternionMatrix(FLOAT3 rot){
		FLOAT4 q = Quternion(rot);

		//1-2yy-2zz , -2wz+2xy , 2wy+2xz
		//2wz+2xy , 1-2xx-2zz , -2wx+2yz
		//-2wy+2xz , 2wx+2yz , 1-2xx-2yy
		/*MATRIX4x4 mat = {
			1-(2*q.y*q.y)-(2*q.z*q.z), (2*q.x*q.y)-(2*q.w*q.z), (2*q.x*q.z)+(2*q.w*q.y), 0,
			(2*q.x*q.y)+(2*q.w*q.z), 1-(2*q.x*q.x)-(2*q.z*q.z), (2*q.y*q.z)-(2*q.w*q.x), 0,
			(2*q.x*q.z)-(2*q.w*q.y), (2*q.y*q.z)+(2*q.w*q.x), 1-(2*q.x*q.x)-(2*q.y*q.y), 0,
			0, 0, 0, 1
		};*/

		MATRIX4x4 mat = {
			1.0f-(2.0f*q.y*q.y)-(2.0f*q.z*q.z), (2.0f*q.x*q.y)+(2.0f*q.w*q.z), (2.0f*q.x*q.z)-(2.0f*q.w*q.y), 0,
			(2.0f*q.x*q.y)-(2.0f*q.w*q.z), 1.0f-(2.0f*q.x*q.x)-(2.0f*q.z*q.z), (2.0f*q.y*q.z)+(2.0f*q.w*q.x), 0,
			(2.0f*q.x*q.z)+(2.0f*q.w*q.y), (2.0f*q.y*q.z)-(2.0f*q.w*q.x), 1.0f-(2.0f*q.x*q.x)-(2.0f*q.y*q.y), 0,
			0, 0, 0, 1
		};

		return mat;
	}
}QUATCAL;

typedef class MatrixCalculator{
public:
	static MATRIX4x4 WorldMatrix(FLOAT3 pos, FLOAT3 rot, FLOAT3 scl, MATRIX4x4 p_mat = MATRIX4x4::Initialize()){
		//scale->rotation->Position
		MATRIX4x4 t_mat = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};

		MATRIX4x4 t_rot = QUATCAL::QuarternionMatrix(rot);

		t_mat._11 = scl.x * t_rot._11;
		t_mat._12 = scl.x * t_rot._12;
		t_mat._13 = scl.x * t_rot._13;
		t_mat._14 = 0;

		t_mat._21 = scl.y * t_rot._21;
		t_mat._22 = scl.y * t_rot._22;
		t_mat._23 = scl.y * t_rot._23;
		t_mat._14 = 0;

		t_mat._31 = scl.z * t_rot._31;
		t_mat._32 = scl.z * t_rot._32;
		t_mat._33 = scl.z * t_rot._33;
		t_mat._34 = 0;

		t_mat._41 = pos.x;
		t_mat._42 = pos.y;
		t_mat._43 = pos.z;
		t_mat._44 = 1;

		//parent * me
		MATRIX4x4 r_mat = MultipleMatrix(t_mat, p_mat);
		/*
		r_mat._11 = (t_mat._11 * p_mat._11) + (t_mat._12 * p_mat._21) + (t_mat._13 * p_mat._31) + (t_mat._14 * p_mat._41);
		r_mat._12 = (t_mat._11 * p_mat._12) + (t_mat._12 * p_mat._22) + (t_mat._13 * p_mat._32) + (t_mat._14 * p_mat._42);
		r_mat._13 = (t_mat._11 * p_mat._13) + (t_mat._12 * p_mat._23) + (t_mat._13 * p_mat._33) + (t_mat._14 * p_mat._43);
		r_mat._14 = (t_mat._11 * p_mat._14) + (t_mat._12 * p_mat._24) + (t_mat._13 * p_mat._34) + (t_mat._14 * p_mat._44);

		r_mat._21 = (t_mat._21 * p_mat._11) + (t_mat._22 * p_mat._21) + (t_mat._23 * p_mat._31) + (t_mat._24 * p_mat._41);
		r_mat._22 = (t_mat._21 * p_mat._12) + (t_mat._22 * p_mat._22) + (t_mat._23 * p_mat._32) + (t_mat._24 * p_mat._42);
		r_mat._23 = (t_mat._21 * p_mat._13) + (t_mat._22 * p_mat._23) + (t_mat._23 * p_mat._33) + (t_mat._24 * p_mat._43);
		r_mat._24 = (t_mat._21 * p_mat._14) + (t_mat._22 * p_mat._24) + (t_mat._23 * p_mat._34) + (t_mat._24 * p_mat._44);

		r_mat._31 = (t_mat._31 * p_mat._11) + (t_mat._32 * p_mat._21) + (t_mat._33 * p_mat._31) + (t_mat._34 * p_mat._41);
		r_mat._32 = (t_mat._31 * p_mat._12) + (t_mat._32 * p_mat._22) + (t_mat._33 * p_mat._32) + (t_mat._34 * p_mat._42);
		r_mat._33 = (t_mat._31 * p_mat._13) + (t_mat._32 * p_mat._23) + (t_mat._33 * p_mat._33) + (t_mat._34 * p_mat._43);
		r_mat._34 = (t_mat._31 * p_mat._14) + (t_mat._32 * p_mat._24) + (t_mat._33 * p_mat._34) + (t_mat._34 * p_mat._44);

		r_mat._41 = (t_mat._41 * p_mat._11) + (t_mat._42 * p_mat._21) + (t_mat._43 * p_mat._31) + (t_mat._44 * p_mat._41);
		r_mat._42 = (t_mat._41 * p_mat._12) + (t_mat._42 * p_mat._22) + (t_mat._43 * p_mat._32) + (t_mat._44 * p_mat._42);
		r_mat._43 = (t_mat._41 * p_mat._13) + (t_mat._42 * p_mat._23) + (t_mat._43 * p_mat._33) + (t_mat._44 * p_mat._43);
		r_mat._44 = (t_mat._41 * p_mat._14) + (t_mat._42 * p_mat._24) + (t_mat._43 * p_mat._34) + (t_mat._44 * p_mat._44);
		*/
		return r_mat;
	}
	static MATRIX4x4 ViewMatrix(FLOAT3 eye, FLOAT3 lookat, FLOAT3 up){
		//z axis : normalize(eye-lookat)
		FLOAT3 z_axis = {lookat.x-eye.x, lookat.y-eye.y, lookat.z-eye.z};
		FLOAT size = sqrt((z_axis.x*z_axis.x)+(z_axis.y*z_axis.y)+(z_axis.z*z_axis.z));
		z_axis.x /= size; z_axis.y /= size; z_axis.z /= size;
		//x axis : normalize(cross(z_axis, up));
		FLOAT3 x_axis = { (z_axis.y*up.z)-(z_axis.z*up.y),
			(z_axis.z*up.x)-(z_axis.x*up.z),
			(z_axis.x*up.y)-(z_axis.y*up.x)};
		size = sqrt((x_axis.x*x_axis.x)+(x_axis.y*x_axis.y)+(x_axis.z*x_axis.z));
		x_axis.x /= size; x_axis.y /= size; x_axis.z /= size;
		//y axis : normalize(cross(x_axis, z_axis));
		FLOAT3 y_axis = { (x_axis.y*z_axis.z)-(x_axis.z*z_axis.y),
			(x_axis.z*z_axis.x)-(x_axis.x*z_axis.z),
			(x_axis.x*z_axis.y)-(x_axis.y*z_axis.x)};
		size = sqrt((y_axis.x*y_axis.x)+(y_axis.y*y_axis.y)+(y_axis.z*y_axis.z));
		y_axis.x /= size; y_axis.y /= size; y_axis.z /= size;
		//pos : dot(x_axis,eye) dot(y_axis,eye) dot(z_axis,eye)
		FLOAT3 pos = {-((x_axis.x*eye.x)+(x_axis.y*eye.y)+(x_axis.z*eye.z)),
			-((y_axis.x*eye.x)+(y_axis.y*eye.y)+(y_axis.z*eye.z)),
			-((z_axis.x*eye.x)+(z_axis.y*eye.y)+(z_axis.z*eye.z))};

		//setting
		return {  x_axis.x, y_axis.x, z_axis.x, 0,
			x_axis.y, y_axis.y, z_axis.y, 0,
			x_axis.z, y_axis.z, z_axis.z, 0,
			pos.x,    pos.y,    pos.z, 1};
	}	//SetView
	static MATRIX4x4 OrthoProjectionMatrix(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f){
		return {
			2.0f/(r-l), 0,0,0,
			0, 2.0f/(b-t),0,0,
			0,0, -2.0f/(f-n),0,
			-((r+l)/(r-l)),-((b+t)/(b-t)),-((f+n)/(f-n)),1.0f
		};
	}	//SetProjection
	static MATRIX4x4 PerspectiveProjectionMatrix(FLOAT aspect, FLOAT fovY, FLOAT n, FLOAT f){
		FLOAT y = n*tan(fovY/2);
		FLOAT x = y*aspect;
		FLOAT l = -x, r = x, t = -y, b = y;
		return {
			(2*n)/(r-l),0,0,0,
			0,(2*n)/(b-t),0,0,
			(r+l)/(r-l),(b+t)/(b-t),((f+n)/(f-n)),1,
			0,0,-((2*f*n)/(f-n)),0
		};
	}
	static MATRIX4x4 ReverseViewProjectionMatrix(MATRIX4x4 view, MATRIX4x4 proj){
		MATRIX4x4 r = {};

		r._11 = (view._11*proj._11)+(view._12*proj._21)+(view._13*proj._31)+(view._14*proj._41);
		r._12 = (view._11*proj._12)+(view._12*proj._22)+(view._13*proj._32)+(view._14*proj._42);
		r._13 = (view._11*proj._13)+(view._12*proj._23)+(view._13*proj._33)+(view._14*proj._43);
		r._14 = (view._11*proj._14)+(view._12*proj._24)+(view._13*proj._34)+(view._14*proj._44);

		r._21 = (view._21*proj._11)+(view._22*proj._21)+(view._23*proj._31)+(view._24*proj._41);
		r._22 = (view._21*proj._12)+(view._22*proj._22)+(view._23*proj._32)+(view._24*proj._42);
		r._23 = (view._21*proj._13)+(view._22*proj._23)+(view._23*proj._33)+(view._24*proj._43);
		r._24 = (view._21*proj._14)+(view._22*proj._24)+(view._23*proj._34)+(view._24*proj._44);

		r._31 = (view._31*proj._11)+(view._32*proj._21)+(view._33*proj._31)+(view._34*proj._41);
		r._32 = (view._31*proj._12)+(view._32*proj._22)+(view._33*proj._32)+(view._34*proj._42);
		r._33 = (view._31*proj._13)+(view._32*proj._23)+(view._33*proj._33)+(view._34*proj._43);
		r._34 = (view._31*proj._14)+(view._32*proj._24)+(view._33*proj._34)+(view._34*proj._44);

		r._41 = (view._41*proj._11)+(view._42*proj._21)+(view._43*proj._31)+(view._44*proj._41);
		r._42 = (view._41*proj._12)+(view._42*proj._22)+(view._43*proj._32)+(view._44*proj._42);
		r._43 = (view._41*proj._13)+(view._42*proj._23)+(view._43*proj._33)+(view._44*proj._43);
		r._44 = (view._41*proj._14)+(view._42*proj._24)+(view._43*proj._34)+(view._44*proj._44);

		FLOAT det = 
			(r._11*(
			(r._22*r._33*r._44)-(r._22*r._34*r._43)+
				(r._23*r._34*r._42)-(r._23*r._32*r._44)+
				(r._24*r._32*r._43)-(r._24*r._33*r._42)))-
				(r._12*(
			(r._23*r._34*r._41)-(r._23*r._31*r._44)+
					(r._24*r._31*r._43)-(r._24*r._33*r._41)+
					(r._21*r._33*r._44)-(r._21*r._34*r._43)))+
					(r._13*(
			(r._24*r._31*r._42)-(r._24*r._32*r._41)+
						(r._21*r._32*r._44)-(r._21*r._34*r._42)+
						(r._22*r._34*r._41)-(r._22*r._31*r._44)))-
						(r._14*(
			(r._21*r._32*r._43)-(r._21*r._33*r._42)+
							(r._22*r._33*r._41)-(r._22*r._31*r._43)+
							(r._23*r._31*r._42)-(r._23*r._32*r._41)))
			;

		if(det == 0.0f){return MATRIX4x4::Initialize();}

		MATRIX4x4 adj = {};

		adj._11 = (
			(r._22*r._33*r._44)-(r._22*r._34*r._43)+
			(r._23*r._34*r._42)-(r._23*r._32*r._44)+
			(r._24*r._32*r._43)-(r._24*r._33*r._42));
		adj._21 = -(
			(r._21*r._33*r._44)-(r._21*r._34*r._43)+
			(r._23*r._34*r._41)-(r._23*r._31*r._44)+
			(r._24*r._31*r._43)-(r._24*r._33*r._41));
		adj._31 = (
			(r._21*r._32*r._44)-(r._21*r._34*r._42)+
			(r._22*r._34*r._41)-(r._22*r._31*r._44)+
			(r._24*r._31*r._42)-(r._24*r._32*r._41));
		adj._41 = -(
			(r._21*r._32*r._43)-(r._21*r._33*r._42)+
			(r._22*r._33*r._41)-(r._22*r._31*r._43)+
			(r._23*r._31*r._42)-(r._23*r._32*r._41));

		adj._12 = -(
			(r._12*r._33*r._44)-(r._12*r._34*r._43)+
			(r._13*r._34*r._42)-(r._13*r._32*r._44)+
			(r._14*r._32*r._43)-(r._14*r._33*r._42));
		adj._22 = (
			(r._11*r._33*r._44)-(r._11*r._34*r._43)+
			(r._13*r._34*r._41)-(r._13*r._31*r._44)+
			(r._14*r._31*r._43)-(r._14*r._33*r._41));
		adj._32 = -(
			(r._11*r._32*r._44)-(r._11*r._34*r._42)+
			(r._12*r._34*r._41)-(r._12*r._31*r._44)+
			(r._14*r._31*r._42)-(r._14*r._32*r._41));
		adj._42 = (
			(r._11*r._32*r._43)-(r._11*r._33*r._42)+
			(r._12*r._33*r._41)-(r._12*r._31*r._43)+
			(r._13*r._31*r._42)-(r._13*r._32*r._41));

		adj._13 = (
			(r._12*r._23*r._44)-(r._12*r._24*r._43)+
			(r._13*r._24*r._42)-(r._13*r._22*r._44)+
			(r._14*r._22*r._43)-(r._14*r._23*r._42));
		adj._23 = -(
			(r._11*r._23*r._44)-(r._11*r._24*r._43)+
			(r._13*r._24*r._41)-(r._13*r._21*r._44)+
			(r._14*r._21*r._43)-(r._14*r._23*r._41));
		adj._33 = (
			(r._11*r._22*r._44)-(r._11*r._24*r._42)+
			(r._12*r._24*r._41)-(r._12*r._21*r._44)+
			(r._14*r._21*r._42)-(r._14*r._22*r._41));
		adj._43 = -(
			(r._11*r._22*r._43)-(r._11*r._23*r._42)+
			(r._12*r._23*r._41)-(r._12*r._21*r._43)+
			(r._13*r._21*r._42)-(r._13*r._22*r._41));

		adj._14 = -(
			(r._12*r._23*r._34)-(r._12*r._24*r._33)+
			(r._13*r._24*r._32)-(r._13*r._22*r._34)+
			(r._14*r._22*r._33)-(r._14*r._23*r._32));
		adj._24 = (
			(r._11*r._23*r._34)-(r._11*r._24*r._33)+
			(r._13*r._24*r._31)-(r._13*r._21*r._34)+
			(r._14*r._21*r._33)-(r._14*r._23*r._31));
		adj._34 = -(
			(r._11*r._22*r._34)-(r._11*r._24*r._32)+
			(r._12*r._24*r._31)-(r._12*r._21*r._34)+
			(r._14*r._21*r._32)-(r._14*r._22*r._31));
		adj._44 = (
			(r._11*r._22*r._33)-(r._11*r._23*r._32)+
			(r._12*r._23*r._31)-(r._12*r._21*r._33)+
			(r._13*r._21*r._32)-(r._13*r._22*r._31));

		r._11 = adj._11/det;	r._12 = adj._12/det;	r._13 = adj._13/det;	r._14 = adj._14/det;
		r._21 = adj._21/det;	r._22 = adj._22/det;	r._23 = adj._23/det;	r._24 = adj._24/det;
		r._31 = adj._31/det;	r._32 = adj._32/det;	r._33 = adj._33/det;	r._34 = adj._34/det;
		r._41 = adj._41/det;	r._42 = adj._42/det;	r._43 = adj._43/det;	r._44 = adj._44/det;

		return r;
	}
	static MATRIX4x4 ReverseMatrix(MATRIX4x4 r){
		FLOAT det = 
			(r._11*(
			(r._22*r._33*r._44)-(r._22*r._34*r._43)+
				(r._23*r._34*r._42)-(r._23*r._32*r._44)+
				(r._24*r._32*r._43)-(r._24*r._33*r._42)))-
				(r._12*(
			(r._23*r._34*r._41)-(r._23*r._31*r._44)+
					(r._24*r._31*r._43)-(r._24*r._33*r._41)+
					(r._21*r._33*r._44)-(r._21*r._34*r._43)))+
					(r._13*(
			(r._24*r._31*r._42)-(r._24*r._32*r._41)+
						(r._21*r._32*r._44)-(r._21*r._34*r._42)+
						(r._22*r._34*r._41)-(r._22*r._31*r._44)))-
						(r._14*(
			(r._21*r._32*r._43)-(r._21*r._33*r._42)+
							(r._22*r._33*r._41)-(r._22*r._31*r._43)+
							(r._23*r._31*r._42)-(r._23*r._32*r._41)))
			;

		if(det == 0.0f){return MATRIX4x4::Initialize();}

		MATRIX4x4 adj = {};

		adj._11 = (
			(r._22*r._33*r._44)-(r._22*r._34*r._43)+
			(r._23*r._34*r._42)-(r._23*r._32*r._44)+
			(r._24*r._32*r._43)-(r._24*r._33*r._42));
		adj._21 = -(
			(r._21*r._33*r._44)-(r._21*r._34*r._43)+
			(r._23*r._34*r._41)-(r._23*r._31*r._44)+
			(r._24*r._31*r._43)-(r._24*r._33*r._41));
		adj._31 = (
			(r._21*r._32*r._44)-(r._21*r._34*r._42)+
			(r._22*r._34*r._41)-(r._22*r._31*r._44)+
			(r._24*r._31*r._42)-(r._24*r._32*r._41));
		adj._41 = -(
			(r._21*r._32*r._43)-(r._21*r._33*r._42)+
			(r._22*r._33*r._41)-(r._22*r._31*r._43)+
			(r._23*r._31*r._42)-(r._23*r._32*r._41));

		adj._12 = -(
			(r._12*r._33*r._44)-(r._12*r._34*r._43)+
			(r._13*r._34*r._42)-(r._13*r._32*r._44)+
			(r._14*r._32*r._43)-(r._14*r._33*r._42));
		adj._22 = (
			(r._11*r._33*r._44)-(r._11*r._34*r._43)+
			(r._13*r._34*r._41)-(r._13*r._31*r._44)+
			(r._14*r._31*r._43)-(r._14*r._33*r._41));
		adj._32 = -(
			(r._11*r._32*r._44)-(r._11*r._34*r._42)+
			(r._12*r._34*r._41)-(r._12*r._31*r._44)+
			(r._14*r._31*r._42)-(r._14*r._32*r._41));
		adj._42 = (
			(r._11*r._32*r._43)-(r._11*r._33*r._42)+
			(r._12*r._33*r._41)-(r._12*r._31*r._43)+
			(r._13*r._31*r._42)-(r._13*r._32*r._41));

		adj._13 = (
			(r._12*r._23*r._44)-(r._12*r._24*r._43)+
			(r._13*r._24*r._42)-(r._13*r._22*r._44)+
			(r._14*r._22*r._43)-(r._14*r._23*r._42));
		adj._23 = -(
			(r._11*r._23*r._44)-(r._11*r._24*r._43)+
			(r._13*r._24*r._41)-(r._13*r._21*r._44)+
			(r._14*r._21*r._43)-(r._14*r._23*r._41));
		adj._33 = (
			(r._11*r._22*r._44)-(r._11*r._24*r._42)+
			(r._12*r._24*r._41)-(r._12*r._21*r._44)+
			(r._14*r._21*r._42)-(r._14*r._22*r._41));
		adj._43 = -(
			(r._11*r._22*r._43)-(r._11*r._23*r._42)+
			(r._12*r._23*r._41)-(r._12*r._21*r._43)+
			(r._13*r._21*r._42)-(r._13*r._22*r._41));

		adj._14 = -(
			(r._12*r._23*r._34)-(r._12*r._24*r._33)+
			(r._13*r._24*r._32)-(r._13*r._22*r._34)+
			(r._14*r._22*r._33)-(r._14*r._23*r._32));
		adj._24 = (
			(r._11*r._23*r._34)-(r._11*r._24*r._33)+
			(r._13*r._24*r._31)-(r._13*r._21*r._34)+
			(r._14*r._21*r._33)-(r._14*r._23*r._31));
		adj._34 = -(
			(r._11*r._22*r._34)-(r._11*r._24*r._32)+
			(r._12*r._24*r._31)-(r._12*r._21*r._34)+
			(r._14*r._21*r._32)-(r._14*r._22*r._31));
		adj._44 = (
			(r._11*r._22*r._33)-(r._11*r._23*r._32)+
			(r._12*r._23*r._31)-(r._12*r._21*r._33)+
			(r._13*r._21*r._32)-(r._13*r._22*r._31));

		r._11 = adj._11/det;	r._12 = adj._12/det;	r._13 = adj._13/det;	r._14 = adj._14/det;
		r._21 = adj._21/det;	r._22 = adj._22/det;	r._23 = adj._23/det;	r._24 = adj._24/det;
		r._31 = adj._31/det;	r._32 = adj._32/det;	r._33 = adj._33/det;	r._34 = adj._34/det;
		r._41 = adj._41/det;	r._42 = adj._42/det;	r._43 = adj._43/det;	r._44 = adj._44/det;

		return r;
	}
	static MATRIX4x4 MultipleMatrix(MATRIX4x4 a, MATRIX4x4 b){
		MATRIX4x4 m = {};
		m._11 = (a._11*b._11)+(a._12*b._21)+(a._13*b._31)+(a._14*b._41);
		m._12 = (a._11*b._12)+(a._12*b._22)+(a._13*b._32)+(a._14*b._42);
		m._13 = (a._11*b._13)+(a._12*b._23)+(a._13*b._33)+(a._14*b._43);
		m._14 = (a._11*b._14)+(a._12*b._24)+(a._13*b._34)+(a._14*b._44);
		m._21 = (a._21*b._11)+(a._22*b._21)+(a._23*b._31)+(a._24*b._41);
		m._22 = (a._21*b._12)+(a._22*b._22)+(a._23*b._32)+(a._24*b._42);
		m._23 = (a._21*b._13)+(a._22*b._23)+(a._23*b._33)+(a._24*b._43);
		m._24 = (a._21*b._14)+(a._22*b._24)+(a._23*b._34)+(a._24*b._44);
		m._31 = (a._31*b._11)+(a._32*b._21)+(a._33*b._31)+(a._34*b._41);
		m._32 = (a._31*b._12)+(a._32*b._22)+(a._33*b._32)+(a._34*b._42);
		m._33 = (a._31*b._13)+(a._32*b._23)+(a._33*b._33)+(a._34*b._43);
		m._34 = (a._31*b._14)+(a._32*b._24)+(a._33*b._34)+(a._34*b._44);
		m._41 = (a._41*b._11)+(a._42*b._21)+(a._43*b._31)+(a._44*b._41);
		m._42 = (a._41*b._12)+(a._42*b._22)+(a._43*b._32)+(a._44*b._42);
		m._43 = (a._41*b._13)+(a._42*b._23)+(a._43*b._33)+(a._44*b._43);
		m._44 = (a._41*b._14)+(a._42*b._24)+(a._43*b._34)+(a._44*b._44);
		return m;
	}
}MATCAL;

typedef class VectorCalculator{
public:
	static FLOAT dot(FLOAT2 v1, FLOAT2 v2){return v1.x*v2.x+v1.y*v2.y;}
	static FLOAT dot(FLOAT3 v1, FLOAT3 v2){return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;}
	static FLOAT cross(FLOAT2 v1, FLOAT2 v2){return v1.x*v2.y-v2.x*v1.y;}
	static FLOAT3 cross(FLOAT3 v1, FLOAT3 v2){return {v1.y*v2.z-v2.y*v1.z, v1.z*v2.x-v2.z*v1.x, v1.x*v2.y-v2.x*v1.y};}
	static FLOAT length_square(FLOAT2 v){return v.x*v.x+v.y*v.y;}
	static FLOAT length_square(FLOAT3 v){return v.x*v.x+v.y*v.y+v.z*v.z;}
	static FLOAT length(FLOAT2 v){return sqrt(v.x*v.x+v.y*v.y);}
	static FLOAT length(FLOAT3 v){return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
	static FLOAT2 normalize(FLOAT2 v){FLOAT len = length(v); if(len == 0.0f){return v;} return v/len;}
	static FLOAT3 normalize(FLOAT3 v){FLOAT len = length(v); if(len == 0.0f){return v;} return v/len;}

	static FLOAT2 MultipleMatrix(FLOAT2 v, MATRIX4x4 m){
		FLOAT2 result = {0.0f, 0.0f};
		result.x = v.x*m._11+v.y*m._21+m._41;
		result.y = v.x*m._12+v.y*m._22+m._42;
		return result;
	}
	static FLOAT3 MultipleMatrix(FLOAT3 v, MATRIX4x4 m){
		FLOAT3 result = {0.0f, 0.0f, 0.0f};
		result.x = v.x*m._11+v.y*m._21+v.z*m._31+m._41;
		result.y = v.x*m._12+v.y*m._22+v.z*m._32+m._42;
		result.z = v.x*m._13+v.y*m._23+v.z*m._33+m._43;
		return result;
	}
	static FLOAT4 MultipleMatrix(FLOAT4 v, MATRIX4x4 m){
		FLOAT4 result = {0.0f, 0.0f, 0.0f, 1.0f};
		result.x = v.x*m._11+v.y*m._21+v.z*m._31+v.w*m._41;
		result.y = v.x*m._12+v.y*m._22+v.z*m._32+v.w*m._42;
		result.z = v.x*m._13+v.y*m._23+v.z*m._33+v.w*m._43;
		result.w = v.x*m._14+v.y*m._24+v.z*m._34+v.w*m._44;
		return result;
	}

	//Spline Set
	//Bezier : sigma(n, i=0) n!/i!*(n-1)! * (1-t)^(n-i) * t^i * pn
	static VECTOR2 BezierSpline(VECTOR2* p, INT p_num, FLOAT t){
		FLOAT n = 1.0f;
		for(UINT i = 1 ; i<p_num ; ++i){
			n *= i;
		}
		VECTOR2 r = VECTOR2::zero();
		for(INT i = 0 ; i<p_num ; ++i){
			FLOAT temp = 1.0f;
			FLOAT k = 1.0f;
			FLOAT nk = 1.0f;
			for(INT j = 1 ; j<p_num ; ++j){
				if(j>i){temp *= (1.0f-t);}
				else{temp *= t;}
				if(j<=i){k *= j;}
				if(j<(p_num-i)){nk *= j;}
			}
			FLOAT constant = n/(k*nk);
			r.x += constant*p[i].x*temp;
			r.y += constant*p[i].y*temp;
		}
		return r;
	}
	static VECTOR3 BezierSpline(VECTOR3* p, INT p_num, FLOAT t){
		FLOAT n = 1.0f;
		for(UINT i = 1 ; i<p_num ; ++i){
			n *= i;
		}
		VECTOR3 r = {0.0f, 0.0f, 0.0f};
		for(INT i = 0 ; i<p_num ; ++i){
			FLOAT temp = 1.0f;
			FLOAT k = 1.0f;
			FLOAT nk = 1.0f;
			for(INT j = 1 ; j<p_num ; ++j){
				if(j>i){temp *= (1.0f-t);}
				else{temp *= t;}
				if(j<=i){k *= j;}
				if(j<(p_num-i)){nk *= j;}
			}
			FLOAT constant = n/(k*nk);
			r.x += constant*p[i].x*temp;
			r.y += constant*p[i].y*temp;
			r.z += constant*p[i].z*temp;
		}
		return r;
	}
	static VECTOR2 BezierSpline2(VECTOR2* p, FLOAT t){return (p[0]*(1.0f-t)*(1.0f-t))+(p[1]*t*(1.0f-t)*2.0f)+(p[2]*t*t);}
	static VECTOR3 BezierSpline2(VECTOR3* p, FLOAT t){return (p[0]*(1.0f-t)*(1.0f-t))+(p[1]*t*(1.0f-t)*2.0f)+(p[2]*t*t);}
	static VECTOR2 BezierSpline3(VECTOR2* p, FLOAT t){return (p[0]*(1.0f-t)*(1.0f-t)*(1.0f-t))+(p[1]*t*(1.0f-t)*(1.0f-t)*3.0f)+(p[2]*t*t*(1.0f-t)*3.0f)+(p[3]*t*t*t);}
	static VECTOR3 BezierSpline3(VECTOR3* p, FLOAT t){return (p[0]*(1.0f-t)*(1.0f-t)*(1.0f-t))+(p[1]*t*(1.0f-t)*(1.0f-t)*3.0f)+(p[2]*t*t*(1.0f-t)*3.0f)+(p[3]*t*t*t);}
}VECCAL;

//Interface
struct MOUSE{
	INT x, y, z;
	BYTE button[8];
};

struct KEYBOARD{
	BYTE KEY[256];
};

struct TOUCH{
	FLOAT2* pos;
	BYTE* state;
	UINT size;
};

//Platform
struct PLATFORM_SETTING{
	wchar_t* title;
	UINT x, y;
	bool FullScreen;

	LIBRARY_TYPE::GRAPHIC g_type;
	LIBRARY_TYPE::SOUND s_type;
};

struct WINDOWS_SETTING : public PLATFORM_SETTING{
	void* thishInst;
	INT* cmdShow;

	BYTE position;
	bool titlebar;
	bool border;
	bool layered;
};

struct ANDROID_SETTING : public PLATFORM_SETTING{
	void* app_state;
	UINT orientation;
};

//resource
struct RAW{
	BYTE* buf;
	UINT size;
};

struct IMAGE{
	UINT width;
	UINT height;
	UINT bit_depth;
	bool alpha_able;
	BYTE* buf;
};

struct Atlas{
	struct range{
		UINT st_x, st_y;
		UINT ed_x, ed_y;
	}*arr;
	UINT num;
};

struct SOUND{
	UINT16 wFormatTag;		/* format type */
	UINT16 nChannels;		/* number of channels (i.e. mono, stereo...) */
	UINT nSamplesPerSec;	/* sample rate */
	UINT nAvgBytesPerSec;	/* for buffer estimation */
	UINT16 nBlockAlign;		/* block size of data */
	UINT16 wBitsPerSample;	/* number of bits per sample of mono data */
	UINT16 cbSize;			/* the count in bytes of the size of */
	UINT16 samplerate;
	UINT size;
	BYTE* buf;
};
/* older version
struct GLYPH{
	UINT* contours;
	UINT contours_num;
	VECTOR2* point;
	//max point num = contours[contours_num-1]+1

	UINT16 advanceWidth;
};
*/

/*pos, diffuse, normal, tex*/
struct VERTEX{
	FLOAT4 pos;
	FLOAT4 diffuse;
	FLOAT3 normal;
	FLOAT2 tex;
};

struct INDEXED{
	UINT* index;
	UINT num;
};

/*struct MATERIAL{
	void* diffuse;
	void* normal;
	void* height;
	void* ambient_occlusion;
};*/

struct UNIFORM{
	_UNIFORM::TYPE type;
	INT location;
};

struct SHADER{
	void* shader;
	UNIFORM* uniform;
	UINT uniform_size;
};

//Graphic variable
struct SCREEN{
	UINT x, y, width, height;
	INT depth_distance;
	MATRIX4x4 view,proj;

	SHADER* shd;
	VERTEX* vtx;
	UINT* idx;

	void* screen_buffer;
	void* screen_buffer_texture;
	void* screen_listhead;
	void* screen_counter;
};

struct RENDER_OBJECT{
	_MODULE::TYPE type;
	UINT vertex_num;
	UINT index_num;
	SHADER* shader;

	UINT textures[16];
	UINT texture_num;
};

struct GLYPH{
	VERTEX vtx[4];	//after change convex hull
	UINT vtx_num;
	INDEXED idx;

	BYTE* buf;
	UINT size;
	UINT advanceWidth;

	wchar_t word;
};

struct GCHAR{
	GLYPH* glyph;
	MATRIX4x4 world;
	FLOAT outline_size;
	FLOAT4 solid_color;
	FLOAT4 outline_color;
};

struct RENDER_TEXT{
	_MODULE::TYPE type;
	MATRIX4x4 world;
	GCHAR* gchar;
	UINT gchar_size;
	SHADER* shader;
};
