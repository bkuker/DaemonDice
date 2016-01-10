#include "DXSystem.h"

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