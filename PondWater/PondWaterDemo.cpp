//=============================================================================
// PondWaterDemo.cpp by Frank Luna (C) 2005 All Rights Reserved.
//
// Uses normal maps and reflective/refractive maps to render pond water.
//
// Controls: Use mouse to look and 'W', 'S', 'A', and 'D' keys to move.
//=============================================================================

#include "d3dApp.h"
#include "DirectInput.h"
#include <crtdbg.h>
#include "GfxStats.h"
#include <list>
#include "Camera.h"
#include "Sky.h"
#include "Vertex.h"
#include "PondWater.h"

class PondWaterDemo : public D3DApp
{
public:
	PondWaterDemo(HINSTANCE hInstance, tstring winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~PondWaterDemo();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void drawScene();
	
	void buildFX();

private:
	void drawSceneMesh(D3DXPLANE* reflectedW);

private:
	GfxStats* mGfxStats;
	 
	PondWater* mWater;
	Sky* mSky;
	ID3DXMesh* mSceneMesh;
	D3DXMATRIX mSceneWorld;
	D3DXMATRIX mSceneWorldInv;
	D3DXMATRIX mWaterWorld;
	std::vector<Mtrl> mSceneMtrls;
	std::vector<IDirect3DTexture9*> mSceneTextures;

	// Hack for this particular scene--usually you'd want to come up
	// with a more general method of loading normal maps such that
	// the ith normal map corresponds with the ith mesh subset.
	// For example, you might call each color texture name_color and 
	// its corresponding normal map name_nmap.  Then when you load the
	// name_color texture you also load the corresponding normal map.
	// If a texture doesn't have a normal map, you could use a default one
	// like we use the default white texture.
	IDirect3DTexture9* mSceneNormalMaps[2];

	IDirect3DTexture9* mWhiteTex;

	ID3DXEffect* mFX;
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhWVP;
	D3DXHANDLE   mhWorldInv;
	D3DXHANDLE   mhEyePosW;
	D3DXHANDLE   mhTex;
	D3DXHANDLE   mhMtrl;
	D3DXHANDLE   mhLight;
	D3DXHANDLE   mhNormalMap;

	DirLight mLight;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	// Construct camera before application, since the application uses the camera.
	Camera camera;
	gCamera = &camera;

	PondWaterDemo app(hInstance, _T("Pond Water Demo"), D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gd3dApp = &app;

	DirectInput di(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	gDInput = &di;

    return gd3dApp->run();
}

PondWaterDemo::PondWaterDemo(HINSTANCE hInstance,tstring winCaption, D3DDEVTYPE devType, DWORD requestedVP)
: D3DApp(hInstance, winCaption, devType, requestedVP)
{
	if(!checkDeviceCaps())
	{
		MessageBox(0, _T("checkDeviceCaps() Failed"), 0, 0);
		PostQuitMessage(0);
	}

	InitAllVertexDeclarations();

	mLight.dirW = D3DXVECTOR3(0.0f, -1.0f, -3.0f);
	D3DXVec3Normalize(&mLight.dirW, &mLight.dirW);
	mLight.ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mLight.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLight.spec    = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	mGfxStats = new GfxStats();
	mSky = new Sky(_T("grassenvmap1024.dds"), 10000.0f);

	D3DXMatrixTranslation(&mWaterWorld, 0.0f, 2.0f, 0.0f);

	Mtrl waterMtrl;
	waterMtrl.ambient   = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.00f);
	waterMtrl.diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.00f);
	waterMtrl.spec      = 0.6f*WHITE;
	waterMtrl.specPower = 200.0f;

	PondWater::InitInfo waterInitInfo;
	waterInitInfo.dirLight = mLight;
	waterInitInfo.mtrl     = waterMtrl;
	waterInitInfo.vertRows         = 128;
	waterInitInfo.vertCols         = 128;
	waterInitInfo.dx               = 1.0f;
	waterInitInfo.dz               = 1.0f;
	waterInitInfo.waveMapFilename0 = _T("wave0.dds");
	waterInitInfo.waveMapFilename1 = _T("wave1.dds");
	waterInitInfo.waveMapVelocity0 = D3DXVECTOR2(0.09f, 0.06f);
	waterInitInfo.waveMapVelocity1 = D3DXVECTOR2(-0.05f, 0.08f);
	waterInitInfo.texScale = 10.0f; 
	waterInitInfo.refractBias = 0.1f;
	waterInitInfo.refractPower = 2.0f;
	waterInitInfo.rippleScale  = D3DXVECTOR2(0.06f, 0.03f); 
	waterInitInfo.toWorld = mWaterWorld;
 
	mWater = new PondWater(waterInitInfo);

	ID3DXMesh* tempMesh = 0;
	LoadXFile(_T("BasicColumnScene.x"), &tempMesh, mSceneMtrls, mSceneTextures);

	// Get the vertex declaration for the NMapVertex.
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;
	HR(NMapVertex::Decl->GetDeclaration(elems, &numElems));

	// Clone the mesh to the NMapVertex format.
	ID3DXMesh* clonedTempMesh = 0;
	HR(tempMesh->CloneMesh(D3DXMESH_MANAGED, elems, gd3dDevice, &clonedTempMesh));

	// Now use D3DXComputeTangentFrameEx to build the TNB-basis for each vertex
	// in the mesh.  
	
	HR(D3DXComputeTangentFrameEx(
	  clonedTempMesh, // Input mesh
	  D3DDECLUSAGE_TEXCOORD, 0, // Vertex element of input tex-coords.  
      D3DDECLUSAGE_BINORMAL, 0, // Vertex element to output binormal.
	  D3DDECLUSAGE_TANGENT, 0,  // Vertex element to output tangent.
      D3DDECLUSAGE_NORMAL, 0,   // Vertex element to output normal.
      0, // Options
      0, // Adjacency
	  0.01f, 0.25f, 0.01f, // Thresholds for handling errors
	  &mSceneMesh, // Output mesh
	  0));         // Vertex Remapping

	// Done with temps.
	ReleaseCOM(tempMesh);
	ReleaseCOM(clonedTempMesh);

	D3DXMatrixIdentity(&mSceneWorld);
	D3DXMatrixIdentity(&mSceneWorldInv);

	HR(D3DXCreateTextureFromFile(gd3dDevice, _T("floor_nmap.bmp"), &mSceneNormalMaps[0]));
	HR(D3DXCreateTextureFromFile(gd3dDevice, _T("bricks_nmap.bmp"), &mSceneNormalMaps[1]));

	HR(D3DXCreateTextureFromFile(gd3dDevice, _T("whitetex.dds"), &mWhiteTex));

	// Initialize camera.
	gCamera->pos().y = 7.0f;
	gCamera->pos().z = -30.0f;
	gCamera->setSpeed(10.0f);
	

	mGfxStats->addVertices(mSceneMesh->GetNumVertices());
	mGfxStats->addTriangles(mSceneMesh->GetNumFaces());

	mGfxStats->addVertices(mWater->getNumVertices());
	mGfxStats->addTriangles(mWater->getNumTriangles());

	mGfxStats->addVertices(mSky->getNumVertices());
	mGfxStats->addTriangles(mSky->getNumTriangles());


	buildFX();

	onResetDevice();
}

PondWaterDemo::~PondWaterDemo()
{
	delete mGfxStats;
	delete mSky;
	delete mWater;
 
	ReleaseCOM(mFX);

	ReleaseCOM(mSceneMesh);
	for(UINT i = 0; i < mSceneTextures.size(); ++i)
		ReleaseCOM(mSceneTextures[i]);

	ReleaseCOM(mWhiteTex);
	ReleaseCOM(mSceneNormalMaps[0]);
	ReleaseCOM(mSceneNormalMaps[1]);

	DestroyAllVertexDeclarations();
}

bool PondWaterDemo::checkDeviceCaps()
{
	D3DCAPS9 caps;
	HR(gd3dDevice->GetDeviceCaps(&caps));

	// Check for vertex shader version 2.0 support.
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
		return false;

	// Check for pixel shader version 2.0 support.
	if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
		return false;

	return true;
}

void PondWaterDemo::onLostDevice()
{
	mGfxStats->onLostDevice();
	mSky->onLostDevice();
	mWater->onLostDevice();
	HR(mFX->OnLostDevice());
}

void PondWaterDemo::onResetDevice()
{
	mGfxStats->onResetDevice();
	mSky->onResetDevice();
	mWater->onResetDevice();
	HR(mFX->OnResetDevice());

	// The aspect ratio depends on the backbuffer dimensions, which can 
	// possibly change after a reset.  So rebuild the projection matrix.
	float w = (float)md3dPP.BackBufferWidth;
	float h = (float)md3dPP.BackBufferHeight;
	gCamera->setLens(D3DX_PI * 0.25f, w/h, 1.0f, 15000.0f);
}

void PondWaterDemo::updateScene(float dt)
{
	mGfxStats->update(dt);

	gDInput->poll();

	gCamera->update(dt, 0, 0);

	mWater->update(dt);
}

void PondWaterDemo::drawScene()
{
	// Reflection plane in local space.
	D3DXPLANE waterPlaneL(0.0f, -1.0f, 0.0f, 0.0f);

	// Reflection plane in world space.
	D3DXMATRIX WInvTrans;
	D3DXMatrixInverse(&WInvTrans, 0, &(mWaterWorld));
	D3DXMatrixTranspose(&WInvTrans, &WInvTrans);
	D3DXPLANE waterPlaneW;
	D3DXPlaneTransform(&waterPlaneW, &waterPlaneL, &WInvTrans);

	// Reflection plane in homogeneous clip space.
	D3DXMATRIX WVPInvTrans;
	D3DXMatrixInverse(&WVPInvTrans, 0, &(mWaterWorld*gCamera->viewProj()));
	D3DXMatrixTranspose(&WVPInvTrans, &WVPInvTrans);
	D3DXPLANE waterPlaneH;
	D3DXPlaneTransform(&waterPlaneH, &waterPlaneL, &WVPInvTrans);
 

	DrawableTex2D* reflectMap = mWater->mReflectMap;
	DrawableTex2D* refractMap = mWater->mRefractMap;

	
	float f[4] = {waterPlaneH.a, waterPlaneH.b, waterPlaneH.c, waterPlaneH.d};
	HR(gd3dDevice->SetClipPlane(0, (float*)f));
	HR(gd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 1));
	refractMap->beginScene();
	mSky->draw(0);
	drawSceneMesh(0);
	refractMap->endScene();

