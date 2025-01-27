#include "StdAfx.h"
#include "cRay.h"


cRay::cRay(void)
	: m_eRaySpace(E_NONE)
	, m_vOrg(0, 0, 0)
	, m_vDir(0, 0, 0)
{
}


cRay::~cRay(void)
{
}

cRay cRay::RayAtViewSpace( int x, int y )
{
	D3DVIEWPORT9 vp;
	g_pD3DDevice->GetViewport(&vp);

	D3DXMATRIXA16 matProj;
	g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	cRay r;
	r.m_vDir.x = (( 2.0f * x) / vp.Width  - 1.0f) / matProj._11;
	r.m_vDir.y = ((-2.0f * y) / vp.Height + 1.0f) / matProj._22;
	r.m_vDir.z = 1.0f;

	r.m_eRaySpace = E_VIEW;

	D3DXVec3Normalize(&r.m_vDir, &r.m_vDir);

	return r;
}

cRay cRay::RayAtWorldSpace( int x, int y )
{
	D3DXMATRIXA16 matView, matInvView;
	g_pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matInvView, 0, &matView);

	cRay r = cRay::RayAtViewSpace(x, y);
	
	D3DXVec3Normalize(&r.m_vDir, &r.m_vDir);

	r.m_eRaySpace = E_WORLD;

	return r.Transform(matInvView);
}

cRay cRay::Transform( D3DXMATRIXA16& mat )
{
	cRay r = (*this);
	D3DXVec3TransformNormal(&r.m_vDir, &r.m_vDir, &mat);
	D3DXVec3TransformCoord(&r.m_vOrg, &r.m_vOrg, &mat);
	D3DXVec3Normalize(&r.m_vDir, &r.m_vDir);
	return r;
}

bool cRay::IsPicked( D3DXVECTOR3& vSphereCenter, float fSphereRadius )
{
	assert(m_eRaySpace == E_WORLD);

	D3DXMATRIXA16 matInvWorld;
	D3DXMatrixTranslation(&matInvWorld, -vSphereCenter.x, -vSphereCenter.y, -vSphereCenter.z);
	cRay r = Transform(matInvWorld);

	// (V.Q)^2 - V.V * (Q.Q - r.r) >= 0
	float vv = D3DXVec3Dot(&r.m_vDir, &r.m_vDir);
	float qq = D3DXVec3Dot(&r.m_vOrg, &r.m_vOrg);
	float vq = D3DXVec3Dot(&r.m_vOrg, &r.m_vDir);
	float rr = fSphereRadius * fSphereRadius;

	return (vq * vq - vv * (qq - rr)) >= 0;
}
