// Number.h: interface for the Number class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMBER_H__19AFB377_4E07_4585_90C7_F0DAAC906A4F__INCLUDED_)
#define AFX_NUMBER_H__19AFB377_4E07_4585_90C7_F0DAAC906A4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"

class Number : public Renderable  
{
public:
	Number(DXSystem* dxs);
	virtual ~Number();
	void render(Context *c);
	void draw(int n);
	void draw(){};
	void setValue(int v);
	void addValue(int v){ setValue(value+v); };
	int getValue(){ return value; };
protected:
	void init();

	int value;
	static IDirect3DVertexBuffer8	*polys;
	static LPDIRECT3DTEXTURE8 pTex;
	static int refCount;
	static D3DXMATRIX s_mR1;
	std::vector<int> digits;

};

#endif // !defined(AFX_NUMBER_H__19AFB377_4E07_4585_90C7_F0DAAC906A4F__INCLUDED_)
