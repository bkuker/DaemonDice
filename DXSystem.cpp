// DXSystem.cpp: implementation of the DXSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DXSystem.h"
#define __WITH_SOUND

//////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////

	
// Defines and globals will be class vars someday
#define WINCLASS "9C2A3A67-D7AE-4578-B995-5B12964ACB54-IsoEngineSTD"
#define BYTEPP			4
#define INPUT_BUFSIZE	16

LRESULT CALLBACK WinProc (HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DXSystem::DXSystem(char* appTitle, HINSTANCE hInstance, INPUTPARAMCB inputParamCB):
	m_pLoader(NULL),
	m_pPerformance(NULL),
	pd3dDevice(NULL),
	pd3d(NULL),
	pdi8(NULL),
	devLost(FALSE),	
	console(NULL),
	numInputDevices(0),
	inputDevices(NULL),
	m_bMute(false)
{
	//Callback functions that use DirectX will find the DXERROR macro convenient
	//DXERROR requires a DXSystem* dxs so that static member functions can use it
	//Pardon the redundency
	this->dxs = this;

	////////////////////////////
	//Initialize Window
	////////////////////////////
	//Build a windowclass to use with some
	//rather generic values.
	WNDCLASSEX wc;
	wc.cbSize		=	sizeof( WNDCLASSEX );
	wc.style		=	CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc	=	(WNDPROC)WinProc;
	wc.cbClsExtra	=	0;
	wc.cbWndExtra	=	0;
	wc.hInstance	=	hInstance;
	wc.hIcon		=	0;
	wc.hCursor		=	0;
	wc.hbrBackground=	(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName	=	0;
	wc.lpszClassName=	WINCLASS;
	wc.hIconSm		=	0;

	//register the windowclass
	RegisterClassEx( &wc );

	//Make a new MAIN window to display in
	hWindow = CreateWindowEx(	
				0,
				WINCLASS,
				appTitle,
				//WS_POPUP | WS_SYSMENU,	//No title borderless
				WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, //title, border, close
				//WS_OVERLAPPEDWINDOW, //full, resizable
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				WIDTH,
				HEIGHT,
				NULL,
				NULL,
				hInstance,
				NULL ); //create main

	////////////////////////////
	//Initialize DirectX Graphics
	////////////////////////////
	//d3d init -- Get a d3d 8 object
	if((pd3d = Direct3DCreate8( D3D_SDK_VERSION )) == NULL )
		error("Failed to create DirectX object. (__FILE__:__LINE__)");
	

	setFullScreen(FALSE);	//Startup in windowed mode
	
	//create the d3d device
	DXERROR(pd3d->CreateDevice(	
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL, 
				hWindow,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&pd3dDevice ));

	//Display & update the window
	ShowWindow(hWindow, SW_RESTORE);
	UpdateWindow(hWindow);

	////////////////////////////
	//Initialize timing
	////////////////////////////
	if ( !QueryPerformanceFrequency( (LARGE_INTEGER*)&hpcFreq ) )
		error("System does not support high resolution timer. (__FILE__:__LINE__)");

	////////////////////////////
	//Initialize Input
	////////////////////////////
	DXERROR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pdi8, NULL));
	//Initialize action format
	int	numInputActions;
	ZeroMemory(&diaf, sizeof(DIACTIONFORMAT));
	diaf.dwSize        = sizeof(DIACTIONFORMAT);
	diaf.dwActionSize  = sizeof(DIACTION);
	//Callback to game code to get info about game input
	inputParamCB(numInputActions, diaf.guidActionMap, diaf.dwGenre, diaf.rgoAction, diaf.tszActionMap);
	diaf.dwDataSize    = numInputActions * sizeof(DWORD);
	diaf.dwNumActions  = numInputActions;
	diaf.dwBufferSize  = INPUT_BUFSIZE;
	DXERROR(pdi8->EnumDevicesBySemantics(NULL, &diaf, DIEnumDevicesBySemanticsCallback, this, 0));
	actionState = new BOOL[numInputActions];

	////////////////////////////
	//Initialize Sound
	////////////////////////////
#ifdef __WITH_SOUND
	//Create Loader and Performace COM Objects
	CoInitialize(NULL);
	CoCreateInstance(CLSID_DirectMusicLoader, NULL, 
		CLSCTX_INPROC, IID_IDirectMusicLoader8,
		(void**)&m_pLoader);
	CoCreateInstance(CLSID_DirectMusicPerformance, NULL,
		CLSCTX_INPROC, IID_IDirectMusicPerformance8,
		(void**)&m_pPerformance );	
	m_pPerformance->InitAudio( 
		NULL,      // IDirectMusic interface not needed.
		NULL,      // IDirectSound interface not needed.
		hWindow,      // Window handle.
		DMUS_APATH_SHARED_STEREOPLUSREVERB,  // Default audiopath type.
		32,        // Number of performance channels.
		DMUS_AUDIOF_ALL,   // Features on synthesizer.
		NULL       // Audio parameters; use defaults.
		);
#endif
}

