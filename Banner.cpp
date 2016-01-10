// Banner.cpp: implementation of the Banner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Banner.h"
#include "DXSystem.h"

IDirect3DVertexBuffer8 *Banner::polys = NULL;
int Banner::refCount = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Banner::Banner(DXSystem* dxs, std::string texFile):
	Renderable(dxs)
{
	D3DXMatrixIdentity( &tm );
	pTex = dxs->loadTexture(texFile);
	refCount++;
	if (refCount == 1)
		init();
}

Banner::~Banner()
{
	refCount--;
	dxs->releaseTexture(pTex);
	if (refCount == 0)
	{
		polys->Release();
	}
		
}

void Banner::setArea(float x1, float y1, float x2, float y2)
{
	D3DXMatrixScaling( &tm, x2-x1, y2-y1, 1 );
	tm._31 = x1;
	tm._32 = y1;
}

void Banner::init()
{
	pd3dDevice->CreateVertexBuffer( 4*sizeof(PNTVertex), 0, FVF_PNT, D3DPOOL_MANAGED, &polys);
	PNTVertex* pV;
	polys->Lock(0, 0, (BYTE**)&pV,0);
	for ( int i = 0; i < 4; i++){
		pV[i].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f);
	}


	pV[0].p = D3DXVECTOR3(0,1,0);
	pV[1].p = D3DXVECTOR3(0,0,0);
	pV[2].p = D3DXVECTOR3(1,1,0);
	pV[3].p = D3DXVECTOR3(1,0,0);

	pV[0].t = D3DXVECTOR2(0,1);
	pV[1].t = D3DXVECTOR2(0,0);
	pV[2].t = D3DXVECTOR2(1,1);
	pV[3].t = D3DXVECTOR2(1,0);
	polys->Unlock();
}

void Banner::draw()
{
		pd3dDevice->SetTexture( 0, pTex );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pd3dDevice->SetVertexShader(FVF_PNT);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		pd3dDevice->SetTransform( D3DTS_TEXTURE0, &tm );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2  );

		pd3dDevice->SetStreamSource(0, polys, sizeof(PNTVertex));
		DXERROR(pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		pd3dDevice->SetTransform( D3DTS_TEXTURE0, &s_mIdent );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
}