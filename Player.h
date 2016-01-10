// Player.h: interface for the Player class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYER_H__DDA0BB7F_EE6D_470D_9257_DA8AA1D991B6__INCLUDED_)
#define AFX_PLAYER_H__DDA0BB7F_EE6D_470D_9257_DA8AA1D991B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnimatedMd2.h"

class Cell;
class Info;

#ifndef __CELL_DIR__
#define __CELL_DIR__ 
enum CELL_DIR {NORTH, SOUTH, EAST, WEST, NONE}; 
#endif

enum PLAYER_STATE {STANDING, MOVING};

class Player : public AnimatedMd2
{
public:
	friend Cell;
	friend Info;
	Player(DXSystem* dxs, std::string ply);
	virtual ~Player();

	std::string getName(){ return m_sName; };

	void render(Context* c);
	void move(CELL_DIR);
	void stop();
	void squash();
	void setPosition(Cell* cell, float x, float y);
	void setColor( D3DXVECTOR3 c1, D3DXVECTOR3 c2 );

	//Score things..
	void award(int p){ score += p;};
	int getScore(){ return score; };
	void resetScore(){ score = 0; };

	void tick(int milliseconds);

	void onResetDevice();

protected:
	void doMovement(int milliseconds); 

protected:
	int score;
	Cell *m_pCell, *m_pDest;
	std::string m_sName;
	D3DXVECTOR3 m_v3Color1, m_v3Color2;
	D3DXVECTOR2 m_v2Position;
	CELL_DIR facing;
	PLAYER_STATE state;
	bool up;
	float speed;
	
	///Graphics
	D3DMATERIAL8 m_mtrl;
};

#endif // !defined(AFX_PLAYER_H__DDA0BB7F_EE6D_470D_9257_DA8AA1D991B6__INCLUDED_)
