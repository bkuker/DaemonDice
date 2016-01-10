// Die.cpp: implementation of the Die class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Die.h"
#include "DXSystem.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IDirect3DVertexBuffer8* Die::s_pvPoints = NULL;
int Die::refCount = 0;
D3DXMATRIX Die::s_mShrink;
	
Die::Die(DXSystem* dxs):
	Renderable(dxs),
	pTex1(dxs->loadTexture("textures/die.dds")),
	pTex2(dxs->loadTexture("textures/die2.dds")),
	pTex(pTex1)
{
	rise(0);
	refCount++;
	init();
}

	Die::~Die()
{
	refCount--;
	if (refCount == 0){
		s_pvPoints->Release();
		s_pvPoints = NULL;
	}
	if (pTex1 != NULL )
		dxs->releaseTexture( pTex1 );
	if (pTex2 != NULL )
		dxs->releaseTexture( pTex2 );
}

void Die::roll(CELL_DIR d, float f){
	pTex=pTex1;
	D3DXMatrixIdentity( &m_mOrient );
	if ( f > 1.0f )
		f = 1.0f;
	if ( f < 0 )
		f = 0;
	f = D3DX_PI /  2.0f * f;
	switch (d){
	case NORTH:
		D3DXMatrixTranslation( &s_mDiscard, 0, 0.5f, -0.5f );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, f, 0);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixTranslation( &s_mDiscard, 0, -0.5f, 0.5f );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case SOUTH:
		D3DXMatrixTranslation( &s_mDiscard, 0, 0.5f, 0.5f );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, -f, 0);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixTranslation( &s_mDiscard, 0, -0.5f, -0.5f );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case EAST:
		D3DXMatrixTranslation( &s_mDiscard, -0.5f, 0.5f, 0 );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, 0, -f);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixTranslation( &s_mDiscard, 0.5f, -0.5f, 0 );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case WEST:
		D3DXMatrixTranslation( &s_mDiscard, 0.5f, 0.5f, 0 );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, 0, f);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		D3DXMatrixTranslation( &s_mDiscard, -0.5f, -0.5f, 0 );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;
	}
}

void Die::infoRoll(CELL_DIR d, float f){
	pTex=pTex1;
	D3DXMatrixIdentity( &m_mOrient );
	if ( f > 1.0f )
		f = 1.0f;
	if ( f < 0 )
		f = 0;
	f = D3DX_PI /  2.0f * f;
	switch (d){
	case NORTH:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, f, 0);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case SOUTH:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, -f, 0);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case EAST:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, 0, -f);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case WEST:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, 0, f);
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;
	}
}

void Die::slide(CELL_DIR d, float f){
	pTex=pTex1;
	D3DXMatrixIdentity( &m_mOrient );
	if ( f > 1.0f )
		f = 1.0f;
	if ( f < 0 )
		f = 0;
	switch (d){
	case NORTH:
		D3DXMatrixTranslation( &s_mDiscard, 0, 0, f );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case SOUTH:
		D3DXMatrixTranslation( &s_mDiscard, 0, 0, -f );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case EAST:
		D3DXMatrixTranslation( &s_mDiscard, f, 0, 0 );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;	
	case WEST:
		D3DXMatrixTranslation( &s_mDiscard, -f, 0, 0 );
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mDiscard );
		break;
	}
}

void Die::rise(float f){
	D3DXMatrixIdentity( &m_mOrient );
	if ( f > 1.0f )
		f = 1.0f;
	if ( f < 0 )
		f = 0;
	D3DXMatrixTranslation( &m_mOrient, 0, -1.0f + f, 0 );

	if (f <= 0.5f){
		pTex=pTex2;
		D3DXMatrixMultiply( &m_mOrient, &m_mOrient, &s_mShrink );
	}
	else
		pTex=pTex1;
}

void Die::rotate(CELL_DIR d){
	switch (d){
	case NORTH:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, D3DX_PI /  2.0f, 0);
		D3DXMatrixMultiply( &m_mDist, &m_mDist, &s_mDiscard );
		break;	
	case SOUTH:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, -D3DX_PI /  2.0f, 0);
		D3DXMatrixMultiply( &m_mDist, &m_mDist, &s_mDiscard );
		break;	
	case EAST:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, 0, -D3DX_PI /  2.0f);
		D3DXMatrixMultiply( &m_mDist, &m_mDist, &s_mDiscard );
		break;	
	case WEST:
		D3DXMatrixRotationYawPitchRoll(&s_mDiscard, 0, 0, D3DX_PI /  2.0f);
		D3DXMatrixMultiply( &m_mDist, &m_mDist, &s_mDiscard );
		break;
	case NONE:
		break;
	}
}

