// Board.cpp: implementation of the Board class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Board.h"
#include "DXSystem.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	
Board::Board(DXSystem* dxs, int _size, std::string fTex):
	Renderable(dxs),
	pTex(dxs->loadTexture(fTex)),
	size(_size),
	newFreq(0)
{
	init(size);
	scale(size,0.25f,size);
	cells = new Cell*[size*size];
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ ){
			cells[x+size*y] = new Cell(dxs, x, y);
			cells[x+size*y]->translate(x - size / 2.0f + 0.5f, 0.625f, y - size / 2.0f + 0.5f);
		}
	for ( x = 0; x < size; x++)
		for( int y = 0; y < size; y++ ){
			cell(x,y)->north = cell(x, y + 1);
			cell(x,y)->south = cell(x, y - 1);
			cell(x,y)->east = cell(x + 1, y);
			cell(x,y)->west = cell(x - 1, y);
		}
}


Board::~Board()
{

	m_pvPoints->Release();
	if (pTex != NULL )
		dxs->releaseTexture( pTex );
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			delete cells[x+size*y];
}

float Board::filled()
{
	return (float)Cell::dieCount / (size*size);
}

void Board::reset()
{
	Cell::dieCount = 0;
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			cells[x+size*y]->reset();
}

void Board::fill(float d){
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			if ( (float)rand() / RAND_MAX < d )
				cells[x+size*y]->fill();
}


void Board::tick(int milliseconds)
{
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			cells[x+size*y]->tick(milliseconds);

	//Consider filling a new cell
	float r = (float)rand() / RAND_MAX;
	float t = milliseconds / 1000.0f;
	if ( (1 - filled() ) * r < t * newFreq )
	{
		//dxs->debugprntf("NEW DIE! r=%f, t=%f", r, t);
		int rx, ry;
		rx = (rand() * size - 1) / RAND_MAX;
		ry = (rand() * size - 1) / RAND_MAX;
		cell(rx, ry)->fill();
	}
}

void Board::render(Context *c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	draw();
	c->m_mspWorld->Pop();
	for ( int x = 0; x < size; x++)
		for( int y = size - 1; y >= 0; y-- )
			cells[x+size*y]->render(c);
}

Cell* Board::cell(int x, int y)
{
	if ( x < 0 || y < 0 || x >= size || y >= size )
		return NULL;
	return cells[x+size*y];
}

void Board::init(int size)
{
		dxs->debugprintf("Initializing Board Geometry");
		pd3dDevice->CreateVertexBuffer(26*sizeof(PNTVertex), 0 ,FVF_PNT, D3DPOOL_MANAGED, &m_pvPoints);
		PNTVertex* pV;
		m_pvPoints->Lock(0, 0, (BYTE**)&pV,0);

		pV[0].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pV[1].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pV[2].p = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pV[3].p = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		pV[0].n = D3DXVECTOR3(0, 0, -1);
		pV[2].n = D3DXVECTOR3(0, 0, -1);
		pV[3].n = D3DXVECTOR3(0, 0, -1);
		pV[4].n = D3DXVECTOR3(0, 0, -1);


		pV[4].p = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		pV[5].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
		pV[6].p = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pV[7].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[4].n = D3DXVECTOR3(1,0,0);
		pV[5].n = D3DXVECTOR3(1,0,0);
		pV[6].n = D3DXVECTOR3(1,0,0);
		pV[7].n = D3DXVECTOR3(1,0,0);

		pV[8].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[9].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
		pV[10].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		pV[11].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
		pV[8].n = D3DXVECTOR3(0,0,1);
		pV[9].n = D3DXVECTOR3(0,0,1);
		pV[10].n = D3DXVECTOR3(0,0,1);
		pV[11].n = D3DXVECTOR3(0,0,1);

		pV[12].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
		pV[13].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pV[14].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		pV[15].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pV[12].n = D3DXVECTOR3(-1,0,0);
		pV[13].n = D3DXVECTOR3(-1,0,0);
		pV[14].n = D3DXVECTOR3(-1,0,0);
		pV[15].n = D3DXVECTOR3(-1,0,0);
		
		pV[16].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pV[17].p = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pV[18].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		pV[19].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[16].n = D3DXVECTOR3(0,-1,0);
		pV[17].n = D3DXVECTOR3(0,-1,0);
		pV[18].n = D3DXVECTOR3(0,-1,0);
		pV[19].n = D3DXVECTOR3(0,-1,0);

		pV[20].p = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		pV[21].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);

		pV[22].p = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
		pV[23].p = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		pV[24].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);	
		pV[25].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pV[22].n = D3DXVECTOR3(0,1,0);
		pV[23].n = D3DXVECTOR3(0,1,0);
		pV[24].n = D3DXVECTOR3(0,1,0);
		pV[25].n = D3DXVECTOR3(0,1,0);

		pV[1].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[0].t = D3DXVECTOR2(0.0f, 1.0f);
		pV[3].t = D3DXVECTOR2(size, 0.0f);
		pV[2].t = D3DXVECTOR2(size, 1.0f);

		pV[4].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[6].t = D3DXVECTOR2(0.0f, 1.0f);
		pV[5].t = D3DXVECTOR2(size, 0.0f);
		pV[7].t = D3DXVECTOR2(size, 1.0f);

		pV[9].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[8].t = D3DXVECTOR2(0.0f, 1.0f);
		pV[11].t = D3DXVECTOR2(size, 0.0f);
		pV[10].t = D3DXVECTOR2(size, 1.0f);

		pV[12].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[14].t = D3DXVECTOR2(0.0f, 1.0f);
		pV[13].t = D3DXVECTOR2(size, 0.0f);
		pV[15].t = D3DXVECTOR2(size, 1.0f);

		pV[16].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[18].t= D3DXVECTOR2(0.0f, size);
		pV[17].t = D3DXVECTOR2(size, 0.0f);
		pV[19].t = D3DXVECTOR2(size, size);

		pV[24].t = D3DXVECTOR2(0.0f, 0.0f);
		pV[25].t = D3DXVECTOR2(0.0f, size);
		pV[22].t = D3DXVECTOR2(size, 0.0f);
		pV[23].t = D3DXVECTOR2(size, size);

		m_pvPoints->Unlock();
}


void Board::draw()
{
	pd3dDevice->SetMaterial( &mtrl );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTexture( 0, pTex );
	pd3dDevice->SetVertexShader(FVF_PNT);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetStreamSource(0, m_pvPoints, sizeof(PNTVertex));
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 24);
}


