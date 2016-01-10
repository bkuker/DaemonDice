// Number.cpp: implementation of the Number class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Number.h"
#include "DXSystem.h"
#include "Context.h"

IDirect3DVertexBuffer8*	Number::polys = NULL;
LPDIRECT3DTEXTURE8 Number::pTex = NULL;
int Number::refCount = 0;
D3DXMATRIX Number::s_mR1;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Number::Number(DXSystem* dxs):
	Renderable(dxs)
{
	if ( polys == NULL )
		init();
	refCount++;
	digits.clear();
}

Number::~Number()
{
	refCount--;
	if (refCount == 0){
		polys->Release();
		dxs->releaseTexture(pTex);
	}
}


void Number::init()
{
	pTex = dxs->loadTexture("textures/numbers.dds");
	D3DXMatrixTranslation( &s_mR1,0.5f,0,0);
	pd3dDevice->CreateVertexBuffer( 12*4*sizeof(PNTVertex), 0, FVF_PNT, D3DPOOL_MANAGED, &polys);
	PNTVertex* pV;
	polys->Lock(0, 0, (BYTE**)&pV,0);
	for ( int i = 0; i < 12*4; i++){
		pV[i].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f);
	}

	float x, y;
	for ( i = 0; i < 12; i++ ){
		pV[4*i+0].p = D3DXVECTOR3(0,1,0);
		pV[4*i+1].p = D3DXVECTOR3(0,0,0);
		pV[4*i+2].p = D3DXVECTOR3(1,1,0);
		pV[4*i+3].p = D3DXVECTOR3(1,0,0);

		if ( i < 4)
			y = 0;
		else if ( i < 8 )
			y = .25;
		else
			y = .5;

		x = i % 4 * .25;

		pV[4*i+0].t = D3DXVECTOR2(x,y+0.25f);
		pV[4*i+2].t = D3DXVECTOR2(x+0.25f,y+0.25f);
		pV[4*i+1].t = D3DXVECTOR2(x,y);
		pV[4*i+3].t = D3DXVECTOR2(x+0.25f,y);
	}


	polys->Unlock();
}

void Number::setValue(int v)
{
	value = v;
	digits.clear();
	int dv = 1;
	while ( v != 0 ){
		int i = v % (10*dv);
		v = v - i;
		digits.push_back(i / dv);
		dv=dv*10;
	}
}

void Number::render(Context *c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	for (std::vector<int>::iterator i = digits.end() - 1; i != digits.begin() - 1; i--)
	{
		draw(*i);
		c->m_mspWorld->MultMatrixLocal( &s_mR1 );
		pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

	}
	c->m_mspWorld->Pop();
}

void Number::draw(int n)
{
	if ( n < 0 || n > 11)
		return;
	pd3dDevice->SetTexture( 0, pTex );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDevice->SetVertexShader(FVF_PNT);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pd3dDevice->SetStreamSource(0, polys, sizeof(PNTVertex));
	DXERROR(pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4*n, 2));
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}