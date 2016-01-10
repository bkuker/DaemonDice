// Moveable.cpp: implementation of the Moveable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Moveable.h"


D3DXMATRIX Moveable::s_mDiscard, Moveable::s_mIdent;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Moveable::Moveable()
{
	D3DXMatrixIdentity( &m_mOrient );
	D3DXMatrixIdentity( &m_mPos );
	D3DXMatrixIdentity( &m_mDist );
	D3DXMatrixIdentity( &s_mIdent );
}

Moveable::~Moveable()
{

}

const D3DXMATRIX* Moveable::transform() const {
	return D3DXMatrixMultiply( 
				&s_mDiscard, 
				D3DXMatrixMultiply( &s_mDiscard, distortion(), orientation()),
				position()
	);
}