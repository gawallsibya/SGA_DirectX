#include "StdAfx.h"
#include "cMainGame.h"
#include "cCube.h"
#include "cGrid.h"
#include "cCamera.h"
#include "cObjLoader.h"
#include "cGroup.h"
#include "cAseLoader.h"
#include "cFrame.h"
#include "cMtlTex.h"
#include "cRay.h"
#include "cDijkstraNode.h"
#include "cSkinnedMesh.h"

cMainGame::cMainGame(void)
	: m_pCamera(NULL)
	, m_pGrid(NULL)
	, m_pFont(NULL)
// 	, m_pRoot(NULL)
// 	, m_pMesh(NULL)
// 	, m_pGroupHead(NULL)
	, m_pSphere(NULL)
	, m_pTeapot(NULL)
	, m_vTeapotPos(0, 0, 0)
	, m_fTeapotRadius(0.0f)
	, m_pPickedNode(NULL)
	, m_pStartNode(NULL)
	, m_pDestNode(NULL)
	, m_pSkinnedMesh(NULL)
{
}

cMainGame::~cMainGame(void)
{
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pGrid);
	SAFE_DELETE(m_pSkinnedMesh);

	SAFE_RELEASE(m_pFont);
	
	SAFE_RELEASE(m_pSphere);
	SAFE_RELEASE(m_pTeapot);
	
// 
// 	for (size_t i = 0; i < m_vecMtlTex.size(); ++i)
// 	{
// 		SAFE_RELEASE(m_vecMtlTex[i]);
// 	}
// 	m_vecMtlTex.clear();
// 
// 	m_pRoot->Destroy();
// 
// 	m_pGroupHead->Destroy();

	g_pTextureManager->Destroy();

	g_pDeviceManager->Destroy();

	
}

void cMainGame::Setup()
{
	m_pSkinnedMesh = new cSkinnedMesh;
	m_pSkinnedMesh->Setup("Zealot", "zealot.X");

//  	cObjLoader l;
//  	m_pGroupHead = l.Load("./obj/Map.obj");
// 
// 	m_pMesh = l.LoadMesh("./obj/Map.obj", m_vecMtlTex);

	int nNodeId = 0;

	for (int i = -5; i <= 5; ++i)
	{
		for (int j = -5; j <= 5; ++j)
		{
			cDijkstraNode* pNode = new cDijkstraNode;
			pNode->SetNodeId(nNodeId++);
			pNode->SetPosition(D3DXVECTOR3(i, 0.01f, j));
			m_vecDijkstraNode.push_back(pNode);
		}
	}

	m_vecEdgeCost.resize(m_vecDijkstraNode.size());
	for (int i = 0; i < m_vecDijkstraNode.size(); ++i)
	{
		m_vecEdgeCost[i].resize(m_vecDijkstraNode.size());
		for (int j = 0; j < m_vecDijkstraNode.size(); ++j)
		{
			m_vecEdgeCost[i][j] = (i == j) ? 0 : COST_INFINITY;
		}
	}


	//D3DXCreateBox(g_pD3DDevice, 2, 2, 2, &m_pSphere, NULL);
	D3DXCreateTeapot(g_pD3DDevice, &m_pTeapot, NULL);
// 
// 	// 주전자의 바운딩 정보 획득
// 	ST_PN_VERTEX* pV = NULL;
// 	m_pTeapot->LockVertexBuffer(0, (LPVOID*)&pV);
// 	D3DXComputeBoundingSphere(&pV[0].p,
// 		m_pTeapot->GetNumVertices(),
// 		sizeof(ST_PN_VERTEX),
// 		&m_vTeapotPos,
// 		&m_fTeapotRadius);
// 	m_pTeapot->UnlockVertexBuffer();

	// 디버깅용 바운딩 스피어 생성.
	D3DXCreateSphere(g_pD3DDevice, 0.2f, 10, 10, &m_pSphere, NULL);
	
// 	cAseLoader loader;
// 	m_pRoot = loader.Load("woman/woman_01_all.ASE");

	ZeroMemory(&m_stWhiteMtl, sizeof(D3DMATERIAL9));
	m_stWhiteMtl.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_stWhiteMtl.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_stWhiteMtl.Specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	ZeroMemory(&m_stRedMtl, sizeof(D3DMATERIAL9));
	m_stRedMtl.Ambient = D3DXCOLOR(0.8f, 0.0f, 0.0f, 1.0f);
	m_stRedMtl.Diffuse = D3DXCOLOR(0.8f, 0.0f, 0.0f, 1.0f);
	m_stRedMtl.Specular = D3DXCOLOR(0.8f, 0.0f, 0.0f, 1.0f);


	//폰트 생성
	D3DXFONT_DESC fd;
	ZeroMemory(&fd,sizeof(D3DXFONT_DESC));
	fd.Height			= 45;
	fd.Width			= 28;
	fd.Weight			= FW_MEDIUM;
	fd.Italic			= false;
	fd.CharSet			= DEFAULT_CHARSET;
	fd.OutputPrecision  = OUT_DEFAULT_PRECIS;
	fd.PitchAndFamily   = FF_DONTCARE;
	//strcpy_s(fd.FaceName, "궁서체");	//글꼴 스타일
	AddFontResource("umberto.ttf");
	strcpy(fd.FaceName, "umberto");

	D3DXCreateFontIndirect(g_pD3DDevice, &fd, &m_pFont);

	m_pCamera = new cCamera;
	m_pCamera->Setup(NULL);

	m_pGrid = new cGrid;
	m_pGrid->Setup(15, 1.0f);
	
	SetLight();
}

