// Die.h: interface for the Die class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Die_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_)
#define AFX_Die_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"
#include "Moveable.h"
class Context;

#ifndef __CELL_DIR__
#define __CELL_DIR__ 
enum CELL_DIR {NORTH, SOUTH, EAST, WEST, NONE}; 
#endif

class Die : 
	public Renderable
{
public:
	Die(DXSystem* dxs);
	void roll(CELL_DIR, float);
	void infoRoll(CELL_DIR, float);
	void slide(CELL_DIR, float);
	void rise(float);
	void rotate(CELL_DIR);
	virtual ~Die();
	void draw();

private:
	void init();
	LPDIRECT3DTEXTURE8 pTex1, pTex2;
	LPDIRECT3DTEXTURE8 pTex;
	static IDirect3DVertexBuffer8	*s_pvPoints;
	static int refCount;
	static D3DXMATRIX s_mShrink;

};

#endif // !defined(AFX_Die_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_)
