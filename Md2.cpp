
// MD2 View

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 7.0a SDK
// Date : 16th May 2000
// File : ImportMD2.cpp

// Website: http://members.aol.com/billybop7
// Email:  billybop7@aol.com 

#include "stdafx.h"

#include "iostream.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>
#include <windows.h>

#include "Md2.h"
#include "DXSystem.h"
#include "Context.h"

#include "Md2normals.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


PNTVertex src_v[10000];


Md2::Md2(DXSystem* dxs, std::string file, std::string tex):
	Renderable(dxs),
	Interpolate(true),
	m_pModel(NULL),
	IntVerts(NULL),
	pTex(NULL)
{
	load( file, tex);
}

Md2::Md2(DXSystem* dxs):
	Renderable(dxs),
	Interpolate(true),
	m_pModel(NULL),
	IntVerts(NULL),
	pTex(NULL)
{

}

void Md2::load( std::string file, std::string tex){
	if (m_pModel != NULL)
		ReleaseMD2Model(m_pModel);
	if (IntVerts != NULL)
		IntVerts->Release();
	if (pTex != NULL)
		dxs->releaseTexture(pTex);

	m_pModel = LoadMD2Model( file );
	pd3dDevice->CreateVertexBuffer( 
			m_pModel->NumVerts*sizeof(PNTVertex),
			0, FVF_PNT,
			D3DPOOL_MANAGED,
			&IntVerts);
		
	// Load the skin
	if( (pTex = dxs->loadTexture(tex)) == NULL )
		dxs->debugprintf("Couldn't find texture : %s", tex.c_str());
	else
		dxs->debugprintf("CMD2::LoadMD2Model - loaded %s", tex.c_str());
}

Md2::~Md2()
{
	ReleaseMD2Model(m_pModel);
	IntVerts->Release();
	dxs->releaseTexture(pTex);
}

MD2MODEL* Md2::LoadMD2Model( std::string file )
{
	std::map< std::string, MD2MODEL* >::iterator i;
	i = models.find( file );
	if ( i == models.end() ){
		MD2MODEL* pModel = NULL;
		
		//load file into pModel
		pModel = __LoadMD2Model( file.c_str() );

		models[ file ] = pModel;
		modelfile[ pModel ] = file;
		modelref[ pModel ] = 1;

		return pModel;
	} else {
		modelref[i->second]++;
		return i->second;
	}
}

void Md2::ReleaseMD2Model( MD2MODEL *model )
{
	if ( model != NULL ){
		modelref[model]--;
		if ( modelref[model] == 0 ){
			models.erase( modelfile[model] );
			modelfile.erase( model );
			__ReleaseMD2Model(model);
		}
	}
}


	// Load in a MD2 model from file, read all the model data, and put it
