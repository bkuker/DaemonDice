//////////////////////////////////////////////////////////////////////
// DXSystem.h: interface for the DXSystem class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXSYSTEM_H__B8E43DE9_86A5_447E_987E_35B9274F3659__INCLUDED_)
#define AFX_DXSYSTEM_H__B8E43DE9_86A5_447E_987E_35B9274F3659__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DXSystem;

#define WIDTH			640
#define HEIGHT			480


#include "Console.h"
#define INITGUID
#include <dmusici.h>

//A nice Direct X Error macro
#define DXERROR(fcn) \
{ HRESULT hRes;\
	if (FAILED(hRes = fcn)) \
	{ \
		dxs->dxerror(__FILE__, __LINE__, hRes); \
	} \
}

//Input Typedefs
typedef void (*INPUTPARAMCB)(int &, GUID &, DWORD &, DIACTION* &, TCHAR name[MAX_PATH]);
typedef struct {
	LPDIRECTINPUTDEVICE8	pdidDevice;
	BOOL*					pbState;
	DWORD					dwDevType;
	BOOL					inUse;
} InputDevice;


//Sound Typedefs
typedef IDirectMusicSegment8* SOUND_REF;

class DXSystem  
{
friend LRESULT CALLBACK WinProc (HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam);
//Constructors / destructors
public:
	DXSystem(char*, HINSTANCE, INPUTPARAMCB);
	virtual ~DXSystem();

//Public functions
public:
	void run();
	virtual void render();
	virtual void drawScene();
	BOOL recover();
	BOOL isFullScreen();
	void setFullScreen(BOOL fullscreen);
	static BOOL CALLBACK DIEnumDevicesBySemanticsCallback(LPCDIDEVICEINSTANCE, LPDIRECTINPUTDEVICE8, DWORD,	DWORD, LPVOID);
	int getFrameTime();

//Public Common Input Callbacks	
public:
	virtual void wire(int, char*);
	void quit(int argc, char** argv);
	void inputConf(int argc = 0, char** argv = 0);
	void setFullScreen(int argc, char** argv);
	void mute(int argc, char** argv);

//Public Texture functions
public:
	LPDIRECT3DTEXTURE8 loadTexture( std::string file );
	void releaseTexture( LPDIRECT3DTEXTURE8 );

//Public Sound functions
public:
	SOUND_REF loadSound( std::string file );
	void releaseSound( SOUND_REF );
	void playSound( SOUND_REF );
	void mute(bool);

	
//Public Debug / Error functions
public:
	void debugprintf(const char *format, ...);
	void error(LPCSTR ErrorText);
	void dxerror(LPSTR file, DWORD line, HRESULT err);

//Public data members
public:
	BOOL devLost;
	IDirect3DDevice8*		pd3dDevice;
	Console*				console;

/////////////////////////Private

//Private methods
protected:
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void inputCB(BOOL* input);
	void recoverInput();
	void releaseInput();
	void processInput();
	

//Private variables
protected:
	HWND					hWindow;
	IDirect3D8*				pd3d;	
	D3DPRESENT_PARAMETERS	d3dpp;
	DXSystem*				dxs;	//=this Used in DXERROR macro
	INT64					hpcFreq;
	int						elapsedTime;


//texture variables
protected:
	std::map< std::string, LPDIRECT3DTEXTURE8 > textures;
	std::map< LPDIRECT3DTEXTURE8, int > texref;
	std::map< LPDIRECT3DTEXTURE8, std::string > texfile;
	
//sound variables
protected:
	std::map< std::string, SOUND_REF > sounds;
	std::map< SOUND_REF, int > soundref;
	std::map< SOUND_REF, std::string > soundfile; 
	IDirectMusicLoader8*  m_pLoader;
	IDirectMusicPerformance8* m_pPerformance;
	bool m_bMute;

//Input vars
protected:
	int			numInputDevices;
	InputDevice *inputDevices;
	BOOL		*actionState;
	LPDIRECTINPUT8			pdi8;
	LPDIRECTINPUTDEVICE8	pdiKeyboard;
	DIACTIONFORMAT			diaf;

};
#endif // !defined(AFX_DXSYSTEM_H__B8E43DE9_86A5_447E_987E_35B9274F3659__INCLUDED_)
