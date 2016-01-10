// Renderable.h: interface for the Renderable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERABLE_H__FA93B7A4_8550_4DC3_B1E6_5D94B3319DB2__INCLUDED_)
#define AFX_RENDERABLE_H__FA93B7A4_8550_4DC3_B1E6_5D94B3319DB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Moveable.h"
class Context;

typedef struct {
    D3DXVECTOR3	p;	// The position for the vertex.
	D3DXVECTOR3	n;
    DWORD		c;		// The vertex color.
} PNCVertex;
#define FVF_PNC (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

typedef struct {
    D3DXVECTOR3	p;	// The position for the vertex.
	D3DXVECTOR3	n;
	DWORD		c;		// The vertex color.
	D3DXVECTOR2	t;
} PNCTVertex;
#define FVF_PNCT (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)//|D3DFVF_TEXCOORDSIZE2(0))

typedef struct {
    D3DXVECTOR3	p;	// The position for the vertex.
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
} PNTVertex;
#define FVF_PNT (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)//|D3DFVF_TEXCOORDSIZE2(0))

class Renderable;
class DXSystem;

class Renderable : virtual public Moveable
{
public:
	Renderable(DXSystem* dxs);
	virtual ~Renderable();
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void render( Context* c );
	virtual void draw() = 0;

protected:
	DXSystem* dxs;
	IDirect3DDevice8*	pd3dDevice;
	static D3DMATERIAL8 mtrl;
};

#endif // !defined(AFX_RENDERABLE_H__FA93B7A4_8550_4DC3_B1E6_5D94B3319DB2__INCLUDED_)