// into the model object.
MD2MODEL* Md2::__LoadMD2Model( const char *filename )
{
	MD2MODEL* model = new MD2MODEL;
	FILE *fp = NULL;
	MD2HEADER header;
	MD2BYTE_VERTEX bverts;
	float bscale[3];		
	float translate[3]; 
	char name[16];		
	int v;
	int frame_num;
	int N;
	int id;
	int findex;
	int num_indices_per_primitive;	
	int cnt = 0;
	int prim_cnt = 0;
	int num_glcmds = 0;
	int num_indices = 0;
	float fs, ft;
	

	fp = fopen(filename,"rb");

	if(fp == NULL)
	{
		dxs->debugprintf("CMD2::LoadMD2Model - Can't find model : %s", filename);
		return NULL;
	}
	else
		dxs->debugprintf("CMD2::LoadMD2Model - Loading : %s", filename);
	

	// read file header into MD2HEADER structure
	fread(&header, sizeof(MD2HEADER), 1, fp); 



    // QUICK CHECK TO GET the number of GLCMDS in model

	// move file pointer to the start of the GL command list
	fseek(fp, (UINT)header.offset_glcmds, SEEK_SET);
	
	// if there are no GL commands here quit
	if(header.offset_end == header.offset_glcmds)
	{
		fclose(fp);
		dxs->debugprintf("CMD2::LoadMD2Model - No GL Commands");
		return NULL;
	}

	id = 1;
	num_glcmds = 0;
	num_indices = 0;

	while( id != 0 )	
	{
		// read in primitive commands from md2 file
		fread(&id,sizeof(int),1,fp);
		
		// id = 0 indicates the end of the command list
		if(id != 0)
		{
			num_indices_per_primitive = abs(id);

			for(findex = 0; findex < num_indices_per_primitive; findex++)
			{
				fread(&fs,sizeof(float),1,fp);
				fread(&ft,sizeof(float),1,fp);
				fread(&N,sizeof(int),1,fp);

				num_indices++;
			} 
			num_glcmds++;
		}

	} // end while


	// LOAD FACE AND TEXTURE CO_ORDINATES
	
	// the GL command format:
	// a positive integer for the "id" variable starts a tristrip command, 
	// followed by that many vertex structures.
	// a negative integer foir "id" starts a trifan command, followed 
	// by -x vertexes a zero indicates the end of the command list.
	// a vertex consists of a floating point s, a floating point t,
	// and an integer vertex index.


	// move file pointer to the start of the GL command list
	fseek(fp, (UINT)header.offset_glcmds, SEEK_SET);
	
	// if there are no GL commands here quit
	if(header.offset_end == header.offset_glcmds)
	{
		fclose(fp);
		dxs->debugprintf("CMD2::LoadMD2Model - No GL Commands");
		return NULL;
	}
		
	model->NumPrimitives = num_glcmds;
	model->Primitives = new MD2OBJECT[num_glcmds];


	id = 1;
	num_indices = 0;
	prim_cnt = 0;

	//position in master index buffer
	int index_cnt = 0;
	while( id != 0 )	
	{
		// read in primitive commands from md2 file
		fread(&id,sizeof(int),1,fp);
		
		// id = 0 indicates the end of the command list
		if(id != 0)
		{
			num_indices_per_primitive = abs(id);

			if(id < 0 )
				model->Primitives[prim_cnt].PrimitiveType = D3DPT_TRIANGLEFAN;

			if(id > 0 )
				model->Primitives[prim_cnt].PrimitiveType = D3DPT_TRIANGLESTRIP;

		
		
			
			model->Primitives[prim_cnt].FaceIndices = new MD2FACEINDEX[num_indices_per_primitive];
			model->Primitives[prim_cnt].NumFaceIndices = num_indices_per_primitive;

			model->Primitives[prim_cnt].min = 100000;
			model->Primitives[prim_cnt].max = 0;
			model->Primitives[prim_cnt].first = index_cnt;

			for(findex = 0; findex < num_indices_per_primitive; findex++)
			{
				// read texture co-ordinates s and t
				fread(&fs,sizeof(float),1,fp);
				model->Primitives[prim_cnt].FaceIndices[findex].s = fs;

				fread(&ft,sizeof(float),1,fp);
				model->Primitives[prim_cnt].FaceIndices[findex].t = ft;

				// read face index
				fread(&N,sizeof(int),1,fp);
				if ( N < model->Primitives[prim_cnt].min )
					model->Primitives[prim_cnt].min = N;
				if ( N > model->Primitives[prim_cnt].max )
					model->Primitives[prim_cnt].max = N;
				model->Primitives[prim_cnt].FaceIndices[findex].index = N;

				num_indices++;
				index_cnt++;

			} // end for findex

			model->Primitives[prim_cnt].last = index_cnt;

			prim_cnt++;
		
		} // end if

	} // end while
	model->NumIndices = num_indices;


	// LOAD VERTS FOR EACH FRAME OF ANIMATION

	model->Frames = new MD2FRAME[header.num_frames];
	model->NumFrames = header.num_frames;

	model->NumVerts = header.num_verts;

	// read vertices for all frames

	fseek(fp, (UINT)header.offset_frames, SEEK_SET);

	for(frame_num = 0; frame_num < header.num_frames; frame_num++)
	{
		fread(bscale,sizeof(float),3,fp);		
		fread(translate,sizeof(float),3,fp);
		fread(name,1,16,fp);				
		
		if(strlen(name) <= 16)
			strcpy(model->Frames[frame_num].framename, name );
	
		//model->Frames[frame_num].Verts = new MD2VERT[header.num_verts];
		pd3dDevice->CreateVertexBuffer( 
			header.num_verts*sizeof(PNTVertex),
			0, FVF_PNT,
			D3DPOOL_MANAGED,
			&(model->Frames[frame_num].Verts));
		PNTVertex* pV;
		model->Frames[frame_num].Verts->Lock(0, 0, (BYTE**)&pV,0);

		for(v = 0; v < header.num_verts; v++) // VERTS
		{
			fread(&bverts,sizeof(MD2BYTE_VERTEX),1,fp); 
			//Swap X and Z 			
			pV[v].p.z = -(bscale[0] * bverts.v[0] + translate[0]);
			pV[v].p.x = (bscale[1] * bverts.v[1] + translate[1]);
			pV[v].p.y = (bscale[2] * bverts.v[2] + translate[2]);
			
			pV[v].n.x = normal_table[bverts.lightnormalindex][1];
			pV[v].n.y = normal_table[bverts.lightnormalindex][2];
			pV[v].n.z = normal_table[bverts.lightnormalindex][3];
		}

		model->Frames[frame_num].Verts->Unlock();
	}

	//Build master index buffer
	pd3dDevice->CreateIndexBuffer( 
		index_cnt * num_indices_per_primitive * 2,
		0, 
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&(model->MasterIndexBuffer));
	short* pI;
	model->MasterIndexBuffer->Lock(0, index_cnt, (BYTE**)&pI,0);

	int master_index_cnt = 0;
	//For each primitive apply the texture cords.
	for(prim_cnt = 0; prim_cnt < model->NumPrimitives; prim_cnt++)
	{
		// get the number of indices (indexes) for this poly
		num_indices = model->Primitives[prim_cnt].NumFaceIndices;
		// go through all the indices, and texture co-ordinates for this poly
		for(int index_cnt = 0; index_cnt < num_indices; index_cnt++)
		{
			int index = model->Primitives[prim_cnt].FaceIndices[index_cnt].index;
			float tex_coord_s = model->Primitives[prim_cnt].FaceIndices[index_cnt].s;
			float tex_coord_t = model->Primitives[prim_cnt].FaceIndices[index_cnt].t;
			pI[master_index_cnt] = index;

			//for each frame
			for(frame_num = 0; frame_num < header.num_frames; frame_num++)
			{
				PNTVertex* pV;
				model->Frames[frame_num].Verts->Lock(0, 0, (BYTE**)&pV,0);

				pV[index].t.x = tex_coord_s;
				pV[index].t.y = tex_coord_t;

				model->Frames[frame_num].Verts->Unlock();
			}
			master_index_cnt++;
		}
	}

	model->MasterIndexBuffer->Unlock();

	fclose(fp);

	dxs->debugprintf(	"CMD2::LoadMD2Model - %s", filename);
	dxs->debugprintf(	"      with %i frames", header.num_frames);
	dxs->debugprintf(	"           %i faces", header.num_faces);
	dxs->debugprintf(	"           %i vertices", header.num_verts);

	return model;
}


