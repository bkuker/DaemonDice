// Console.cpp: implementation of the Console class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Console.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Console::Console(DXSystem* dxs):Renderable(dxs)
{
	//Console Setup
	position = 0;
	desiredPosition = 0;
	numLines = 0;
	consoleHead = consoleFoot = consoleCurrent = NULL;
	commandToBind = "";
	bindKey = 0;
	showInput = true;
	inputLine[0] = 0;
	prompt[0] = 0;
	cLock = false;

	

	//Attach some console callbacks
	ch = new Console_helper(this);
	registerConsoleCommand("help", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::help));
	registerConsoleCommand("?", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::help));
	registerConsoleCommand("bind", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::bindCommand));
	registerConsoleCommand("alias", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::aliasCommand));
	registerConsoleCommand("clear", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::clearConsole));
	registerConsoleCommand("console", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::setConsoleStatus));

	D3DXCreateFont(pd3dDevice, (HFONT)GetStockObject(ANSI_FIXED_FONT), &pdxfDefFont);

	//Console polys
	pd3dDevice->CreateVertexBuffer( 4*sizeof(PNCTVertex), 0, FVF_PNCT, D3DPOOL_MANAGED, &conpolys);
	PNCTVertex* pV;
	conpolys->Lock(0, 0, (BYTE**)&pV,0);
	for ( int i = 0; i < 4; i++){
		pV[i].c = D3DCOLOR_XRGB(0,0,0);
		pV[i].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f);
	}


	pV[0].p = D3DXVECTOR3(0,0,0);
	pV[1].p = D3DXVECTOR3(0,1,0);
	pV[2].p = D3DXVECTOR3(1,0,0);
	pV[3].p = D3DXVECTOR3(1,1,0);

	pV[0].t = D3DXVECTOR2(0,0);
	pV[1].t = D3DXVECTOR2(0,1);
	pV[2].t = D3DXVECTOR2(1,0);
	pV[3].t = D3DXVECTOR2(1,1);
	conpolys->Unlock();
	contex = NULL;
}

Console::~Console()
{
	if (contex != NULL)
		dxs->releaseTexture( contex );

	if(pdxfDefFont != NULL)
		pdxfDefFont->Release();

	conpolys->Release();
	delete ch;
}

void Console::onLostDevice(){
	pdxfDefFont->OnLostDevice();
}

void Console::onResetDevice(){
	pdxfDefFont->OnResetDevice();
}

