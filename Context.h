// Context.h: interface for the Context class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXT_H__18E5576E_C16D_4C27_BA67_91E96E787DFD__INCLUDED_)
#define AFX_CONTEXT_H__18E5576E_C16D_4C27_BA67_91E96E787DFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Camera.h"
class Cell;

typedef struct {
    float top;	// The position for the vertex.
	float bottom;
	float left;
	float right;
} FRECT;

class Context  
{
public:
	Context();
	virtual ~Context();
	virtual apply( IDirect3DDevice8* dev );
	virtual SetTransform( D3DTRANSFORMSTATETYPE , const D3DXMATRIX* );
	virtual D3DXMATRIX* transform();
	virtual D3DXMATRIX* worldView();
	ID3DXMatrixStack *m_mspWorld, *m_mspView, *m_mspProjection;
	D3DXMATRIX m_mTransform;
	Camera* cam;
};

#endif // !defined(AFX_CONTEXT_H__18E5576E_C16D_4C27_BA67_91E96E787DFD__INCLUDED_)