void Md2::__ReleaseMD2Model( MD2MODEL *model )
{
	int i;

	for(i = 0; i < model->NumFrames; i++)
		model->Frames[i].Verts->Release();

	SAFE_DELETE_ARRAY(model->Frames);

	for(i = 0; i < model->NumPrimitives; i++){
		SAFE_DELETE_ARRAY(model->Primitives[i].FaceIndices);
	}

	model->MasterIndexBuffer->Release();
	SAFE_DELETE_ARRAY(model->Primitives);
	dxs->releaseTexture(pTex);
}

void Md2::setFrame(int f1, int f2, float f)
{
	if ( m_pModel == NULL )
		return;

	curFrame = f1;
	if (Interpolate){
		if ( f1 >= 0 && f1 < m_pModel->NumFrames && f2 >= 0 && f2 < m_pModel->NumFrames ){


			PNTVertex *pV1, *pV2, *pVi;
			m_pModel->Frames[f1].Verts->Lock(0, 0, (BYTE**)&pV1,0);
			m_pModel->Frames[f2].Verts->Lock(0, 0, (BYTE**)&pV2,0);
			IntVerts->Lock(0, 0, (BYTE**)&pVi,0);

			for (int i = 0; i < m_pModel->NumVerts; i++){
				pVi[i].p = f * pV2[i].p + (1.0 - f) * pV1[i].p;
				//These should be moved
				pVi[i].t = pV1[i].t;
				pVi[i].n = pV1[i].n;
			}

			m_pModel->Frames[f1].Verts->Unlock();
			m_pModel->Frames[f2].Verts->Unlock();
			IntVerts->Unlock();

		}
	} 	
}


void Md2::draw( )
{	
	if (m_pModel == NULL)
		return;

	dxs->pd3dDevice->SetTexture( 0, pTex );
	dxs->pd3dDevice->SetVertexShader(FVF_PNT);
	
	if (Interpolate)
		dxs->pd3dDevice->SetStreamSource(0, IntVerts, sizeof(PNTVertex));
	else
		dxs->pd3dDevice->SetStreamSource(0, m_pModel->Frames[curFrame].Verts, sizeof(PNTVertex));

	dxs->pd3dDevice->SetIndices( m_pModel->MasterIndexBuffer, 0 );
	

	// go through the list of polys one by one and render then
	for(int prim_cnt = 0; prim_cnt < m_pModel->NumPrimitives; prim_cnt++)
	{
		
		if (dxs->pd3dDevice->DrawIndexedPrimitive(
										m_pModel->Primitives[prim_cnt].PrimitiveType,
										m_pModel->Primitives[prim_cnt].min,
										m_pModel->Primitives[prim_cnt].max - m_pModel->Primitives[prim_cnt].min + 1,
										m_pModel->Primitives[prim_cnt].first,
										m_pModel->Primitives[prim_cnt].NumFaceIndices - 2
										) != D3D_OK) 
		{
			dxs->debugprintf("CMD2::RenderMD2Model - DrawPrimitive FAILED at %i", prim_cnt);	
			return;
		}

	}

	return;
}