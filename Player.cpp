// Player.cpp: implementation of the Player class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Player.h"
#include "Cell.h"
#include "Context.h"
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Player::Player(DXSystem* _dxs, std::string ply):
	AnimatedMd2(_dxs),
	score(0),
	speed(1.0f),
	m_sName("Player"),
	m_pCell(NULL),
	m_pDest(NULL)
{	
	std::ifstream in(ply.c_str());
	if(!in){
		dxs->debugprintf("Player file %s not found!", ply.c_str());
		return;
	}
	std::string header;
	in >> header;
	dxs->debugprintf("Read player header %s", header.c_str() );

	float a, b, c;
	in >> header;
	in >> a;
	in >> b;
	in >> c;
	m_v3Color1 = D3DXVECTOR3(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	m_v3Color2 = D3DXVECTOR3(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	scale(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	translate(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	rotateYawPitchRoll(a, b, c);

	std::string ani, md2, tex;
	in >> md2;
	in >> tex;
	in >> ani;

	AnimatedMd2::load( ani, md2, tex );


	onResetDevice();
	setAnimation("stand");
}

Player::~Player()
{

}

void Player::render(Context *c){
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	//pd3dDevice->SetMaterial( &m_mtrl );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_ADD );
	draw();
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetMaterial( &mtrl );
	c->m_mspWorld->Pop();
}


void Player::setColor( D3DXVECTOR3 c1, D3DXVECTOR3 c2 )
{
	m_v3Color1 = c1;
	m_v3Color2 = c2;
	onResetDevice();
}

void Player::onResetDevice()
{
	ZeroMemory( &m_mtrl, sizeof(D3DMATERIAL8) );
	m_mtrl.Diffuse.r = m_mtrl.Ambient.r = m_v3Color1.x;
	m_mtrl.Diffuse.g = m_mtrl.Ambient.g = m_v3Color1.y;
	m_mtrl.Diffuse.b = m_mtrl.Ambient.b = m_v3Color1.z;
	m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.0f;
}

void Player::move(CELL_DIR d)
{
	if (m_pCell != NULL)
	{
		facing = d;
		state = MOVING;
		m_pDest = m_pCell->n(d);
	}
}

void Player::stop()
{
	state = STANDING;
	m_pDest = NULL;
}

void Player::squash(){
	dxs->debugprintf("Player squashed");
}
void Player::setPosition(Cell* cell, float x, float y)
{
	m_pCell = cell;
	if (cell->state == OCCUPIED ||
		cell->state == RISE2)
		up = true;
	else
		up = false;

	m_v2Position.x = x;
	m_v2Position.y = y;
}


void Player::tick(int ms)
{
	if (m_pCell->state == CLEAR && !m_pCell->into)
		up = false;
	if (m_pCell->state == OCCUPIED)
		up = true;

	AnimatedMd2::tick(ms);
		doMovement(ms);
	if (up)
	{
		if ( m_pCell->state == SINK1 )
			translate(m_v2Position.x-0.5f, (1 - m_pCell->time) + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == RISE2 )
			translate(m_v2Position.x-0.5f, m_pCell->time + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == ROLLOUT )
			translate(m_v2Position.x-0.5f, 0.1f * sinf(D3DX_PI * m_pCell->time) + 1.3f, m_v2Position.y-0.5f);
		else
			translate(m_v2Position.x-0.5f, 1.3f, m_v2Position.y-0.5f);
	}  
	else
	{
		if ( m_pCell->state == SINK2 )
			translate(m_v2Position.x-0.5f, (1 - m_pCell->time) + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == RISE1 )
			translate(m_v2Position.x-0.5f, m_pCell->time + 0.3f, m_v2Position.y-0.5f);
		else
			translate(m_v2Position.x-0.5f, 0.3f, m_v2Position.y-0.5f);
	} 
	switch (facing)
	{
	case NORTH:
		rotateYawPitchRoll(D3DX_PI,0,0);
		break;
	case SOUTH:
		rotateYawPitchRoll(0,0,0);
		break;
	case EAST:
		rotateYawPitchRoll(3*D3DX_PI/2,0,0);
		break;
	case WEST:
		rotateYawPitchRoll(D3DX_PI/2,0,0);
		break;
	}
}


void Player::doMovement(int ms)
{
	float d = speed * (ms / CELL_ROLL_TIME) / 1000.0f;
	Cell* newCell = m_pCell;;
	D3DXVECTOR2 newPos = m_v2Position;

	if ( m_pCell->state == ROLLOUT){
		setAnimation("walk");
		return;
	}

	D3DXVECTOR2 dest;
	dest.x = dest.y = 0.5f;
	if (m_pCell == m_pDest || m_pDest == NULL)
	{
		//Move to the center
	} else if ( state == MOVING ){
		switch (facing)
		{
		case NORTH:
			dest.y += 1;
			break;
		case SOUTH:
			dest.y -= 1;
			break;
		case EAST:
			dest.x += 1;
			break;
		case WEST:
			dest.x -= 1;
			break;
		}
	}

	//Now take into account a sliding die
	if (m_pCell->state == PUSHOUT && up){
		m_pCell->delPlayer(this);
		m_pCell = m_pCell->n(m_pCell->out);
		m_pCell->addPlayer(this);
	/*	switch (m_pCell->out)
		{
		case NORTH:
			m_v2Position.y -= m_pCell->time;
			break;
		case SOUTH:
			m_v2Position.y += m_pCell->time;
			break;
		case EAST:
			m_v2Position.x -= m_pCell->time;
			break;
		case WEST:
			m_v2Position.x += m_pCell->time;
			break;
		}*/
	}

	//find direction from current to dest
	D3DXVECTOR2 dd = dest - m_v2Position;
	//if we are not close to it
	if ( D3DXVec2Length( &dd ) > d ){
		//normalize it
		D3DXVec2Normalize( &dd, &dd);
		//multiply by speed
		dd = dd * d;
		
		newPos = m_v2Position + dd;
		//m_v2Position = newPos; 
	}

	if (newPos.y < 0){
		if ( newCell->n(SOUTH) != NULL ){
			newPos.y += 1;
			newCell = newCell->n(SOUTH);
		} else {
			newPos.y = 0;
		}
	} else 	if (newPos.y > 1){
		if ( newCell->n(NORTH) != NULL ){
			newPos.y -= 1;
			newCell = newCell->n(NORTH);
		} else {
			newPos.y = 1;
		}
	} else 	if (newPos.x < 0){
		if ( newCell->n(WEST) != NULL ){
			newPos.x += 1;
			newCell = newCell->n(WEST);
		} else {
			newPos.x = 0;
		}
	} else 	if (newPos.x > 1){
		if ( newCell->n(EAST) != NULL ){
			newPos.x -= 1;
			newCell = newCell->n(EAST);
		} else {
			newPos.x = 1;
		}
	}
	if (m_pCell->into)
		return;
	if (m_pCell != newCell){
		//You may move to the new cell if
		CELL_STATE c = m_pCell->state;
		CELL_STATE n = newCell->state;
		bool ok = false;
		if (
			( c == RISE1 && n == RISE2 ) || 
			( c == SINK2 && n == SINK1 )
			){
			ok = true;
			up = true;
		}

		if (
			( c == n ) ||
			( c == RISE2 && n == OCCUPIED ) ||
			( c == SINK1 && n == OCCUPIED ) ||
			( c == LIGHTNING && n == SINK2 ) ||
			( c == SINK2 && n == RISE1 ) ||
			( c == RISE1 && n == SINK2 ) ||
			( c == SINK1 && n == RISE2 ) ||
			( c == CLEAR && n == SINK2 ) ||
			//( c == OCCUPIED && n == SINK1 ) ||
			( c == CLEAR && n == LIGHTNING ) ||
			( c == LIGHTNING && n == CLEAR ) ||
			( c == CLEAR && n == RISE1 ) ||
			//( c == RISE1 && n == CLEAR ) || 
			( c == LIGHTNING && n == RISE1 ) ||
			( c == RISE1 && n == LIGHTNING ) ||
			//( c == OCCUPIED && n == RISE2 ) ||
			( c == SINK2 && n == CLEAR ) ||
			( c == SINK2 && n == LIGHTNING ) ||
			( c == RISE2 && n == SINK1 )
			){
			ok = true;
		}

	/*	if ( // COMMENTED OUT SO YOU CANT WALK DOWN
			//( c == SINK1 && n == SINK2 )
			//( c == RISE2 && n == RISE1 )
			){
			up = false;
			ok = true;
		}*/

		if (!ok)
			setAnimation("stand");
		else
			setAnimation("walk");

		if (ok)
		{ // Move to the new cell
			m_pCell->delPlayer(this);
			newCell->addPlayer(this);
			m_pCell=newCell;
			m_v2Position = newPos;
		} else if ( up && m_pCell->state == OCCUPIED ){
			m_pCell->roll(facing, this);
		} else if ( !up && newCell->state == OCCUPIED ){
			if (m_pCell->n(facing)->push(facing, this) == YES)
			{
				m_pCell->delPlayer(this);
				newCell->addPlayer(this);
				m_pCell=newCell;
				m_v2Position = newPos;
			} else {
				setAnimation("stand");
			}
		}
	} else {
		if ( m_v2Position == newPos )
			setAnimation("stand");
		else
			setAnimation("walk");

		m_v2Position = newPos;
	}
}