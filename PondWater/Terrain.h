//=============================================================================
// Terrain.h by Frank Luna (C) 2004 All Rights Reserved.
//
// Assumptions: In this book, we only use one terrain per demo and we 
//              construct the terrain geometry directly in world space.
//=============================================================================

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Heightmap.h"
#include "d3dUtil.h"
#include "Vertex.h"

 
class Terrain
{
public:
	Terrain(UINT vertRows, UINT vertCols, float dx, float dz, 
		tstring heightmap, tstring tex0, tstring tex1, 
		tstring tex2, tstring blendMap, float heightScale, 
		float yOffset);
	~Terrain();

	DWORD getNumTriangles();
	DWORD getNumVertices();

	float getWidth();
	float getDepth();

	void onLostDevice();
	void onResetDevice();

	// (x, z) relative to terrain's local space.
	float getHeight(float x, float z);
	
	void setDirToSunW(const D3DXVECTOR3& d);

	void draw();

private:
	void buildGeometry();
	void buildSubGridMesh(RECT& R, VertexPNT* gridVerts); 
	void buildEffect();

	struct SubGrid
	{
		ID3DXMesh* mesh;
		AABB box;

		// For sorting.
		bool operator<(const SubGrid& rhs)const;

		const static int NUM_ROWS  = 33;
		const static int NUM_COLS  = 33;
		const static int NUM_TRIS  = (NUM_ROWS-1)*(NUM_COLS-1)*2;
		const static int NUM_VERTS = NUM_ROWS*NUM_COLS;
	};
private:
	Heightmap mHeightmap;
	std::vector<SubGrid> mSubGrids;

	DWORD mVertRows;
	DWORD mVertCols;

	float mWidth;
	float mDepth;

	float mDX;
	float mDZ;

	IDirect3DTexture9* mTex0;
	IDirect3DTexture9* mTex1;
	IDirect3DTexture9* mTex2;
	IDirect3DTexture9* mBlendMap;
	ID3DXEffect*       mFX;
	D3DXHANDLE         mhTech;
	D3DXHANDLE         mhViewProj;
	D3DXHANDLE         mhDirToSunW;
	D3DXHANDLE         mhTex0;
	D3DXHANDLE         mhTex1;
	D3DXHANDLE         mhTex2;
	D3DXHANDLE         mhBlendMap;
};

#endif // TERRAIN_H