// Cell.cpp: implementation of the Cell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cell.h"
#include "math.h"
#include "Player.h"

CELL_DIR c(CELL_DIR); //returns the opposite direction

std::set<Cell*> Cell::visited;
int Cell::matchCount = 0;
int Cell::dieCount = 0;
Player* Cell::sinker = NULL;

bool Cell::oneFall = false;
Cell* Cell::oneStart = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cell::Cell(DXSystem* _dxs, int _x, int _y):
	Renderable(_dxs),
	dxs(_dxs),
	x(_x),
	y(_y),
	snd_roll(dxs->loadSound("sounds/roll.wav")),
	snd_slide(dxs->loadSound("sounds/slide.wav")),
	snd_lightning(dxs->loadSound("sounds/lightning.wav")),
	die(NULL),
	lightning(NULL),
	checkMatch(false),
	m_pWhoSunk(NULL),
	chain(0)
{
	reset();
}

Cell::~Cell()
{
	if (die != NULL)
		delete die;
	if (lightning != NULL)
		delete lightning;
	dxs->releaseSound(snd_roll);
	dxs->releaseSound(snd_slide);
	dxs->releaseSound(snd_lightning);
}

void Cell::addPlayer( Player* pe)
{
	players.insert(pe);
}

void Cell::delPlayer( Player* pe)
{
	std::set<Player*>::iterator i = players.find(pe);
	if (i != players.end())
		players.erase(i);
}


void Cell::reset()
{
	if (die != NULL)
		delete die;
	if (lightning != NULL)
		delete lightning;
	die = NULL;
	lightning = NULL;
	time = 0;
	state = CLEAR;
	into = false;
	chain = 0;
	m_pWhoSunk = NULL;
	checkMatch = false;
}

void Cell::render(Context* c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	if (die != NULL)
	{
		if ( m_pWhoSunk != NULL && (state == SINK1 || state == SINK2) )
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			pd3dDevice->SetMaterial( &(m_pWhoSunk->m_mtrl) );
			die->render(c);
			pd3dDevice->SetMaterial( &mtrl );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		}
		else
		{
			die->render(c);
			//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			//pd3dDevice->SetMaterial( &mtrl );
		}
	}
	if (lightning != NULL)
		lightning->render(c);
	for (std::set<Player*>::iterator i = players.begin(); i != players.end(); i++ )
			(*i)->render(c);
	c->m_mspWorld->Pop();
}


void Cell::tick(int ms)
{
	if (oneFall && this == oneStart)
	{
		oneStart = NULL;
		oneFall = false;
		dxs->debugprintf("One stop");
	}
	if (time < 0)
		time = 0;
	switch (state){
	case CLEAR:
		break;
	case LIGHTNING:
		time = time + ms / CELL_LIGHTNING_TIME / 1000;
		lightning->tick(ms);
		if (time >= 1.0f)
			step();
		break;
    case RISE1:
		time = time + ms / CELL_RISE_TIME / 1000;
		die->rise(time);
		if (time >= 0.5f)
			step();
        break;
	case RISE2:
		time = time + ms / CELL_RISE_TIME / 1000;
		die->rise(time);
		if (time >= 1.0f)
			step();
        break;
	case OCCUPIED:
		if ( players.size() > 0)
			die->rise(0.9f);
		else
			die->rise(1);
		if ( n_up == 1 && oneFall && this != oneStart)
		{
			m_pWhoSunk = oneStart->m_pWhoSunk;
			m_pWhoSunk->award(1);
			sink(0);
		}
		if ( checkMatch )
		{	
			matchCount = 0;
			visited.clear();
			sinker = m_pWhoSunk;
			matchCheck();
			sinker = NULL;
			checkMatch = false;
		}
        break;
	case ROLLOUT:
		time = time + ms / CELL_ROLL_TIME / 1000;
		die->roll(out, time);
		if (time >= 1.0f)
			step();
		break;
	case PUSHOUT:
		time = time + ms / CELL_PUSH_TIME / 1000;
		die->slide(out, time);
		if (time >= 1.0f)
			step();
        break;
	case SINK1:
		time = time + ms / CELL_SINK_TIME / 1000;
		die->rise(1.0f-time);
		if (time >= 0.5f)
			step();
        break;
	case SINK2:
		time = time + ms / CELL_SINK_TIME / 1000;
		die->rise(1.0f-time);
		if (time >= 1.0f)
			step();
        break;
	}
}

