#include "StdAfx.h"
#include "cMtlTex.h"


cMtlTex::cMtlTex(void)
	: m_pTexture(NULL)
{
	ZeroMemory(&m_stMtl, sizeof(D3DMATERIAL9));
}

cMtlTex::~cMtlTex(void)
{
}

void cMtlTex::SetTexture( char* szFullPath )
{
	m_pTexture = g_pTextureManager->GetTexture(szFullPath);
}