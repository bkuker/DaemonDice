// Renderable.cpp: implementation of the Renderable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renderable.h"
#include "DXSystem.h"
#include "Context.h"

D3DMATERIAL8 Renderable::mtrl;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Renderable::Renderable(DXSystem* _dxs):pd3dDevice(_dxs->pd3dDevice),dxs(_dxs){
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
}

Renderable::~Renderable()
{

}

void Renderable::render(Context *c){
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	draw();
	c->m_mspWorld->Pop();
}

void Renderable::onLostDevice(){

}

void Renderable::onResetDevice(){

}
