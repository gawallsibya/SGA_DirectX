#pragma once

#define g_pTextureManager cTextureManager::GetInstance()

class cTextureManager
{
private:
	std::map<std::string, LPDIRECT3DTEXTURE9> m_mapTexture;

public:
	SINGLETONE(cTextureManager);
	
	LPDIRECT3DTEXTURE9 GetTexture(char* szFullPath);
	LPDIRECT3DTEXTURE9 GetTexture(std::string sFullPath);
	void Destroy();
};

