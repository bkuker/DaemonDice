// Context.cpp: implementation of the Context class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Context::Context()
{
	D3DXCreateMatrixStack( 0, &m_mspView );
	D3DXCreateMatrixStack( 0, &m_mspWorld );
	D3DXCreateMatrixStack( 0, &m_mspProjection );
}

Context::~Context()
{

}

Context::apply(IDirect3DDevice8* dev){
	if (m_mspWorld != NULL)
		dev->SetTransform( D3DTS_WORLD, m_mspWorld->GetTop() );
	if (m_mspView != NULL)
		dev->SetTransform( D3DTS_VIEW, m_mspView->GetTop() );
	if (m_mspProjection != NULL)
		dev->SetTransform( D3DTS_PROJECTION, m_mspProjection->GetTop() );
}

Context::SetTransform( D3DTRANSFORMSTATETYPE t, const D3DXMATRIX* m){
	switch (t){
	case D3DTS_WORLD:
		m_mspWorld->Release();
		D3DXCreateMatrixStack( 0, &m_mspWorld );
		m_mspWorld->LoadMatrix( m );
		break;
	case D3DTS_VIEW:
		m_mspView->Release();
		D3DXCreateMatrixStack( 0, &m_mspView );
		m_mspView->LoadMatrix( m );
		break;
	case D3DTS_PROJECTION:
		m_mspProjection->Release();
		D3DXCreateMatrixStack( 0, &m_mspProjection );
		m_mspProjection->LoadMatrix( m );
		break;
	}
}

D3DXMATRIX* Context::transform(){
	return D3DXMatrixMultiply( 
				&m_mTransform,
				D3DXMatrixMultiply( &m_mTransform, m_mspWorld->GetTop(), m_mspView->GetTop()),
				m_mspProjection->GetTop()
	);
}

D3DXMATRIX* Context::worldView(){
	return D3DXMatrixMultiply( &m_mTransform, m_mspView->GetTop(), m_mspWorld->GetTop());
}