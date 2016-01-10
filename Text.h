// Text.h: interface for the Text class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXT_H__FB5600CE_F961_484D_BB03_788FCA4BF106__INCLUDED_)
#define AFX_TEXT_H__FB5600CE_F961_484D_BB03_788FCA4BF106__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"
#include "Banner.h"
#define MAX_TEXT 255

enum JUSTIFICATION {LEFT, RIGHT, CENTER};

class Text : public Renderable  
{
public:
	Text(DXSystem* dxs);
	virtual ~Text();
	void render(Context* c);
	void draw(){};
	void printf(const char *, ...);
	void vprintf(const char *, va_list argptr);
	void justify(JUSTIFICATION _j){ j = _j; };

protected:
	char text[MAX_TEXT];
	static D3DXMATRIX s_mR1, s_mL1;
	JUSTIFICATION j;
	static Banner* b;
	static int refCount;
};

#endif // !defined(AFX_TEXT_H__FB5600CE_F961_484D_BB03_788FCA4BF106__INCLUDED_)
