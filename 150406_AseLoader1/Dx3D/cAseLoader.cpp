#include "StdAfx.h"
#include "cAseLoader.h"
#include "Asciitok.h"
#include "cMtlTex.h"
#include "cFrame.h"

cAseLoader::cAseLoader(void)
	: m_fp(NULL)
	, m_pRoot(NULL)
{
}


cAseLoader::~cAseLoader(void)
{
}

cFrame* cAseLoader::Load( char* szFullPath )
{
	fopen_s(&m_fp, szFullPath, "r");
	while(char* szToken = GetToken())
	{
		if(IsEqual(szToken, ID_SCENE))
		{
			SkipBlock();
		}
		else if(IsEqual(szToken, ID_MATERIAL_LIST))
		{
			ProcessMATERIAL_LIST();
		}
		else if(IsEqual(szToken, ID_GEOMETRY))
		{
			cFrame* pFrame = ProcessGEOMOBJECT();
			m_mapFrame[pFrame->GetFrameName()] = pFrame;
			if(m_pRoot == NULL)
				m_pRoot = pFrame;
		}
	}
		
	fclose(m_fp);

	return m_pRoot;
}

char* cAseLoader::GetToken()
{
	static char szToken[1024];

	int nCnt = 0;
	bool isQuote = false;

	while(true)
	{
		char c = fgetc(m_fp);
		
		if(feof(m_fp))
			break;

		if(c == '\"')
		{
			if(isQuote)
				break;

			isQuote = true;
			continue;
		}

		if(!isQuote && c < 33)
		{
			if(nCnt == 0)
				continue;
			else
				break;
		}

		szToken[nCnt++] = c;
		
	}

	if(nCnt == 0)
		return NULL;

	szToken[nCnt++] = '\0';

	return szToken;
}

int cAseLoader::GetInteger()
{
	return atoi(GetToken());
}

float cAseLoader::GetFloat()
{
	return (float)atof(GetToken());
}

bool cAseLoader::IsEqual( char* str1, char* str2 )
{
	return strcmp(str1, str2) == 0;
}

void cAseLoader::SkipBlock()
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMATERIAL_LIST()
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MATERIAL_COUNT))
		{
			m_vecMtlTex.resize(GetInteger());
		}
		else if(IsEqual(szToken, ID_MATERIAL))
		{
			int nMtlIndex = GetInteger();
			m_vecMtlTex[nMtlIndex] = new cMtlTex;
			ProcessMATERIAL(m_vecMtlTex[nMtlIndex]);
		}

	} while (nLevel > 0);
}

void cAseLoader::ProcessMATERIAL(cMtlTex* pMtlTex)
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_AMBIENT))
		{
			pMtlTex->m_stMtl.Ambient.r = GetFloat();
			pMtlTex->m_stMtl.Ambient.g = GetFloat();
			pMtlTex->m_stMtl.Ambient.b = GetFloat();
			pMtlTex->m_stMtl.Ambient.a = 1.0f;
		}
		else if(IsEqual(szToken, ID_DIFFUSE))
		{
			pMtlTex->m_stMtl.Diffuse.r = GetFloat();
			pMtlTex->m_stMtl.Diffuse.g = GetFloat();
			pMtlTex->m_stMtl.Diffuse.b = GetFloat();
			pMtlTex->m_stMtl.Diffuse.a = 1.0f;
		}
		else if(IsEqual(szToken, ID_SPECULAR))
		{
			pMtlTex->m_stMtl.Specular.r = GetFloat();
			pMtlTex->m_stMtl.Specular.g = GetFloat();
			pMtlTex->m_stMtl.Specular.b = GetFloat();
			pMtlTex->m_stMtl.Specular.a = 1.0f;
		}
		else if(IsEqual(szToken, ID_MAP_DIFFUSE))
		{
			ProcessMAP_DIFFUSE(pMtlTex);
		}

	} while (nLevel > 0);
}

void cAseLoader::ProcessMAP_DIFFUSE( cMtlTex* pMtlTex )
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_BITMAP))
		{
			char* szFullPath = GetToken();
			pMtlTex->SetTexture(szFullPath);
		}

	} while (nLevel > 0);
}

cFrame* cAseLoader::ProcessGEOMOBJECT()
{
	cFrame* pFrame = new cFrame;

	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_NODE_NAME))
		{
			pFrame->SetFrameName(GetToken());
		}
		else if(IsEqual(szToken, ID_NODE_PARENT))
		{
			std::string sParentName(GetToken());
			m_mapFrame[sParentName]->AddChild(pFrame);
		}
		else if(IsEqual(szToken, ID_NODE_TM))
		{
			SkipBlock();
		}
		else if(IsEqual(szToken, ID_MESH))
		{
			ProcessMESH(pFrame);
		}
		else if(IsEqual(szToken, ID_TM_ANIMATION))
		{
			SkipBlock();
		}
		else if(IsEqual(szToken, ID_MATERIAL_REF))
		{
			pFrame->SetMtlTex(m_vecMtlTex[GetInteger()]);
		}

	} while (nLevel > 0);

	return pFrame;
}

