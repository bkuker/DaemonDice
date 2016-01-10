// Info.cpp: implementation of the Info class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Info.h"
#include "DXSystem.h"
#include "Player.h"
#include "Cell.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Info::Info(DXSystem* dxs, Player* player):
	Banner(dxs, "textures/info.dds"),
	m_Player(player)
{


	D3DXMatrixLookAtLH(&s_mDieView, 
		&D3DXVECTOR3( 4,3,-4),
		&D3DXVECTOR3( 0,0,0),
		&D3DXVECTOR3( 0,-1,0));
	D3DXMatrixScaling( &s_mDiscard, 1.2f, 1.2f, 1 );
	D3DXMatrixMultiply( &s_mDieView, &s_mDieView, &s_mDiscard );

	D3DXMatrixPerspectiveFovLH( &s_mDiscard, D3DX_PI/4, -1.4f, 0.1f, 1000.0f );
	D3DXMatrixMultiply( &s_mDieView, &s_mDieView, &s_mDiscard );

	D3DXMatrixTranslation( &s_mDiscard, 0.5f, 0.5f, 0 );
	D3DXMatrixMultiply( &s_mDieView, &s_mDieView, &s_mDiscard );


}

Info::~Info()
{

}

void Info::render(Context* c)
{
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	if (	m_Player != NULL && 
			m_Player->m_pCell != NULL )
	{
		c->m_mspWorld->Push();
		c->m_mspWorld->MultMatrixLocal( &s_mDieView );
		if ( m_Player->m_pCell->state == ROLLOUT)
		{
			Cell* cell = m_Player->m_pCell;//->n(m_Player->m_pCell->in);
			Die* d = cell->die;
			d->infoRoll(
				cell->out,
				cell->time);
			d->render(c);
		} 
		else if (  m_Player->m_pCell->die != NULL && m_Player->up )
		{
			m_Player->m_pCell->die->rise(1);
			m_Player->m_pCell->die->render(c);
		}
		c->m_mspWorld->Pop();
	} 
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	draw();
	c->m_mspWorld->Pop();
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}