void cMainGame::Update()
{
	if(m_pCamera && m_pPickedNode == NULL)
	{
		m_pCamera->Update();
	}

	m_pSkinnedMesh->Update();

	/*g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);*/
}

void cMainGame::Render()
{
	g_pD3DDevice->Clear(NULL,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(47, 121, 112),
		//D3DCOLOR_XRGB(0, 0, 255),
		1.0f, 0);

	g_pD3DDevice->BeginScene();

	// 그림을 그린다.
	m_pGrid->Render();

	for each(auto p in m_vecDijkstraNode)
	{
		p->Render(m_pSphere);
	}

	m_pSkinnedMesh->Render();

// 	D3DXMATRIXA16 matWorld;
// 	D3DXMatrixIdentity(&matWorld);
//  	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
// 	//m_pCube->Render();
// 
// 	g_pD3DDevice->SetMaterial(&m_stWhiteMtl);
// 	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
// 	//m_pSphere->DrawSubset(0);
// 	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
// 	if(m_isPicked)
// 		g_pD3DDevice->SetMaterial(&m_stRedMtl);
// 	else
// 		g_pD3DDevice->SetMaterial(&m_stWhiteMtl);
// 	m_pTeapot->DrawSubset(0);

// 	m_pRoot->Render();

// 	DWORD dwStart, dwFinish, dwGroup, dwMesh;
// 
// 	D3DXMatrixIdentity(&matWorld);
// 	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
// 
// 	dwStart = GetTickCount();
// 	for (int k = 0; k < 1000; ++k)
// 		m_pGroupHead->Render();
// 
// 	dwFinish = GetTickCount();
// 	dwGroup = dwFinish - dwStart;
// 	dwStart = dwFinish;
// 
// 	for (int k = 0; k < 1000; ++k)
// 		for (size_t i = 0; i < m_vecMtlTex.size(); ++i)
// 		{
// 			g_pD3DDevice->SetTexture(0, m_vecMtlTex[i]->GetTexture());
// 			g_pD3DDevice->SetMaterial(&m_vecMtlTex[i]->GetMaterial());
// 			m_pMesh->DrawSubset(i);
// 		}
// 	
// 	dwFinish = GetTickCount();
// 	dwMesh = dwFinish - dwStart;

	if(m_pFont)
	{
		RECT rc;
		SetRect(&rc, 100, 100, 101, 101);
		char szTemp[1024] = "ABC 123 ?!";
		//sprintf(szTemp, "%f", dwGroup / (float)dwMesh);
		m_pFont->DrawTextA(NULL,
			szTemp,
			strlen(szTemp),
			&rc,
			DT_LEFT | DT_TOP | DT_NOCLIP,
			D3DCOLOR_XRGB(255, 255, 0));
	}

	if(!m_vecEdge.empty())
	{
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
		g_pD3DDevice->SetTexture(0, NULL);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST,
			m_vecEdge.size() / 2, 
			&m_vecEdge[0],
			sizeof(ST_PC_VERTEX));

	}
	
	g_pD3DDevice->EndScene();
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void cMainGame::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if(m_pCamera)
		m_pCamera->WndProc(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_RBUTTONDOWN:
		{
			m_pStartNode = GetPickedNode(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_RBUTTONUP:
		{
			if(m_pStartNode)
			{
				m_pDestNode = GetPickedNode(LOWORD(lParam), HIWORD(lParam));
				if(m_pDestNode)
				{
					FindPath();
				}
			}
			m_pDestNode = NULL;
			m_pStartNode = NULL;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			ResetAllNode();
			
			m_pPickedNode = GetPickedNode(LOWORD(lParam), HIWORD(lParam));
			if(m_pPickedNode)
			{
				m_pPickedNode->SetState(cDijkstraNode::E_SELECTED);
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			cDijkstraNode* pNode = GetPickedNode(LOWORD(lParam), HIWORD(lParam));

			if(m_pPickedNode && pNode)
			{
				ST_PC_VERTEX v;
				v.c = D3DCOLOR_XRGB(255, 0 ,0);
				v.p = m_pPickedNode->GetPosition();
				m_vecEdge.push_back(v);

				v.p = pNode->GetPosition();
				m_vecEdge.push_back(v);

				D3DXVECTOR3 vDelta = m_pPickedNode->GetPosition() - pNode->GetPosition();
				float l = D3DXVec3Length(&vDelta);
				m_vecEdgeCost[m_pPickedNode->GetNodeId()][pNode->GetNodeId()] = l;
				m_vecEdgeCost[pNode->GetNodeId()][m_pPickedNode->GetNodeId()] = l;
			}

			m_pPickedNode = NULL;

			ResetAllNode();
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			cRay r = cRay::RayAtWorldSpace(x, y);
			for (size_t i = 0; i < m_vecDijkstraNode.size(); ++i)
			{
				if(m_vecDijkstraNode[i]->GetState() == cDijkstraNode::E_SELECTED)
					continue;

				if(m_vecDijkstraNode[i]->GetState() == cDijkstraNode::E_PATH)
					continue;

				if(r.IsPicked(m_vecDijkstraNode[i]->GetPosition(), m_vecDijkstraNode[i]->GetRadius()))
				{
					m_vecDijkstraNode[i]->SetState(cDijkstraNode::E_MOUSE_OVER);
				}
				else
				{
					m_vecDijkstraNode[i]->SetState(cDijkstraNode::E_NORMAL);
				}
			}
		}
		break;
	}
}

void cMainGame::SetLight()
{
	D3DLIGHT9 stLight;
	ZeroMemory(&stLight, sizeof(D3DLIGHT9));
	stLight.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	stLight.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	stLight.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	stLight.Type = D3DLIGHT_DIRECTIONAL;

	D3DXVECTOR3 vDir(0.7f, -1, 1);
	D3DXVec3Normalize(&vDir, &vDir);
	stLight.Direction = vDir;
	g_pD3DDevice->SetLight(0, &stLight);
	g_pD3DDevice->LightEnable(0, true);
	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	//g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
}

cDijkstraNode* cMainGame::GetPickedNode( int x, int y )
{
	cRay r = cRay::RayAtWorldSpace(x, y);
	for (size_t i = 0; i < m_vecDijkstraNode.size(); ++i)
	{
		if(r.IsPicked(m_vecDijkstraNode[i]->GetPosition(), 
			m_vecDijkstraNode[i]->GetRadius()))
		{
			return m_vecDijkstraNode[i];
		}
	}
	return NULL;
}

void cMainGame::ResetAllNode()
{
	for (size_t i = 0; i < m_vecDijkstraNode.size(); ++i)
	{
		m_vecDijkstraNode[i]->SetState(cDijkstraNode::E_NORMAL);
	}
}

void cMainGame::FindPath()
{
	m_pStartNode->SetCost(0.0f);

	while(true)
	{
		cDijkstraNode* pViaNode = GetMinCostNode();
		
		if(pViaNode == NULL)
			break;

		pViaNode->SetVisited(true);

		for (size_t i = 0; i < m_vecDijkstraNode.size(); ++i)
		{
			if(m_vecDijkstraNode[i]->GetVisited())
				continue;

			float fUpdateCost = pViaNode->GetCost() + m_vecEdgeCost[pViaNode->GetNodeId()][m_vecDijkstraNode[i]->GetNodeId()];
			if(m_vecDijkstraNode[i]->GetCost() > fUpdateCost)
			{
				m_vecDijkstraNode[i]->SetCost(fUpdateCost);
				m_vecDijkstraNode[i]->SetVia(pViaNode->GetNodeId());
			}
		}
	}
	
	int nIndex = m_pDestNode->GetNodeId();
	while(nIndex != m_pStartNode->GetNodeId())
	{
		if(nIndex < 0)
			break;
		m_vecDijkstraNode[nIndex]->SetState(cDijkstraNode::E_PATH);
		nIndex = m_vecDijkstraNode[nIndex]->GetVia();
	}
}

cDijkstraNode* cMainGame::GetMinCostNode()
{
	cDijkstraNode* pMinCostNode = NULL;
	float fMinCost = COST_INFINITY;

	for (size_t i = 0; i < m_vecDijkstraNode.size(); ++i)
	{
		if(m_vecDijkstraNode[i]->GetVisited())
			continue;

		if(m_vecDijkstraNode[i]->GetCost() < fMinCost)
		{
			fMinCost = m_vecDijkstraNode[i]->GetCost();
			pMinCostNode = m_vecDijkstraNode[i];
		}
	}
	return pMinCostNode;
}