#define SHRINK 0.95f
void Die::init(){
	if (s_pvPoints == NULL){
		dxs->debugprintf("Initializing Dice Geometry");

		D3DXMatrixScaling( &s_mShrink, SHRINK, SHRINK, SHRINK  );

		pd3dDevice->CreateVertexBuffer(26*sizeof(PNTVertex), 0 ,FVF_PNT, D3DPOOL_MANAGED, &s_pvPoints);
		PNTVertex* pV;
		s_pvPoints->Lock(0, 0, (BYTE**)&pV,0);


		pV[0].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pV[1].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pV[2].p = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pV[3].p = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		pV[0].n = D3DXVECTOR3(0, 0, -1);
		pV[2].n = D3DXVECTOR3(0, 0, -1);
		pV[3].n = D3DXVECTOR3(0, 0, -1);
		pV[4].n = D3DXVECTOR3(0, 0, -1);


		pV[4].p = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		pV[5].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
		pV[6].p = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pV[7].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[4].n = D3DXVECTOR3(1,0,0);
		pV[5].n = D3DXVECTOR3(1,0,0);
		pV[6].n = D3DXVECTOR3(1,0,0);
		pV[7].n = D3DXVECTOR3(1,0,0);

		pV[8].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[9].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
		pV[10].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		pV[11].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
		pV[8].n = D3DXVECTOR3(0,0,1);
		pV[9].n = D3DXVECTOR3(0,0,1);
		pV[10].n = D3DXVECTOR3(0,0,1);
		pV[11].n = D3DXVECTOR3(0,0,1);

		pV[12].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
		pV[13].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pV[14].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		pV[15].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pV[12].n = D3DXVECTOR3(-1,0,0);
		pV[13].n = D3DXVECTOR3(-1,0,0);
		pV[14].n = D3DXVECTOR3(-1,0,0);
		pV[15].n = D3DXVECTOR3(-1,0,0);
		
		pV[16].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pV[17].p = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pV[18].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		pV[19].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[16].n = D3DXVECTOR3(0,-1,0);
		pV[17].n = D3DXVECTOR3(0,-1,0);
		pV[18].n = D3DXVECTOR3(0,-1,0);
		pV[19].n = D3DXVECTOR3(0,-1,0);

		pV[20].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[21].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);

		pV[22].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
		pV[23].p = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		pV[24].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);	
		pV[25].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pV[22].n = D3DXVECTOR3(0,1,0);
		pV[23].n = D3DXVECTOR3(0,1,0);
		pV[24].n = D3DXVECTOR3(0,1,0);
		pV[25].n = D3DXVECTOR3(0,1,0);

		//ONE
		pV[1].t = D3DXVECTOR2(0.0f, 0.25f);
		pV[0].t = D3DXVECTOR2(0.0f, 0.5f);
		pV[3].t = D3DXVECTOR2(0.25f, 0.25f);
		pV[2].t = D3DXVECTOR2(0.25f, 0.5f);
		//FIVE
		pV[4].t = D3DXVECTOR2(0.25f, 0.0f);
		pV[6].t = D3DXVECTOR2(0.25f, 0.25f);
		pV[5].t = D3DXVECTOR2(0.5f, 0.0f);
		pV[7].t = D3DXVECTOR2(0.5f, 0.25f);
		//SIX
		pV[9].t = D3DXVECTOR2(0.5f, 0.25f);
		pV[8].t = D3DXVECTOR2(0.5f, 0.5f);
		pV[11].t = D3DXVECTOR2(0.75f, 0.25f);
		pV[10].t = D3DXVECTOR2(0.75f, 0.5f);
		//TWO
		pV[12].t = D3DXVECTOR2(0.25f, 0.5f);
		pV[14].t = D3DXVECTOR2(0.5f, 0.5f);
		pV[13].t = D3DXVECTOR2(0.25f, 0.75f);
		pV[15].t = D3DXVECTOR2(0.5f, 0.75f);

		//THREE
		pV[16].t = D3DXVECTOR2(0.25f, 0.25f);
		pV[18].t = D3DXVECTOR2(0.25f, 0.5f);
		pV[17].t = D3DXVECTOR2(0.5f, 0.25f);
		pV[19].t = D3DXVECTOR2(0.5f, 0.5f);

		//FOUR
		pV[24].t = D3DXVECTOR2(0.75f, 0.25f);
		pV[25].t = D3DXVECTOR2(0.75f, 0.5f);
		pV[22].t = D3DXVECTOR2(1.0f, 0.25f);
		pV[23].t = D3DXVECTOR2(1.0f, 0.5f);


		s_pvPoints->Unlock();
	}
}

void Die::draw(){
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTexture( 0, pTex );
	pd3dDevice->SetVertexShader(FVF_PNT);
//	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetStreamSource(0, s_pvPoints, sizeof(PNTVertex));
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 24);
}


