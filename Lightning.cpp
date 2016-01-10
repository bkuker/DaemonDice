// Lightning.cpp: implementation of the Lightning class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lightning.h"
#include "DXSystem.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IDirect3DVertexBuffer8* Lightning::s_pvPoints = NULL;
int Lightning::refCount = 0;
LPDIRECT3DTEXTURE8 Lightning::pTex = NULL;

Lightning::Lightning(DXSystem* dxs):
Renderable(dxs),
time(0)
{
	D3DXMatrixScaling( &tm, 0.25f, 1, 1 );
	tm._32 = 0.25f; 
	refCount++;
	init();
	if (pTex == NULL)
		pTex = dxs->loadTexture("textures/lightning.dds");
	scale(1, 5, 1);
	translate(0, 2.0f, 0);
	rotateYawPitchRoll(-D3DX_PI / 4, 0, 0);
}

Lightning::~Lightning()
{
	refCount--;
	if (refCount == 0){
		dxs->releaseTexture(pTex);
		pTex = NULL;
		s_pvPoints->Release();
		s_pvPoints = NULL;
	}
}

void Lightning::init(){
	if (s_pvPoints == NULL){
		pd3dDevice->CreateVertexBuffer(4*sizeof(PNTVertex), 0 ,FVF_PNT, D3DPOOL_MANAGED, &s_pvPoints);
		PNTVertex* pV;
		s_pvPoints->Lock(0, 0, (BYTE**)&pV,0);
		
		for (int n = 0; n < 4; n++){
			pV[n].n = D3DXVECTOR3(0,0,-1);
		}
		
		pV[0].p = D3DXVECTOR3(-0.5f, -0.5f, 0);
		pV[1].p = D3DXVECTOR3(-0.5f,  0.5f, 0);
		pV[2].p = D3DXVECTOR3( 0.5f, -0.5f, 0);
		pV[3].p = D3DXVECTOR3( 0.5f,  0.5f, 0);
		
		
		pV[1].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[0].t = D3DXVECTOR2(0.0f, 1.0f);
		pV[3].t = D3DXVECTOR2(1.0f, 0.0f);
		pV[2].t = D3DXVECTOR2(1.0f, 1.0f);
		
		s_pvPoints->Unlock();
	}
}

#define __LCPS__ 1000/12
void Lightning::tick(int ms){
	time += ms;
	if (time >= __LCPS__){
		time = time - __LCPS__;
		tm._32 += 0.25f;
	}
}

void Lightning::draw(){
	pd3dDevice->SetMaterial( &mtrl );
	pd3dDevice->SetTexture( 0, pTex );


	pd3dDevice->SetTransform( D3DTS_TEXTURE0, &tm );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2  );

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetVertexShader(FVF_PNT);
	pd3dDevice->SetStreamSource(0, s_pvPoints, sizeof(PNTVertex));
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	pd3dDevice->SetTransform( D3DTS_TEXTURE0, &s_mIdent );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

}
