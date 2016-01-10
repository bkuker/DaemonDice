// Lightning.h: interLightning for the Lightning class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Lightning_H__53BFFBCB_4143_409A_9D70_FC1DA9BCA003__INCLUDED_)
#define AFX_Lightning_H__53BFFBCB_4143_409A_9D70_FC1DA9BCA003__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Animated.h"
#include "Moveable.h"
#include "Renderable.h"

class Lightning : public Animated, public Renderable 
{
public:
public:
	Lightning(DXSystem* dxs);
	virtual ~Lightning();
	void tick(int milliseconds);
	void draw();
	D3DXMATRIX tm;

private:
	void init();
	static LPDIRECT3DTEXTURE8 pTex;
	static IDirect3DVertexBuffer8	*s_pvPoints;
	static int refCount;
	int time;
};

#endif // !defined(AFX_Lightning_H__53BFFBCB_4143_409A_9D70_FC1DA9BCA003__INCLUDED_)
