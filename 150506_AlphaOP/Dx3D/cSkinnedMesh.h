#pragma once

#include "cCharacter.h"

struct ST_BONE;

class cSkinnedMesh : public cCharacter
{
	friend class cSkinnedMeshManager;

private:
	//하나만 생성
	ST_BONE*					m_pRootFrame;			
	DWORD						m_dwWorkingPaletteSize;
	D3DXMATRIX*					m_pmWorkingPalette;
	LPD3DXEFFECT				m_pEffect;

	// 객체마다 생성
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;
	
	//SYNTHESIZE(D3DXVECTOR3, m_vPosition, Position);

public:
	cSkinnedMesh(char* szFolder, char* szFilename);
	~cSkinnedMesh(void);
	
	void Render();
	void SetAnimationIndex(int nIndex);

	void SetRandomTrackPosition(); // 테스트용
private:
	cSkinnedMesh();
	void Load(char* szFolder, char* szFilename);
	LPD3DXEFFECT LoadEffect(char* szFilename);
	void Update(ST_BONE* pCurrent, D3DXMATRIXA16* pmatParent);
	void Render(ST_BONE* pBone = NULL);
	void SetupBoneMatrixPtrs(ST_BONE* pBone);
	void Destroy();
};