void Cell::step()
{
	Cell* no = NULL;
	int e,s,u;
	Die* d;
	Lightning* l;
    float t;
	CELL_STATE st;
	std::set<Player*>::iterator i;

	switch (state){
	case CLEAR:
		break;
	case LIGHTNING:
		if (lightning != NULL){
			delete lightning;
			lightning = NULL;
		}
		makeDie();
		state = RISE1;
		time=0;
		break;
    case RISE1:
		for (i = players.begin(); i != players.end(); i++ )
			(*i)->up = true;
		state = RISE2;
        break;
	case RISE2:
		state = OCCUPIED;
        break;
	case OCCUPIED:
        break;
	case ROLLOUT:
		///Move players to next cell
		for (i = players.begin(); i != players.end(); i++ ){
			if ( (*i)->up )
			{
				n(out)->players.insert(*i);
				(*i)->m_pCell = n(out);
				//(*i)->m_v2Position.y = 0.5f;
				//(*i)->m_v2Position.x = 0.5f;
				switch (out){
				case NORTH:
					(*i)->m_v2Position.y = 0;
					break;
				case SOUTH:
					(*i)->m_v2Position.y = 1;
					break;
				case EAST:
					(*i)->m_v2Position.x = 0;
					break;
				case WEST:
					(*i)->m_v2Position.x = 1;
					break;
				}
			}
		}
		players.clear();

		rotate(out);
		die->rotate(out);
		dxs->playSound(snd_roll);
	case PUSHOUT:
		die->rise(1);

		//squish'em
		for (i = n(out)->players.begin(); i != n(out)->players.end(); i++ ){
			if ( !((*i)->up)){
				(*i)->squash();
				addPlayer(*i);
				(*i)->m_pCell = this;
			}
		}
		//Now remove all the players in here from n(out)
		//I know it might make an unneeded call or 2
		for (i = players.begin(); i != players.end(); i++ ){
			n(out)->delPlayer(*i);
		}


		no = n(out);
		//Store neighbor vars
		t = no->time;
		e = no->n_east;
		u = no->n_up;
		s = no->n_south;
		l = no->lightning;
		d = no->die;
		st = no->state;
		
		//set neighbor 
		no->state = OCCUPIED;
		no->n_east = n_east;
		no->n_south = n_south;
		no->n_up = n_up;
		no->die = die;
		no->time = 0;
		no->into = false;
		no->checkMatch = true;
		no->lightning = NULL;
		no->m_pWhoSunk = m_pWhoSunk;

		no->chain = 0;
		chain = 0;
		checkMatch = false;
		m_pWhoSunk = NULL;

		//set my vars
		if ( st == RISE1 || st == RISE2 || st == LIGHTNING){
			state = st;
			die = d;
			lightning = l;
			time = t;
			n_up = u;
			n_east = e;
			n_south = s;
			into = false;
		} else {
			if ( l != NULL )
				delete l;
			if ( d != NULL )
				delete d;
			die = NULL;
			state = CLEAR;
			into = false;
		}
        break;
	case SINK1:
		for (i = players.begin(); i != players.end(); i++ )
			(*i)->up = false;
		state = SINK2;
        break;
	case SINK2:
		delete die;
		die = NULL;
		state = CLEAR;
		m_pWhoSunk = NULL;
		chain = 0;
        break;
	}
	//dxs->debugprintf("Cell %i,%i step() to %i.", x, y, state);
}

CELL_RESULT Cell::roll(CELL_DIR d, Player* p)
{
	if ( n(d) == NULL )
		return NO;
	if ((
			n(d)->state != CLEAR &&
			n(d)->state != SINK2 &&
			n(d)->state != LIGHTNING &&
			n(d)->state != RISE1
		)||
		state != OCCUPIED
		|| n(d)->into)
		return NO;
	time=0;
	state = ROLLOUT;
	n(d)->into = true;
	out = d;
	n(d)->in = c(d);
	m_pWhoSunk = p;
	return YES;
}

CELL_RESULT Cell::push(CELL_DIR d, Player* p)
{
	if ( n(d) == NULL )
		return NO;
	if ((
			n(d)->state != CLEAR &&
			n(d)->state != SINK2 &&
			n(d)->state != LIGHTNING &&
			n(d)->state != RISE1
		)||
		state != OCCUPIED
		|| n(d)->into)
		return NO;
	time=0;
	state = PUSHOUT;
	dxs->playSound(snd_slide);
	n(d)->into = true;
	out = d;
	n(d)->in = c(d);
	m_pWhoSunk = p;
	return YES;
}

CELL_RESULT Cell::fill()
{
	if ( state != CLEAR )
		return NO;
	if ( die != NULL )
		dxs->debugprintf("Cell %i,%i has die object on fill() call!", x, y);
	state = LIGHTNING;
	dxs->playSound(snd_lightning);
	lightning = new Lightning(dxs);
	time = 0;
	dieCount++;
	chain = 0;
	return YES;
}

