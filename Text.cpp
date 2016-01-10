// Text.cpp: implementation of the Text class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Text.h"
#include "Context.h"
#include "DXSystem.h"

D3DXMATRIX Text::s_mR1;
D3DXMATRIX Text::s_mL1;
Banner* Text::b = NULL;
int Text::refCount = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Text::Text(DXSystem* dxs):
	Renderable(dxs),
	j(LEFT)
{
	refCount++;
	if ( refCount == 1 ){
		D3DXMatrixTranslation( &s_mR1,0.33f,0,0);
		D3DXMatrixTranslation( &s_mL1,-0.33f,0,0);
		b = new Banner(dxs, "textures/bigfont.dds");
		b->scale(0.5f, 1, 1);
	}
	printf("A TEST OF FLOAT %f AND INT %i", 1.23f, 24);
}

Text::~Text()
{
	refCount--;
	if (refCount ==0)
		delete b;
}

void Text::render(Context* c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

	if ( j == LEFT)
		for (int i = 0; i < strlen(text); i++)
		{
			///Next 9 lines adopted from Dave Park's
			//3d Games final project
			float lx, ly;
			lx = int(text[i])-32; ly = 0;
			while (lx >= 16) {
				ly++;
				lx -= 16;
			}	
			lx *= (1.0f/16.0f);
			ly *= (1.0f/8.0f);
			
			b->setArea(lx, ly, lx+(1.0f/16.0f), ly+(1.0f/8.0f));
			b->render(c);
			c->m_mspWorld->MultMatrixLocal( &s_mR1 );
			pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

		}
	else if ( j == RIGHT )
		for (int i = strlen(text) - 1; i >= 0; i--)
		{
			///Next 9 lines adopted from Dave Park's
			//3d Games final project
			float lx, ly;
			lx = int(text[i])-32; ly = 0;
			while (lx >= 16) {
				ly++;
				lx -= 16;
			}	
			lx *= (1.0f/16.0f);
			ly *= (1.0f/8.0f);
			
			b->setArea(lx, ly, lx+(1.0f/16.0f), ly+(1.0f/8.0f));
			b->render(c);
			c->m_mspWorld->MultMatrixLocal( &s_mL1 );
			pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

		}
	else if ( j == CENTER )
		dxs->debugprintf("Text: CENTER Justification not supported.");
	else
		dxs->debugprintf("Text: Unrecognized justification %i!!", (int)j);

	c->m_mspWorld->Pop();
}


void Text::printf(const char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	vprintf(format, argptr);
}

void Text::vprintf(const char *format, va_list argptr)
{
	_vsnprintf(text, MAX_TEXT, format, argptr);
}
