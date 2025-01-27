// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <assert.h>

#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

extern HWND g_hWnd;


#define SAFE_DELETE(p) {if(p){delete p;p = NULL;}}
#define SAFE_RELEASE(p) {if(p){p->Release();p = NULL;}}

#define SINGLETONE(class_name) private: class_name(void); ~class_name(void); \
	public: static class_name* GetInstance()\
	{\
		static class_name instance;\
		return &instance;\
	}

// 1. Position 정점의 좌표 x,y,z(float)                 :  D3DFVF_XYZ
// 2. RHW (float)                                              :  D3DFVF_XYZRHW
// (D3DFVF_XYZ 또는 D3DFVF_NORMAL과 같이 사용불가)
// 3. Blending Weight Data 결합 가중치 (float)         : D3DFVF_XYZB1 ~ D3DFVF_XYZB5
// 4. Vertex Normal 정점의 법선 벡터 x,y,z(float)    : D3DFVF_NORMAL
// 5. Vertex Point Size 정점의 점 크기 (float)          : D3DFVF_PSIZE
// 6. Diffuse Color (DWORD)                               : D3DFVF_DIFFUSE
// 7. Specular Color (DWORD)                            D3DFVF_SPECULAR
// 8. Texture Coordinate Set 1 (float)                   : D3DFVF_TEX0 - D3DFVF_TEX8
struct ST_PC_VERTEX
{
	//float x, y, z;
	D3DXVECTOR3 p;
	D3DCOLOR	c;

	enum {FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE};
};

struct ST_PT_VERTEX
{
	D3DXVECTOR3 p;
	//float u, v;
	D3DXVECTOR2	t;

	enum {FVF = D3DFVF_XYZ | D3DFVF_TEX1};
};

#include "cDeviceManager.h"