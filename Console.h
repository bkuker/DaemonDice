// Console.h: interface for the Console class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_)
#define AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class Console;
#include "Renderable.h"
#include "DXSystem.h"


//A class and typedef for console callbacks
#define DEFAULT_CALLBACK "__default_callback_string__"
class CONSOLE_CALLBACK_CLASS {};
typedef CONSOLE_CALLBACK_CLASS* CONSOLE_CALLBACK_OBJECT;
typedef void (CONSOLE_CALLBACK_CLASS::*CONSOLE_CALLBACK_FUNCTION)(int, char**);
typedef void (*STATIC_CONSOLE_CALLBACK_FUNCTION)(int, char**);
struct CONSOLE_CALLBACK{
	CONSOLE_CALLBACK_OBJECT object;
	union{
		CONSOLE_CALLBACK_FUNCTION func;
		STATIC_CONSOLE_CALLBACK_FUNCTION sfunc;
	};
};


//console defines
#define CONSOLE_WIDTH	75
#define CONSOLE_LINES	100
#define CONSOLE_HIDDEN 0
#define CONSOLE_HALF -1
#define CONSOLE_FULL -2

struct CONSOLE_LINE{
	char	text[CONSOLE_WIDTH+1];
	struct	CONSOLE_LINE *next, *prev;
};

class Console;

class Console_helper
{
public:
	Console_helper(Console* _c){ c = _c; };
	void processConsoleCmd(char *);
	void setConsoleStatus( int argc, char** argv);
	void clearConsole(int argc = 0, char** argv = 0);
	void aliasCommand(int argc, char** argv);
	void bindCommand(int argc, char** argv);
	void help(int argc = 0, char** argv = 0);
	Console* c;
};

class Console  : public Renderable  
{
friend DXSystem;
friend Console_helper;
public:
	Console(DXSystem* dxs);
	virtual ~Console();

//Console functions
public:
	void registerConsoleCommand( std::string, CONSOLE_CALLBACK_OBJECT, CONSOLE_CALLBACK_FUNCTION );
	void registerConsoleCommand( std::string, STATIC_CONSOLE_CALLBACK_FUNCTION );
	void unregisterConsoleCommand( std::string );
	void processConsoleLine(const char *);
	void setConsolePosition(int pos);
	void cprintf(const char *, ...);
	void vcprintf(const char *, va_list argptr);
	void lockConsole(BOOL);
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void draw();
	void setTexture( LPDIRECT3DTEXTURE8 tex );

//Key input functions
//These are public because of the windows event loop
//Do not rely on them.
public:
	void keyEvent(WPARAM dir);
	void charInput(WPARAM input);

//Private functions
private:
	void processConsoleCmd(char *);
	void setConsoleStatus( int argc, char** argv);
	void clearConsole(int argc = 0, char** argv = 0);
	void aliasCommand(int argc, char** argv);
	void bindCommand(int argc, char** argv);
	void help(int argc = 0, char** argv = 0);

//Console vars
private:
	std::map<std::string, CONSOLE_CALLBACK> consoleMap;
	std::map<std::string, std::string> consoleAliases;
	std::map<WPARAM, std::string> consoleBindings;
	char	inputLine[CONSOLE_WIDTH];
	char prompt[25];
	BOOL showInput, cLock;
	CONSOLE_LINE *consoleHead, *consoleFoot, *consoleCurrent;
	int	numLines, position, desiredPosition;
	std::string commandToBind;
	int bindKey;

	D3DXMATRIX				matConsView;
	IDirect3DVertexBuffer8	*conpolys;
	LPDIRECT3DTEXTURE8		contex;
	LPD3DXFONT				pdxfDefFont;
	Console_helper* ch;

};

#endif // !defined(AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_)
