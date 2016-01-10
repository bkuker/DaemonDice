// Moveable.h: interface for the Moveable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEABLE_H__5A7AF4D0_0197_4824_B3ED_83BA04DA7F68__INCLUDED_)
#define AFX_MOVEABLE_H__5A7AF4D0_0197_4824_B3ED_83BA04DA7F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Moveable  
{
public:
	Moveable();
	virtual ~Moveable();
	virtual const D3DXMATRIX* position() const {return &m_mPos;};
	virtual const D3DXMATRIX* orientation() const {return &m_mOrient;};
	virtual const D3DXMATRIX* distortion() const {return &m_mDist;};
	virtual const D3DXMATRIX* transform() const;

	virtual void translate( float x, float y, float z ) { D3DXMatrixTranslation( &m_mPos, x, y, z ); };
	virtual void scale( float x, float y, float z ) { D3DXMatrixScaling( &m_mDist, x, y, z ); };
	virtual void rotateYawPitchRoll( float y, float p, float r ) { D3DXMatrixRotationYawPitchRoll( &m_mOrient, y, p, r ); };

protected:
	static D3DXMATRIX s_mDiscard, s_mIdent; //A statically allocate discard matrix
	D3DXMATRIX m_mPos, m_mDist, m_mOrient;
};

#endif // !defined(AFX_MOVEABLE_H__5A7AF4D0_0197_4824_B3ED_83BA04DA7F68__INCLUDED_)
