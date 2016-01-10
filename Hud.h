// Hud.h: interface for the Hud class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUD_H__6447D76A_DC37_4557_96BB_32DCD0F5EE8E__INCLUDED_)
#define AFX_HUD_H__6447D76A_DC37_4557_96BB_32DCD0F5EE8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"
class Player;
class Info;
class Text;

class Hud : public Renderable  
{
public:
	Hud(DXSystem* dxs, Player* player1, Player* player2);
	virtual ~Hud();
	void render(Context* c);
	void draw(){};

protected:
	Player* m_pPlayer1, *m_pPlayer2;
	Info* m_pInfo1, *m_pInfo2;
	Text* m_ptScore1, *m_ptScore2;
};

#endif // !defined(AFX_HUD_H__6447D76A_DC37_4557_96BB_32DCD0F5EE8E__INCLUDED_)
