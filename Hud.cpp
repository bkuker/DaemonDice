// Hud.cpp: implementation of the Hud class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Hud.h"
#include "Player.h"
#include "Text.h"
#include "Context.h"
#include "Info.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Hud::Hud(DXSystem* dxs, Player* player1, Player* player2):
	Renderable(dxs),
	m_pPlayer1(player1),
	m_pPlayer2(player2)
{
	m_pInfo1 = new Info(dxs, player1);
	m_ptScore1 = new Text(dxs);

	if ( player2 == NULL ){
		//setup for 1 player
		m_pInfo1->scale(0.2f,0.2f,0.1f);
		m_pInfo1->translate(0.1f,0,0);
		
		m_ptScore1->scale(0.08f, 0.08f, 1.0f);
		m_ptScore1->translate(0.0f,0.4f,0);
	} else {
		m_pInfo1->scale(0.2f,0.2f,0.1f);
		m_pInfo1->translate(0.0f,0,0);	

		m_ptScore1->scale(0.05f, 0.05f, 1.0f);
		m_ptScore1->translate(0.2f,0.05f,0);

		m_pInfo2 = new Info(dxs, player2);
		m_ptScore2 = new Text(dxs);
		m_pInfo2->scale(0.2f,0.2f,0.1f);
		m_pInfo2->translate(0.8f,0,0);
		
		m_ptScore2->justify(RIGHT);
		m_ptScore2->scale(0.05f, 0.05f, 1.0f);
		m_ptScore2->translate(0.75f,0.01f,0);
	}
}
	
	Hud::~Hud()
	{
		delete m_pInfo1;
		delete m_ptScore1;
		
		if ( m_pPlayer2 != NULL)
		{
			delete m_pInfo2;
			delete m_ptScore2;
		}
		
	}

void Hud::render(Context* c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

	m_pInfo1->render(c);	
	m_ptScore1->printf("%s's score: %i", m_pPlayer1->getName().c_str(), m_pPlayer1->getScore() );
	m_ptScore1->render(c);
	
	if (m_pPlayer2 != NULL)
	{
		m_pInfo2->render(c);	
		m_ptScore2->printf("%s's score: %i", m_pPlayer2->getName().c_str(), m_pPlayer2->getScore() );
		m_ptScore2->render(c);
	}

	c->m_mspWorld->Pop();	 
}