DXSystem::~DXSystem()
{
	//Unset pointer to this object associated with window
	//this should stay at the begining of the destructor
	SetWindowLong( hWindow, GWL_USERDATA, NULL);
	//Documentation says SetWindowPos should be called after SetWindowLong
	SetWindowPos(hWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	
	if(pd3dDevice != NULL)
        pd3dDevice->Release();
	if(pd3d != NULL)
        pd3d->Release();
	//if(pdxfDefFont != NULL)
	//	pdxfDefFont->Release();

	delete actionState;
	releaseInput();
	
	if(pdi8)
		pdi8->Release();
#ifdef __WITH_SOUND
	//////////////////////
	//Tear down Sound
	//////////////////////
	m_pPerformance->Stop(
		NULL, // Stop all segments.
		NULL, // Stop all segment states.
		0,  // Do it immediately.
		0   // Flags.
		);
	m_pPerformance->CloseDown();
	m_pLoader->Release(); 
	m_pPerformance->Release();
	CoUninitialize();
#endif
}

//////////////////////////////////////////////////////////////////////
// Public functions
//////////////////////////////////////////////////////////////////////

void DXSystem::run()
{
	MSG msg;

	SetWindowLong( hWindow, GWL_USERDATA, (long)this);
	//Documentation says SetWindowPos should be called after SetWindowLong
	SetWindowPos(hWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	while( msg.message != WM_QUIT )
	{
		//Check for a message and handle if needed
		if(devLost){
			//Blocking message loop to save CPU when paused
			if(!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			processInput();
						
			//Calculate time since last frame
			static INT64 lastTime;
			INT64 now;
			QueryPerformanceCounter( (LARGE_INTEGER*)&now );
			INT64 hpcElapsed = now - lastTime;
			QueryPerformanceCounter( (LARGE_INTEGER*)&lastTime );
			elapsedTime = hpcElapsed * 1000.0f/(float)hpcFreq;
			if (elapsedTime < 0)
				elapsedTime = 0;

			//Call client game logic
			inputCB(actionState);
			render();
		}
	}
}

int DXSystem::getFrameTime() {
	return elapsedTime; 
};

void DXSystem::render(){

	DXERROR(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0));

	DXERROR(pd3dDevice->BeginScene());
	drawScene();
	DXERROR(pd3dDevice->EndScene());	
	DXERROR(pd3dDevice->Present(0,0,0,0));
	ValidateRect(hWindow, NULL);
}

void DXSystem::drawScene(){

}

void DXSystem::debugprintf(const char *format, ...){
	va_list argptr;
	va_start(argptr, format);
	if (console != NULL)
		console->vcprintf(format, argptr);
}

//////////////////////////////////////////////////////////////////////
// Texture functions
//////////////////////////////////////////////////////////////////////
LPDIRECT3DTEXTURE8 DXSystem::loadTexture( std::string file ){
	//create the texture
	std::map< std::string, LPDIRECT3DTEXTURE8 >::iterator i;
	i = textures.find( file );
	if ( i == textures.end() ){
		LPDIRECT3DTEXTURE8 pTexture = NULL;
		
		if (D3DXCreateTextureFromFileEx(	pd3dDevice, 
										file.c_str(),
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										0,
										D3DFMT_UNKNOWN,
										D3DPOOL_MANAGED,
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										0,
										NULL,
										NULL,
										&pTexture) != D3D_OK){
			debugprintf("Failed to load Texture %s.", file.c_str());
			//throw "Texture Error: Could not find file.";
		}
		textures[ file ] = pTexture;
		texfile[ pTexture ] = file;
		texref[ pTexture ] = 1;
		//debugprintf("Loaded Texture %s.", file.c_str());
		return pTexture;
	} else {
		texref[i->second]++;
		return i->second;
	}
}

void DXSystem::releaseTexture( LPDIRECT3DTEXTURE8 tex ){
	if ( tex ){
		texref[tex]--;
		if ( texref[tex] == 0 ){
			//debugprintf("Unloaded Texture %s.", texfile[tex].c_str());
			//texref.erase( tex );
			textures.erase( texfile[tex] );
			texfile.erase( tex );
			((LPDIRECT3DTEXTURE8)tex)->Release();
		}
	}
}
//////////////////////////////////////////////////////////////////////
//Sound Functions
/////////////////////////////////////////////////////////////////////
void DXSystem::playSound(SOUND_REF snd)
{
#ifdef __WITH_SOUND
	if (!m_bMute)
	{
		m_pPerformance->PlaySegmentEx(
		snd,  // Segment to play.
		NULL,    // Used for songs; not implemented.
		NULL,    // For transitions. 
		DMUS_SEGF_SECONDARY,     // Flags.
		0,     // Start time; 0 is immediate.
		NULL,    // Pointer that receives segment state.
		NULL,    // Object to stop.
		NULL   // Audiopath, if not default.
		);
	}
#endif
}

void DXSystem::mute(bool m)
{
	m_bMute = m;
}

SOUND_REF DXSystem::loadSound( std::string file )
{
#ifdef __WITH_SOUND
	WCHAR wstrSearchPath[MAX_PATH] = L".";
	
	m_pLoader->SetSearchDirectory( 
		GUID_DirectMusicAllTypes, // Types of files sought.
		wstrSearchPath,     // Where to look.
		FALSE         // Don't clear object data.
		);


	// Convert to Unicode.
	WCHAR wstrFile[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, file.c_str(), -1, 
		wstrFile, MAX_PATH );


	//create the Sound
	std::map< std::string, SOUND_REF >::iterator i;
	i = sounds.find( file );
	if ( i == sounds.end() ){
		SOUND_REF pSound = NULL;
		//load sound from file
		if (FAILED(m_pLoader->LoadObjectFromFile(
			CLSID_DirectMusicSegment, // Class identifier.
			IID_IDirectMusicSegment8, // ID of desired interface.
			wstrFile,     // Filename.
			(LPVOID*) &pSound   // Pointer that receives interface.
			)))
		{
			throw "Sound Error: Could not find file.";
		}
		pSound->Download( m_pPerformance );
		sounds[ file ] = pSound;
		soundfile[ pSound ] = file;
		soundref[ pSound ] = 1;
		return pSound;
	} else {
		soundref[i->second]++;
		return i->second;
	}
#else
	return NULL;
#endif
}

void DXSystem::releaseSound( SOUND_REF snd )
{
#ifdef __WITH_SOUND
	if ( snd ){
		soundref[snd]--;
		if ( soundref[snd] < 1 ){
			sounds.erase( soundfile[snd] );
			soundfile.erase( snd );
			((SOUND_REF)snd)->Release();
		}
	}
#endif
}

BOOL DXSystem::recover(){
	switch(pd3dDevice->TestCooperativeLevel()){
	case D3DERR_DEVICELOST:
		onLostDevice();
		d3dpp.Windowed							= TRUE;
		d3dpp.BackBufferWidth					= 0;
		d3dpp.BackBufferHeight					= 0;
		d3dpp.FullScreen_PresentationInterval	= 0;
		return FALSE;
	case D3DERR_DEVICENOTRESET:
	case D3D_OK:
		DXERROR(pd3dDevice->Reset(&d3dpp));
		onResetDevice();
		//At this time, the screen needs a redraw
		//render();
 		return TRUE;
	default:
		error("Unknown return value from TestCooperativeLevel (__FILE__:__LINE__)");
		return FALSE;
	}
}

void DXSystem::onLostDevice()
{

}
void DXSystem::onResetDevice()
{

}


BOOL DXSystem::isFullScreen()
{
	return !d3dpp.Windowed;
}


void DXSystem::setFullScreen(BOOL fullscreen){

	//When a window comes out of fullscreen mode, the proper z-order does not seem
	//to get reset by DirectX so we do it ourselves.
	if(!fullscreen)
		SetWindowPos(hWindow, HWND_NOTOPMOST, 0, 0, WIDTH, HEIGHT, 0);//SWP_NOMOVE | SWP_NOSIZE);

	//get current display mode
	D3DDISPLAYMODE d3ddm;
	DXERROR(pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm));

	//Set up d3d params based on display mode and our needs
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow						= hWindow;
	d3dpp.Windowed							= !fullscreen;
	d3dpp.BackBufferWidth					= (d3dpp.Windowed? 0 : 800);
	d3dpp.BackBufferHeight					= (d3dpp.Windowed? 0 : 600);
	d3dpp.SwapEffect						= D3DSWAPEFFECT_COPY_VSYNC;
	d3dpp.BackBufferFormat					= d3ddm.Format;
	d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT ;
	d3dpp.FullScreen_PresentationInterval	= (d3dpp.Windowed? 0 : D3DPRESENT_INTERVAL_IMMEDIATE );
	d3dpp.BackBufferCount					= 1;
	d3dpp.Flags								= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.EnableAutoDepthStencil			= TRUE;
	d3dpp.AutoDepthStencilFormat			= D3DFMT_D16;

	//Cause an artificial lost device first, then recover to new settings
	if(pd3dDevice){
		onLostDevice();
		recover();	
	}

}

void DXSystem::dxerror(LPSTR file, DWORD line, HRESULT err){
	switch(err){
	case D3DERR_DEVICELOST:
		if(!recover())
			devLost = TRUE;
		break;
	case DIERR_INPUTLOST:
	case DIERR_NOTACQUIRED:
		recoverInput();
		break;
	case DIERR_OTHERAPPHASPRIO:
		//Nothing can be done but wait until we can reacquire
		break;
	default:
		ShowCursor(TRUE);
		ShowWindow(hWindow, FALSE);
		DXTrace(file, line, err, (char*)DXGetErrorDescription8(err), TRUE);
		exit(-1);
	}
}

//Error window function
void DXSystem::error(LPCSTR ErrorText)
{
	MessageBox(hWindow, ErrorText, "APTITLE - Error", MB_OK | MB_ICONERROR);
	exit(-1);
}




//////////////////////////////////////////////////////////////////////
// Input functions
//////////////////////////////////////////////////////////////////////
void DXSystem::inputConf(int argc, char** argv){
	int fs = isFullScreen();
	if (fs)
		setFullScreen(false);

	DICOLORSET dics;
    ZeroMemory(&dics, sizeof(DICOLORSET));
    dics.dwSize = sizeof(DICOLORSET);

	DICONFIGUREDEVICESPARAMS dicdp;
    ZeroMemory(&dicdp, sizeof(dicdp));
    dicdp.dwSize = sizeof(dicdp);
    dicdp.dwcUsers       = 1;
    dicdp.lptszUserNames = NULL;
    dicdp.dwcFormats     = 1;
    dicdp.lprgFormats    = &diaf;
    dicdp.hwnd           = hWindow;
    dicdp.lpUnkDDSTarget = NULL;
	dicdp.dics			 = dics;

	ShowCursor(TRUE);
	ShowWindow(hWindow, SW_HIDE);
	releaseInput();
	DXERROR(pdi8->ConfigureDevices(NULL, &dicdp, DICD_EDIT, NULL));

	//Restart input system for changes to take effect
	DXERROR(pdi8->EnumDevicesBySemantics(NULL, &diaf, DIEnumDevicesBySemanticsCallback, this, 0));

	ShowCursor(FALSE);
	ShowWindow(hWindow, SW_RESTORE);
	if (fs)
		setFullScreen(true);
}

BOOL CALLBACK DXSystem::DIEnumDevicesBySemanticsCallback(
  LPCDIDEVICEINSTANCE	lpddi,  
  LPDIRECTINPUTDEVICE8	lpdid, 
  DWORD					dwFlags, 
  DWORD					dwRemaining, 
  LPVOID				pvRef){
	if(pvRef){
		DXSystem	*dxs = (DXSystem*)pvRef;

		lpdid->Unacquire();	//Never hurts
		//Program gets uninterupted input in the foreground, windows key disabled
		DXERROR(lpdid->SetCooperativeLevel(dxs->hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY));

		//Add to array of input devices
		//WARNING: If devices are re-enumerated before the existing array of input devices is cleared
		//there will be a memory leak.
		if(!dxs->numInputDevices){
			dxs->inputDevices = new InputDevice[dwRemaining+1];
			ZeroMemory(dxs->inputDevices, (dwRemaining+1)*sizeof(InputDevice));
		}
		dxs->inputDevices[dxs->numInputDevices].pdidDevice = lpdid;
		dxs->inputDevices[dxs->numInputDevices].pdidDevice->AddRef();
		dxs->inputDevices[dxs->numInputDevices].pbState = new BOOL[dxs->diaf.dwNumActions];
		dxs->inputDevices[dxs->numInputDevices].dwDevType = lpddi->dwDevType;
		ZeroMemory(dxs->inputDevices[dxs->numInputDevices].pbState, dxs->diaf.dwNumActions*sizeof(BOOL));
		
		HRESULT	hRes;
		if(FAILED(hRes = lpdid->BuildActionMap(&dxs->diaf, NULL, 0))){
			dxs->dxerror(__FILE__,__LINE__, hRes);
		} else{
			//DI_NOEFFECT is a success code that means no keys are assigned to a device
			dxs->inputDevices[dxs->numInputDevices].inUse = (hRes != DI_NOEFFECT);
		}
		
		DXERROR(lpdid->SetActionMap(&dxs->diaf, NULL, 0));

		dxs->numInputDevices++;
		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

void DXSystem::recoverInput()
{
	for(int i = 0; i < numInputDevices; i++){
		inputDevices[i].pdidDevice->Unacquire();
		//Attempting to a acquire a device with no keys is an error
		if(inputDevices[i].inUse)
			DXERROR(inputDevices[i].pdidDevice->Acquire());
	}
}

void DXSystem::releaseInput()
{
	for(int i = 0; i < numInputDevices; i++){
		inputDevices[i].pdidDevice->Unacquire();
		inputDevices[i].pdidDevice->Release();
		delete inputDevices[i].pbState;
	}
	numInputDevices = 0;
	delete inputDevices;
}

void DXSystem::processInput()
{
	DIDEVICEOBJECTDATA	didod[INPUT_BUFSIZE];
	HRESULT				hRes;
	DWORD				dwObjCount;
	int					i,j;

	for(i = 0; i < numInputDevices; i++){
		dwObjCount = INPUT_BUFSIZE;

		//Skip input devices that have no keys set
		if(!inputDevices[i].inUse)
			continue;

		if(FAILED(hRes = inputDevices[i].pdidDevice->Poll())){
			dxerror(__FILE__,__LINE__, hRes);
		}
		else{
			//Retrieve all data in the buffer
			inputDevices[i].pdidDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwObjCount, 0);
			
			if(dwObjCount){
				for(j = 0; j < dwObjCount; j++)
					inputDevices[i].pbState[didod[j].uAppData] = didod[j].dwData & 0x80;
			}
		}
	}
			
	//Combine data from all input devices into something the game cares about
	ZeroMemory(actionState, diaf.dwNumActions*sizeof(BOOL));
	for(i = 0; i < diaf.dwNumActions; i++){
		for(j = 0; j < numInputDevices; j++){
			if(inputDevices[j].pbState[i])
				actionState[i] = TRUE;
		}
	}
}

void DXSystem::quit(int argc, char** argv){
	PostQuitMessage(0);
}

void DXSystem::setFullScreen(int argc, char** argv){
	if ( (argc > 1) && !strcmp( argv[1], "true" ) ){
		setFullScreen( true );
	} else if ( (argc > 1) && !strcmp( argv[1], "false" ) ){
		setFullScreen( false );
	} else {
		console->cprintf("Usage: fullscreen [true|false]");
	}
}

void DXSystem::mute(int argc, char** argv){
	if ( (argc > 1) && !strcmp( argv[1], "true" ) ){
		mute( true );
	} else if ( (argc > 1) && !strcmp( argv[1], "false" ) ){
		mute( false );
	} else {
		//Yeah, this is bad.
		m_bMute = !m_bMute;
	}
}

void DXSystem::wire(int argc, char* argv){
	static int wire = 0;
	wire = ~wire;
	if (wire)
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void DXSystem::inputCB(BOOL* input){
	
}

//////////////////////////////////////////////////////////////////////
//     Window handler function
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WinProc (HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
	DXSystem *dxs = (DXSystem*)GetWindowLong( hWindow, GWL_USERDATA );
	POINT pt = {-1,-1};

	if(!dxs){
		return DefWindowProc(hWindow, message, wParam, lParam);
	}


	//Attempt to recover surfaces
	if(dxs->devLost && message != WM_DESTROY){
		if(dxs->recover()){
			dxs->devLost = FALSE;
		}
	}

	switch (message) 
	{
	case WM_CLOSE:
		//Ignore close message.
		(dxs->console != NULL )? dxs->console->cprintf("Use quit to leave program.") : 0;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		//dxs->render();
        return 0;
	case WM_CHAR:
		(dxs->console != NULL )? dxs->console->charInput(wParam) : 0 ;
		return 0;
	case WM_KEYDOWN:
		(dxs->console != NULL )? dxs->console->keyEvent(wParam) : 0 ;
		return 0;
	case WM_SETCURSOR:	
		GetCursorPos(&pt);
		ScreenToClient(hWindow, &pt);
	
		if (!dxs->isFullScreen() && ((pt.y < 0) || (pt.x < 0))){
			SetCursor(LoadCursor(0, IDC_ARROW));
			return true;
		}else{
			SetCursor(NULL);
			return false;
		}
		SetCursor(NULL);
		dxs->pd3dDevice->ShowCursor(TRUE);

		break;
	default:
		return DefWindowProc(hWindow, message, wParam, lParam);
	}	
}

