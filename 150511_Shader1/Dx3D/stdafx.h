// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <math.h>
#include <assert.h>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


extern HWND		g_hWnd;

#define SAFE_DELETE(p) {if(p){delete p; p = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p){delete [] p; p = NULL;}}
#define SAFE_RELEASE(p) {if(p){p->Release();p = NULL;}}
#define SAFE_ADD_REF(p) {if(p){p->AddRef();}}

#define SINGLETONE(class_name) private: class_name(void); ~class_name(void);\
public:	static class_name* GetInstance() {	static class_name instance;	return &instance; } 

#define SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: inline varType Get##funName(void) const { return varName; }\
public: inline void Set##funName(varType var){ varName = var; }

#define SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: inline varType Get##funName(void) const { return varName; }\
public: inline void Set##funName(varType& var){ varName = var; }

#define SYNTHESIZE_ADD_REF(varType, varName, funName)    \
protected: varType varName; \
public: virtual varType Get##funName(void) const { return varName; } \
public: virtual void Set##funName(varType var){\
	if (varName != var) {\
	SAFE_ADD_REF(var);\
	SAFE_RELEASE(varName);\
	varName = var;\
	}\
}

struct ST_P_VERTEX
{
	D3DXVECTOR3 p;
	enum { FVF = D3DFVF_XYZ };
};

struct ST_PC_VERTEX
{
	D3DXVECTOR3 p;
	D3DCOLOR	c;
	enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
};

struct ST_PT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR2	t;
	enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1 };
};

struct ST_PN_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3	n;
	enum { FVF = D3DFVF_XYZ | D3DFVF_NORMAL };
};

struct ST_PNT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
	enum { FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 };
};

struct ST_PCT_VERTEX
{
	D3DXVECTOR3 p;
	D3DCOLOR	c;
	D3DXVECTOR2	t;
	enum {FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1};
};

struct ST_ROT_SAMPLE
{
	int				nKey;
	D3DXQUATERNION	q;
};

struct ST_POS_SAMPLE
{
	int				nKey;
	D3DXVECTOR3		v;
};

struct ST_SIZEF
{
	float fWidth;
	float fHeight;
	ST_SIZEF() : fWidth(0), fHeight(0) {}
	ST_SIZEF(float _fWidth, float _fHeight) : fWidth(_fWidth), fHeight(_fHeight) {}
};

struct ST_BOUNDING_SPHERE
{
	D3DXVECTOR3 vCenter;
	float		fRadius;

	ST_BOUNDING_SPHERE() 
		: vCenter(0, 0, 0)
		, fRadius(0) 
	{
	}
	ST_BOUNDING_SPHERE(D3DXVECTOR3 _vCenter, float _fRadius)
		: vCenter(_vCenter)
		, fRadius(_fRadius)
	{
	}
};

#include "cDeviceManager.h"
#include "cTextureManager.h"
#include "cSkinnedMeshManager.h"
#include "cTimeManager.h"
