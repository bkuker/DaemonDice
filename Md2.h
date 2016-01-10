
// MD2 View

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 7.0a SDK
// Date : 16th May 2000
// File : ImportMD2.hpp

// Website: http://members.aol.com/billybop7
// Email:  billybop7@aol.com 

 

#if !defined(AFX_MD2_H__E61FECA5_3024_465B_897F_6dd2532E5562__INCLUDED_)
#define AFX_MD2_H__E61FECA5_3024_465B_897F_6dd2532E5562__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"


#define MAX_SKINNAME 64
#define MAX_NUM_SKINS 64

#define MD2_TRIANGLEFAN 1
#define MD2_TRIANGLESTRIP 2

// MD2 File Header structure
typedef struct
{
	int ident;
	int version;

	int skinwidth;
	int skinheight;
	int framesize;	

	int num_skins;
	int num_verts;
	int num_tex_verts;	
	int num_faces;
	int num_glcmds; 
	int num_frames;

	int offset_skins;	
	int offset_tex_verts;		
	int offset_faces;	
	int offset_frames; 
	int offset_glcmds; 
	int offset_end;	

} MD2HEADER;


typedef struct
{
	float x;
	float y;
	float z;
	float u;
	float v;

} MD2VERT;


typedef struct
{
	float s;
	float t;
	int   index;
} MD2FACEINDEX; // GLVERTS


typedef struct
{
	BYTE v[3]; 
	BYTE lightnormalindex;

} MD2BYTE_VERTEX; 


typedef struct
{				
	char framename[16];		
	IDirect3DVertexBuffer8 *Verts;

} MD2FRAME;

typedef struct
{			
	D3DPRIMITIVETYPE PrimitiveType;
	MD2FACEINDEX *FaceIndices;
	int NumFaceIndices;
	int min, max;
	int first, last;

} MD2OBJECT;


typedef struct
{		
	MD2FRAME  *Frames;
	MD2OBJECT *Primitives;
	int NumPrimitives;
	int NumFrames;
	int NumVerts;
	int NumIndices;
	float skx;
	float sky;
	IDirect3DIndexBuffer8 *MasterIndexBuffer;
	
} MD2MODEL;


class Md2 : public Renderable
{
	
public:

	Md2(DXSystem* dxs, std::string model, std::string tex);
	Md2(DXSystem* dxs);
	~Md2();

	void setFrame(int f1, int f2, float f);
	void draw();

protected:
	void load( std::string model, std::string tex);
	//refcounted methods
	MD2MODEL* LoadMD2Model( std::string filename );
	 void ReleaseMD2Model( MD2MODEL *model );

	//real methods
	 MD2MODEL* __LoadMD2Model( const char *filename );
	 void __ReleaseMD2Model( MD2MODEL *model );


protected:
	MD2MODEL* m_pModel;
	bool Interpolate;
	int curFrame; //used if no interpolate
	IDirect3DVertexBuffer8 *IntVerts;

	LPDIRECT3DTEXTURE8 pTex;

//refcount variables
protected:
	std::map< std::string, MD2MODEL* > models;
	std::map< MD2MODEL*, int > modelref;
	std::map< MD2MODEL*, std::string > modelfile;
};

#endif // IMPORTMD2_HPP