void Console::draw(){
	//Setup prompt
	int fps = 1000 / dxs->getFrameTime();
	sprintf( prompt, "%i fps ", fps );
	if (position < desiredPosition ){
		position+=7;
		if (position > desiredPosition)
			position = desiredPosition;
	} else if (position > desiredPosition){
		position-=7;
		if (position < desiredPosition)
			position = desiredPosition;
	}
	
	char			text[CONSOLE_WIDTH+3];
	translate(0, (((float)position + 20) / HEIGHT)-1.0f, 0);
	//render the console if needed
	if(position > 0){
		

		if ( contex == NULL ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		}else{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
			pd3dDevice->SetTexture( 0, contex );
		}
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pd3dDevice->SetVertexShader(FVF_PNCT);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetStreamSource(0, conpolys, sizeof(PNCTVertex));
		DXERROR(pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
		
		//Fix renderstates
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pd3dDevice->SetTexture( 0, NULL );
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);


		RECT			lineRect = {10, position - 15, 0, 0};
		CONSOLE_LINE	*line;
		
		
		
		//print the input line
		if (showInput){
			sprintf(text,"%s> %s_", prompt, inputLine);
			pdxfDefFont->DrawText(text, -1, &lineRect, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
		}
		
		//work up screen printing the rest
		lineRect.top -= 15;
		line = consoleCurrent;
		while(line && lineRect.top >= 0){
			pdxfDefFont->DrawText(line->text, -1, &lineRect, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
			lineRect.top -= 15;
			line = line->next;
		}
		
	} else {//Show the last line of text
		RECT	lineRect = {0, 0, WIDTH, position};
		sprintf(text,"%s: %s", prompt, consoleHead->text);
		pdxfDefFont->DrawText(text, -1, &lineRect, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
	}
}

void Console::setTexture( LPDIRECT3DTEXTURE8 tex ){
	contex = tex;
}
//////////////////////////////////////////////////////////////////////
// Console functions
//////////////////////////////////////////////////////////////////////
void Console::registerConsoleCommand( std::string s, CONSOLE_CALLBACK_OBJECT o, CONSOLE_CALLBACK_FUNCTION f){
	CONSOLE_CALLBACK cc;
	cc.func = f;
	cc.object = o;
	consoleMap[s] = cc;
}

void Console::registerConsoleCommand( std::string s, STATIC_CONSOLE_CALLBACK_FUNCTION f){
	CONSOLE_CALLBACK cc;
	cc.sfunc = f;
	cc.object = NULL;
	consoleMap[s] = cc;
}

void Console::unregisterConsoleCommand( std::string s ){
	consoleMap.erase(s);
}

#define MAX_ARGS	10
void Console::processConsoleLine(const char *cmd){
	int		argc;
	char	*argv[MAX_ARGS];
	char	*command, *cmem;
	
	if(!cmd)
		return;

	command = cmem = _strdup(cmd);

	//chop off whitespace and quotes
	int qcount = 0;
	while( *command == ' ' || *command == '"'){
		if (*command == '"')
			qcount++;
		command++;
	}
	while( qcount && (*(command + strlen(command)-1) == ' ' || *(command + strlen(command)-1) == '"')){
		if ( *(command + strlen(command)-1) == '"' ){
			qcount--;
		}
		*(command + strlen(command)-1) = NULL;
	}

	//chop into args, honoring quotes
	argc = 1;
	argv[0] = command;
	bool quote = false;
	int len=strlen(command);
	for (int i = 0; i < len; i++){
		if (command[i] == '"'){
			quote = !quote;
		}
		if ( command[i] == ';' && !quote){
			command[i] = NULL;
			if (command[i+1] != ';' && argc < MAX_ARGS){
				argv[argc] = command + i + 1;
				argc++;
			}
		}
	}
	if (quote){
		cprintf("Error: Close your quotes: %s", command);
		return;
	}

	for ( i = 0; i < argc; i++){
		processConsoleCmd(argv[i]);
	}

	free(cmem);
}


void Console::processConsoleCmd(char *command)
{
#define MAX_DEPTH 10


	int		argc;
	char	*argv[MAX_ARGS];
	
	if(!command)
		return;


	argc = 1;
	argv[0] = strtok(command, " ");

	while(argv[argc-1] && argc <= MAX_ARGS){
		argv[argc++] = strtok(NULL, " ");
	}
	argc--;

	if (!argv[0])
		return;
	//Next process the input

	//check for an alias
	//check our alias recursion depth
	static int depth = 0;
	depth++;
	if ( depth >= MAX_DEPTH ){
		cprintf("Exceeded alias depth");
		depth = 0;
		return;
	}
	std::map<std::string, std::string>::iterator aliasItr;
	aliasItr = consoleAliases.find(argv[0]);
	if ( aliasItr != consoleAliases.end() ){
		std::string cmd = aliasItr->second;
		int len = cmd.length();
		char *copy = new char[len+1];
		strcpy( copy, cmd.c_str() );
		processConsoleLine( copy );
		delete copy;
		return;
	}
	depth--;

	//check for a callback and call it if it exists
	std::map<std::string, CONSOLE_CALLBACK>::iterator callbackPair;
	callbackPair = consoleMap.find(argv[0]);
	if ( callbackPair == consoleMap.end() ){
		//if there is no specific callback check for a default
		cprintf("No '%s' console callback registered", argv[0]);
		callbackPair = consoleMap.find(DEFAULT_CALLBACK);
	}
	//check to see if we have found a callback, if so call it
	if ( callbackPair != consoleMap.end()){
		CONSOLE_CALLBACK cc = callbackPair->second;
		if( cc.object && cc.func ){
			(*(cc.object).*(cc.func))(argc, (char**)argv);
		}
		else if ( cc.object == NULL && cc.sfunc ){
			(cc.sfunc)(argc, (char**)argv);
		}
	} else {
		cprintf("No default console callback registered");
	}
}

void Console::keyEvent(WPARAM dir)
{	
	if ( bindKey ){
		consoleBindings[dir] = commandToBind;
		

	} else if (!position) {
		std::map<WPARAM, std::string>::iterator bindItr;
		bindItr = consoleBindings.find(dir);
		if ( bindItr != consoleBindings.end() ){
			std::string cmd = bindItr->second;
			int len = cmd.length();
			char *copy = new char[len+1];
			strcpy( copy, cmd.c_str() );
			processConsoleLine( copy );
			delete copy;
		}

	} else {
		switch(dir){
		case VK_PRIOR:
			if ( consoleCurrent != consoleFoot )
				consoleCurrent = consoleCurrent->next;
			break;
		case VK_NEXT:
			if ( consoleCurrent != consoleHead )
				consoleCurrent = consoleCurrent->prev;
			break;
		case VK_HOME:
			consoleCurrent = consoleFoot;
			break;
		case VK_END:
			consoleCurrent = consoleHead;
			break;
		}
	}
	if (dir == VK_F11 && !cLock){
		setConsolePosition( desiredPosition?CONSOLE_HIDDEN:CONSOLE_HALF );
	}

}

void Console::charInput(WPARAM input)
{
	//If the console is out process the input console wise
	if ( desiredPosition  && !bindKey ){
		int	len = strlen(inputLine);
		switch(input){
		case '\b':
			if(len)
				inputLine[len-1] = 0;
			break;
		case '\r':
			//User pressed enter. Process command
			if(len){
				showInput = false;
				cprintf("%s", inputLine);
				processConsoleLine(inputLine);
				inputLine[0] = 0;
				showInput = true;
			}  else {
				cprintf("");
			}
			break;
		case '\t':
			break;
		default:
			if(len < CONSOLE_WIDTH){
				inputLine[len] = (char)input;
				inputLine[len+1] = 0;
			}
		}
	}else {
		bindKey = 0;
	}
	
}

void Console::setConsolePosition(int pos){
	switch (pos){
	case CONSOLE_HIDDEN:
		desiredPosition = 0;
		break;
	case CONSOLE_HALF:
		desiredPosition = HEIGHT / 2;
		break;
	case CONSOLE_FULL:
		desiredPosition = HEIGHT - 25;
		break;
	default:
		desiredPosition = pos;
	}
}

void Console::lockConsole(BOOL l)
{
	cLock = l;
	if(l)
		position = desiredPosition;
}

void Console::cprintf(const char *format, ...){
	va_list argptr;
	va_start(argptr, format);
	vcprintf(format, argptr);
}

void Console::vcprintf(const char *format, va_list argptr){
	struct	CONSOLE_LINE *line;
	line = new CONSOLE_LINE;

	_vsnprintf(line->text, CONSOLE_WIDTH, format, argptr);
	
	line->text[CONSOLE_WIDTH] = 0;
	line->next = consoleHead;
	line->prev = NULL;
	if (consoleHead)
		consoleHead->prev = line;
	consoleHead = line;
	consoleCurrent = consoleHead;

	if(!consoleFoot)
		consoleFoot = line;
	
	if(numLines == CONSOLE_LINES){
		consoleFoot = consoleFoot->prev;
		delete consoleFoot->next;
		consoleFoot->next = NULL;

	}else{
		numLines++;
	}
}

void Console::setConsoleStatus( int argc, char** argv){
	if ( (argc > 1) && !strcmp( argv[1], "full" ) ){
		setConsolePosition(CONSOLE_FULL);
	} else if ( (argc > 1) && !strcmp( argv[1], "half" ) ){
		setConsolePosition(CONSOLE_HALF);
	} else if ( (argc > 1) && !strcmp( argv[1], "hidden" ) ){
		setConsolePosition(CONSOLE_HIDDEN);
	} else {
		cprintf("Usage: console [full|hidden|half]");
	}
}

void Console::clearConsole( int argc, char** argv ){
	//Free console memory
	CONSOLE_LINE	*line, *next;
	line = consoleHead;
	while(line){
		next = line->next;
		delete line;
		line = next;
	}
	consoleHead = consoleFoot = consoleCurrent = NULL;
	numLines = 0;
}

void Console::aliasCommand(int argc, char** argv){
	if (argc < 2){
		cprintf("Usage: alias name [command]");
	} else if (argc == 2){
		cprintf("Unaliased %s", argv[1]);
		consoleAliases.erase(argv[1]);
	} else {
		//make into a single string
		std::string cmd;
		for (int i = 2; i < (argc - 1); i++){
			cmd += argv[i];
			cmd += " ";
		}
		cmd += argv[i];
		cprintf("Aliased %s to \"%s\"", argv[1], cmd.c_str());
		consoleAliases[argv[1]] = cmd;
	}
}

void Console::bindCommand(int argc, char** argv){
	if (argc == 1){
		cprintf("Unbind key [press key]");
		consoleAliases.erase(argv[1]);
	} else {
		//make into a single string
		std::string cmd;
		for (int i = 1; i < (argc - 1); i++){
			cmd += argv[i];
			cmd += " ";
		}
		cmd += argv[i];
		cprintf("Binding \"%s\" [press key]", cmd.c_str());
		commandToBind = cmd;
		bindKey = 1;
	}
}

void Console::help( int argc,  char** argv){
	std::map<std::string, CONSOLE_CALLBACK>::iterator ci;
	cprintf("Available console commands:");
	for(ci = consoleMap.begin(); ci != consoleMap.end(); ci++){
		cprintf("    %s", ci->first.c_str() );
	}
	
}

////////////////
//Console Helper

void Console_helper::setConsoleStatus( int argc, char** argv)
{
	c->setConsoleStatus(argc, argv);
}

void Console_helper::clearConsole(int argc, char** argv)
{
	c->clearConsole(argc, argv);
}

void Console_helper::aliasCommand(int argc, char** argv)
{
	c->aliasCommand(argc, argv);
}

void Console_helper::bindCommand(int argc, char** argv)
{
	c->bindCommand(argc, argv);
}

void Console_helper::help(int argc, char** argv)
{
	c->help(argc, argv);
}