void Cell::makeDie()
{
	if ( die != NULL )
		dxs->debugprintf("Cell %i,%i has die object on makeDie() call!", x, y);
	die = new Die(dxs);
	n_up = 4;
	n_east = 5;
	n_south = 1;
	//Random rotate(s)
	int r = rand() * 6 / RAND_MAX;
	int r2 = rand() * 4 / RAND_MAX;
	int r3 = rand() * 4 / RAND_MAX;
	switch (r){
	case 0:
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		die->rotate((CELL_DIR)r2);
		rotate((CELL_DIR)r2);
		break;
	case 5:
		die->rotate(NORTH);
		rotate(NORTH);
		die->rotate(NORTH);
		rotate(NORTH);
		break;
	}
	if (r3 > 0){
		die->rotate(NORTH);
		rotate(NORTH);
		for (int i = 0; i < r3; i++){
			die->rotate(EAST);
			rotate(EAST);
		}
		die->rotate(SOUTH);
		rotate(SOUTH);
	}
}

CELL_RESULT Cell::sink(int c)
{
	if ( state == OCCUPIED )
	{
		dieCount--;
		state=SINK1;
		time=0;
	}
	if ( n_up != 1 ){
		std::set<Cell*>::iterator i = visited.find(this);
		if ( i != visited.end() )
			return YES;
		visited.insert(this);
		for (int x = 0; x < 4; x++)
		{
			if (n((CELL_DIR)x) != NULL &&
				n((CELL_DIR)x)->n_up == n_up &&
					(n((CELL_DIR)x)->state == OCCUPIED ||
					 n((CELL_DIR)x)->state == SINK1 ||
					 n((CELL_DIR)x)->state == SINK2
					)
				)
			{	
				chain = c;
				n((CELL_DIR)x)->sink(c);	
				time = time - 0.1f;
				//if this puts it back into SINK1 time fix it.
				if ( time < 0.5f && state == SINK2){
					state = SINK1;
					std::set<Player*>::iterator i;
					for (i = players.begin(); i != players.end(); i++ )
						(*i)->up = true;
				}
			}
		}
	}
	return YES;
}

void Cell::rotate(CELL_DIR d)
{
	int o_up = n_up;
	int o_east = n_east;
	int o_south = n_south;
	switch (d){
	case NORTH:
		n_up = o_south;
		n_south = 7 - o_up;
		break;	
	case SOUTH:
		n_up = 7 - o_south;
		n_south = o_up;
		break;	
	case EAST:
		n_east = o_up;
		n_up = 7-o_east;
		break;	
	case WEST:
		n_up = o_east;
		n_east = 7 - o_up;
		break;
	case NONE:
		break;
	}
}

void Cell::matchCheck()
{
	static int max;
	m_pWhoSunk = sinker;
	if (n_up != 1)
	{
		std::set<Cell*>::iterator i = visited.find(this);
		if ( i != visited.end() )
			return;
		visited.insert(this);
		matchCount++;
		if (checkMatch)
			max = 0;
		if ( chain > max )
			max = chain;
		for (int x = 0; x < 4; x++)
		{
			if ( n((CELL_DIR)x) != NULL &&
				n((CELL_DIR)x)->n_up == n_up
				)
			{
				if ( n((CELL_DIR)x)->state == OCCUPIED )
					n((CELL_DIR)x)->matchCheck();
				else if ( 
					n((CELL_DIR)x)->state == SINK1 ||
					n((CELL_DIR)x)->state == SINK2
					)
				{
					n((CELL_DIR)x)->matchCheck();
				}
			}
		}
		if (matchCount >= n_up && checkMatch)
		{
			chain = max + 1;
			dxs->debugprintf("Matched %i %i's X %i", matchCount, n_up, chain);
			m_pWhoSunk->award( chain * matchCount * n_up );
			visited.clear();
			sink(chain);
		}

	} else {
		//do one check
		for (int x = 0; x < 4; x++)
		{
			if (n((CELL_DIR)x) != NULL && n((CELL_DIR)x)->state == SINK1 && n((CELL_DIR)x)->n_up != 1 ||
				n((CELL_DIR)x) != NULL && n((CELL_DIR)x)->state == SINK2 && n((CELL_DIR)x)->n_up != 1)
			{
				dxs->debugprintf("Ones Falling");
				oneFall = true;
				oneStart = this;
			}
		}
	}
	
}

Cell* Cell::n(CELL_DIR d)
{
	switch (d){
	case NORTH:
		return north;
		break;	
	case SOUTH:
		return south;
		break;	
	case EAST:
		return east;
		break;	
	case WEST:
		return west;
		break;
	default:
		return NULL;
	}

}

CELL_DIR c(CELL_DIR d){
	switch (d){
	case NORTH:
		return SOUTH;
		break;	
	case SOUTH:
		return NORTH;
		break;	
	case EAST:
		return WEST;
		break;	
	case WEST:
		return EAST;
		break;
	default:
		return NORTH;
	}

}