void cAseLoader::ProcessMESH( cFrame* pFrame )
{
	std::vector<ST_PNT_VERTEX>	vecVertex;
	std::vector<D3DXVECTOR3>	vecV;
	std::vector<D3DXVECTOR2>	vecVT;
	//std::vector<D3DXVECTOR3>	vecVN;

	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MESH_NUMVERTEX))
		{
			vecV.resize(GetInteger());
		}
		else if(IsEqual(szToken, ID_MESH_NUMFACES))
		{
			vecVertex.resize(GetInteger() * 3);
		}
		else if(IsEqual(szToken, ID_MESH_VERTEX_LIST))
		{
			ProcessMESH_VERTEX_LIST(vecV);
		}
		else if(IsEqual(szToken, ID_MESH_FACE_LIST))
		{
			ProcessMESH_FACE_LIST(vecV, vecVertex);
		}
		else if(IsEqual(szToken, ID_MESH_NUMTVERTEX))
		{
			vecVT.resize(GetInteger());
		}
		else if(IsEqual(szToken, ID_MESH_TVERTLIST))
		{
			ProcessMESH_TVERTLIST(vecVT);
		}
		else if(IsEqual(szToken, ID_MESH_NUMTVFACES))
		{
			GetToken(); // ����
		}
		else if(IsEqual(szToken, ID_MESH_TFACELIST))
		{
			ProcessMESH_TFACELIST(vecVT, vecVertex);
		}
		else if(IsEqual(szToken, ID_MESH_NORMALS))
		{
			ProcessMESH_NORMALS(vecVertex);
		}
	} while (nLevel > 0);

	pFrame->BuildVB(vecVertex);
}

void cAseLoader::ProcessMESH_VERTEX_LIST( OUT std::vector<D3DXVECTOR3>& vecV )
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MESH_VERTEX))
		{
			int nIndex = GetInteger();
			vecV[nIndex].x = GetFloat();
			vecV[nIndex].z = GetFloat();
			vecV[nIndex].y = GetFloat();
		}

	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_FACE_LIST( IN std::vector<D3DXVECTOR3>& vecV,
	OUT std::vector<ST_PNT_VERTEX>& vecVertex )
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MESH_FACE))
		{
			int nFaceIndex = GetInteger();
			GetToken(); // A:
			int nA = GetInteger();
			GetToken(); // B:
			int nB = GetInteger();
			GetToken(); // C:
			int nC = GetInteger();

			vecVertex[nFaceIndex * 3 + 0].p = vecV[nA];
			vecVertex[nFaceIndex * 3 + 1].p = vecV[nC];
			vecVertex[nFaceIndex * 3 + 2].p = vecV[nB];
		}

	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_TVERTLIST( OUT std::vector<D3DXVECTOR2>& vecVT )
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MESH_TVERT))
		{
			int nIndex = GetInteger();
			vecVT[nIndex].x = GetFloat();
			vecVT[nIndex].y = 1.0f - GetFloat();
		}

	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_TFACELIST( IN std::vector<D3DXVECTOR2>& vecVT, 
	OUT std::vector<ST_PNT_VERTEX>& vecVertex )
{
	int nLevel = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MESH_TFACE))
		{
			int nFaceIndex = GetInteger();
			int nA = GetInteger();
			int nB = GetInteger();
			int nC = GetInteger();

			vecVertex[nFaceIndex * 3 + 0].t = vecVT[nA];
			vecVertex[nFaceIndex * 3 + 1].t = vecVT[nC];
			vecVertex[nFaceIndex * 3 + 2].t = vecVT[nB];
		}

	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_NORMALS( OUT std::vector<ST_PNT_VERTEX>& vecVertex )
{
	int nFaceIndex = 0;

	int nLevel = 0;

	int nModIndex[] = {0, 2, 1};

	int nCnt = 0;

	do 
	{
		char* szToken = GetToken();
		if(IsEqual(szToken, "{"))
		{
			++nLevel;
		}
		else if(IsEqual(szToken, "}"))
		{
			--nLevel;
		}
		else if(IsEqual(szToken, ID_MESH_FACENORMAL))
		{
			nFaceIndex = GetInteger();
			nCnt = 0;
		}
		else if(IsEqual(szToken, ID_MESH_VERTEXNORMAL))
		{
			GetToken();
			float x = GetFloat();
			float y = GetFloat();
			float z = GetFloat();

			vecVertex[nFaceIndex * 3 + nModIndex[nCnt]].n = D3DXVECTOR3(x, z, y);
			++nCnt;
		}

	} while (nLevel > 0);
}
