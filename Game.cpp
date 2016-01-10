// Game.cpp: implementation of the Game class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Game.h"
#include "Board.h"
#include "Hud.h"
#include "Player.h"
#include "AnimatedMd2.h"


#include <time.h>

void inputParamCB(int &numActions, GUID &gameguid, DWORD &genre, DIACTION* &actions, TCHAR name[MAX_PATH]);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Game::Game(char* appTitle, HINSTANCE hInstance)
:DXSystem(appTitle, hInstance, &inputParamCB)
{
	console = new Console( this );
	console->setTexture(loadTexture( "textures/console.dds" ));

	console->registerConsoleCommand(
		"inputconf", 
		(CONSOLE_CALLBACK_OBJECT)dxs, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::inputConf));
	console->registerConsoleCommand(
		"fullscreen", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::setFullScreen));
	console->registerConsoleCommand(
		"wire", 
		(CONSOLE_CALLBACK_OBJECT)this,
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::wire));
	console->registerConsoleCommand(
		"quit", 
		(CONSOLE_CALLBACK_OBJECT)dxs, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::quit));
	console->registerConsoleCommand(
		"mute", 
		(CONSOLE_CALLBACK_OBJECT)dxs, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::mute));
	console->registerConsoleCommand(
		"reset", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION)(reset));


	D3DXMatrixIdentity( &matIdent );

	//Build world context
	D3DXMatrixLookAtLH(&matView, 
		&D3DXVECTOR3( 8,8,-8),
		&D3DXVECTOR3( 0,-1,0),
		&D3DXVECTOR3( 0,1,0));
	D3DXMatrixPerspectiveFovLH( &matWProj, D3DX_PI/4, 4.0f/3.0f, 0.1f, 1000.0f );
	m_cWorld.SetTransform( D3DTS_WORLD, &matIdent );
	m_cWorld.SetTransform( D3DTS_VIEW, &matView);//m_Camera.transform() );
	m_cWorld.SetTransform( D3DTS_PROJECTION, &matWProj );

	//build hud context
	D3DXMatrixOrthoOffCenterLH(&matHudProj, 0, 1, 1, 0, 0, 1000);
	m_cHud.SetTransform( D3DTS_WORLD, &matIdent );
	m_cHud.SetTransform( D3DTS_VIEW, &matIdent );
	m_cHud.SetTransform( D3DTS_PROJECTION, &matHudProj );
 
	//Set up the light
	D3DLIGHT8 light;
	ZeroMemory( &light, sizeof(D3DLIGHT8) );
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Direction = D3DXVECTOR3(-1,-4,1);
	pd3dDevice->SetLight( 0, &light );
	pd3dDevice->LightEnable( 0, TRUE);

	//Mute sound so I dont go 'naners
	dxs->mute(true);

	//Seed the random number generator
	srand( (unsigned)time( NULL ) );

	///Test  stuff
	b = new Board(dxs, 10, "textures/board.dds");
	((Board*)b)->reset();
	((Board*)b)->fill(0.6f);
	((Board*)b)->setFreq(0.5f);

	c = new Player(dxs, "models/akiko.ply");
	((Player*)c)->setPosition(((Board*)b)->cell(0,0),0.5f,0.5f);
	((Board*)b)->cell(0,0)->addPlayer((Player*)c);

	n = new Player(dxs, "models/wizard.ply");
	((Player*)n)->setPosition(((Board*)b)->cell(5,5),0.5f,0.5f);
	((Board*)b)->cell(5,5)->addPlayer((Player*)n);

	i = new Hud(dxs,(Player*)c, (Player*)n);
	
}

Game::~Game()
{
	delete c;
	delete b;
	delete n;
	delete i;
	if ( console != NULL )
		 delete console;
}

void Game::reset(int argc, char** argv){
	((Board*)b)->reset();
	((Board*)b)->fill(0.6f);
	((Board*)b)->setFreq(0.2f);
}


void Game::onLostDevice(){
	console->onLostDevice();	
}

void Game::onResetDevice(){	
	//Set up the light
	D3DLIGHT8 light;
	ZeroMemory( &light, sizeof(D3DLIGHT8) );
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Direction = D3DXVECTOR3(-1,-4,1);
	pd3dDevice->SetLight( 0, &light );
	pd3dDevice->LightEnable( 0, TRUE);

	console->onResetDevice();
}

void Game::drawScene(){
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ); 
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
	

	//mip mapping
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

	m_cWorld.apply( pd3dDevice );
	b->render(&m_cWorld);
	//n->render(&m_cWorld);

	m_cHud.apply( pd3dDevice );
	i->render(&m_cHud);
	console->render(&m_cHud);
	
}

#define MOVE_STEP 0.1f
#define ROTATE_STEP 0.1f
void Game::inputCB(BOOL* input){
	
	((Board*)b)->tick(getFrameTime());
	((Player*)c)->tick(getFrameTime());
	((Player*)n)->tick(getFrameTime());

	//process input
	if ( input[INPUT_1UP] ){
		((Player*)c)->move(NORTH);
	} else if( input[INPUT_1DOWN] ){
		((Player*)c)->move(SOUTH);
	} else if( input[INPUT_1RIGHT] ){
		((Player*)c)->move(EAST);
	} else if( input[INPUT_1LEFT] ){
		((Player*)c)->move(WEST);
	} else {
		((Player*)c)->stop();
	}

	if ( input[INPUT_2UP] ){
		((Player*)n)->move(NORTH);
	} else if( input[INPUT_2DOWN] ){
		((Player*)n)->move(SOUTH);
	} else if( input[INPUT_2RIGHT] ){
		((Player*)n)->move(EAST);
	} else if( input[INPUT_2LEFT] ){
		((Player*)n)->move(WEST);
	} else {
		((Player*)n)->stop();
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
		Game* g = new Game("Game", hInstance);
		g->run();
		delete g;
		return 0;
}

const GUID APPGUID = {0xa64c477c, 0x1f5e, 0x5391, {0xcb, 0xce, 0xe7, 0x3c, 0x13, 0xcd, 0xfb, 0x2f}};
void inputParamCB(int &numActions, GUID &gameguid, DWORD &genre, DIACTION* &actions, TCHAR name[MAX_PATH]){
	numActions	= NUM_INPUT_ACTIONS;
	gameguid	= APPGUID;
	genre		= _GENRE;
	actions		= rgGameAction;
	_tcscpy(name, _T("Daemon Dice Controls") );
}