	// Seems like we need to reset these due to a driver bug.  It works
	// correctly without these next two lines in the REF and another 
	// video card, however.
	HR(gd3dDevice->SetClipPlane(0, (float*)f));
	HR(gd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 1));

	HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
	reflectMap->beginScene();
	mSky->draw(&waterPlaneW);
	drawSceneMesh(&waterPlaneW);
	reflectMap->endScene();

	HR(gd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0));
	HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));

	HR(gd3dDevice->BeginScene());

	mSky->draw(0);
	drawSceneMesh(0);
	mWater->draw();

	mGfxStats->display();

	HR(gd3dDevice->EndScene());

	// Present the backbuffer.
	HR(gd3dDevice->Present(0, 0, 0, 0));
}

void PondWaterDemo::drawSceneMesh(D3DXPLANE* reflectedW)
{
	D3DXMATRIX RefMtx;
	D3DXMatrixIdentity(&RefMtx);
	if(reflectedW)
		D3DXMatrixReflect(&RefMtx, reflectedW);


	HR(mFX->SetValue(mhLight, &mLight, sizeof(DirLight)));
	HR(mFX->SetMatrix(mhWVP, &(mSceneWorld*RefMtx*gCamera->viewProj())));
	HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3)));
	
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));

	for(UINT j = 0; j < mSceneMtrls.size(); ++j)
	{
		HR(mFX->SetValue(mhMtrl, &mSceneMtrls[j], sizeof(Mtrl)));
	
		// If there is a texture, then use.
		if(mSceneTextures[j] != 0)
		{
			HR(mFX->SetTexture(mhTex, mSceneTextures[j]));
		}

		// But if not, then set a pure white texture.  When the texture color
		// is multiplied by the color from lighting, it is like multiplying by
		// 1 and won't change the color from lighting.
		else
		{
			HR(mFX->SetTexture(mhTex, mWhiteTex));
		}
	
		HR(mFX->SetTexture(mhNormalMap, mSceneNormalMaps[j]));

		HR(mFX->CommitChanges());
		HR(mSceneMesh->DrawSubset(j));
	}
	HR(mFX->EndPass());
	HR(mFX->End());
}

void PondWaterDemo::buildFX()
{
	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, _T("NormalMap.fx"), 
		0, 0, 0, 0, &mFX, &errors));
	if( errors )
		MessageBox(0, (TCHAR*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhTech       = mFX->GetTechniqueByName("NormalMapTech");
	mhWVP        = mFX->GetParameterByName(0, "gWVP");
	mhWorldInv   = mFX->GetParameterByName(0, "gWorldInv");
	mhMtrl       = mFX->GetParameterByName(0, "gMtrl");
	mhLight      = mFX->GetParameterByName(0, "gLight");
	mhEyePosW    = mFX->GetParameterByName(0, "gEyePosW");
	mhTex        = mFX->GetParameterByName(0, "gTex");
	mhNormalMap  = mFX->GetParameterByName(0, "gNormalMap");

	// Set parameters that do not vary:

	// World is the identity, so inverse is also identity.
	HR(mFX->SetMatrix(mhWorldInv, &mSceneWorldInv));
	HR(mFX->SetTechnique(mhTech));
}
