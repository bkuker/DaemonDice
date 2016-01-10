// Game.h: interface for the Game class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Game_H__79C09CDE_C501_4281_8498_B379A4927EFD__INCLUDED_)
#define AFX_Game_H__79C09CDE_C501_4281_8498_B379A4927EFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DXSystem.h"
#include "Renderable.h"
#include "Camera.h"
#include "Context.h"

class Game : public DXSystem  
{
public:
	Game(char* appTitle, HINSTANCE hInstance);
	virtual ~Game();
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void drawScene();
	virtual void inputCB(BOOL* input);
	void reset(int argc, char** argv);
	Renderable *c,*b,*i, *n;
	Context m_cHud, m_cWorld;
	D3DXMATRIX matWorld, matView, matWProj, matHudProj, matIdent, mt;
};


#define _GENRE DIVIRTUAL_STRATEGY_ROLEPLAYING
//All of the actions a player is able to do with an input press
enum INPUT_ACTIONS{
	INPUT_1UP,
	INPUT_1DOWN,
	INPUT_1RIGHT,
	INPUT_1LEFT,
	INPUT_2UP,
	INPUT_2DOWN,
	INPUT_2RIGHT,
	INPUT_2LEFT,


	//Required at the end
	NUM_INPUT_ACTIONS,
	ENUMSIZE = 0xffffffff
};

DIACTION rgGameAction[NUM_INPUT_ACTIONS] =
{
    // Device input pre-defined by DInput, according to genre
	{INPUT_1UP, DIKEYBOARD_W, 0, "Player 1 Up",},
	{INPUT_1DOWN, DIKEYBOARD_S, 0, "Player 1 Down",},
	{INPUT_1LEFT, DIKEYBOARD_A, 0, "Player 1 Left",},
	{INPUT_1RIGHT, DIKEYBOARD_D, 0, "Player 1 Right",},

	{INPUT_2UP, DIKEYBOARD_I, 0, "Player 2 Up",},
	{INPUT_2DOWN, DIKEYBOARD_K, 0, "Player 2 Down",},
	{INPUT_2LEFT, DIKEYBOARD_J, 0, "Player 2 Left",},
	{INPUT_2RIGHT, DIKEYBOARD_L, 0, "Player 2 Right",},
};
#endif // !defined(AFX_Game_H__79C09CDE_C501_4281_8498_B379A4927EFD__INCLUDED_)
