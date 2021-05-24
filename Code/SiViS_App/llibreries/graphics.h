#pragma once

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "lodepng.h"
#include "llibreries.hpp"


using namespace cv;
using namespace std;



void InitGraphics();
void ReleaseGraphics();
void BeginFrame();
void EndFrame();

/***********************************************************************
Clase GfxShader
************************************************************************/
class GfxShader
{
	GLchar* Src;
	GLuint Id;
	GLuint GlShaderType;

public:

	GfxShader() : Src(NULL), Id(0), GlShaderType(0) {}
	~GfxShader() { if(Src) delete[] Src; }

	bool   LoadVertexShader		(const char* filename);
	bool   LoadFragmentShader	(const char* filename);
	GLuint GetId			( void ) { return Id; }
};


/***********************************************************************
Clase GfxProgram
************************************************************************/
class GfxProgram
{
	GfxShader* VertexShader;
	GfxShader* FragmentShader;
	GLuint Id;

public:

	GfxProgram() {}
	~GfxProgram() {}

	bool Create(GfxShader* vertex_shader, GfxShader* fragment_shader);
	GLuint GetId() { return Id; }
};


/*********************************************************************************************
Clase GfxTexture
**********************************************************************************************/
class GfxTexture
{
	int Width;
	int Height;
	GLuint Id;
	bool IsRGBA;

	GLuint FramebufferId;
public:

	GfxTexture() : Width(0), Height(0), Id(0), FramebufferId(0) {}
	~GfxTexture() {}

	bool	CreateRGBA		( int width, int height, const void* data = NULL );
	bool	CreateGreyScale		( int width, int height, const void* data = NULL );
	bool	GenerateFrameBuffer();
	void	SetPixels		( const void* data );
	GLuint	GetId			( void ) 	{ return Id; }
	GLuint	GetFramebufferId	( void ) 	{ return FramebufferId; }
	int	GetWidth		( void ) 	{ return Width;}
	int	GetHeight		( void ) 	{ return Height;}
	void	Save			( const char* fname );
	void	SaveBMP			( const char* fname );
	void	GetPixels 		( void* image_desti );
};




/***************************************************************************************************************************************************************
Funcions Comuns a les classes anteriors
****************************************************************************************************************************************************************/
void SaveFrameBuffer(const char* fname);

void DrawTextureRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawYUVTextureRect			( GfxTexture* ytexture, GfxTexture* utexture, GfxTexture* vtexture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawBlurredRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawSobelRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawMedianRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawMultRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, float r, float g, float b, GfxTexture* render_target);
void DrawThreshRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, float r, float g, float b, GfxTexture* render_target);
void DrawDilateRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawErodeRect			( GfxTexture* texture, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawPatroRect     			( GfxTexture* texture, float R, float G, float B, float x0, float y0, float x1, float y1, float inc, GfxTexture* render_target);
void DrawCompensarFondo 		( GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawCompensar2    			( GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawCompensarGrisos  		( GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawCompensarPD      		( GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target);
void DrawCompensar3   			( GfxTexture* texture_im, GfxTexture* texture_pant, int maxvalor, int Rc, int Gc, int Bc, float x0, float y0, float x1, float y1, GfxTexture* render_target);
int  ILLUMINACIO       			( GfxTexture* texture, int Rc, int Gc, int Bc, int fondo);
void Matriu_Transformacio 		( GfxTexture* texture_im, float corregir_x, float corregir_y, GfxTexture* render_target);
bool Matriu_Transformacio_CPU_eficient 	( GfxTexture* pantalla_tex, GfxTexture* mostrar_tex, GfxTexture* textura_calibracion);
void DrawPuntRect      			( GfxTexture* texture, int x, int y, GfxTexture* render_target);
void DrawMijaRect      			( GfxTexture* texture, int AxA, int x, int y, GfxTexture* render_target);
void DrawMija5x5Rect   			( GfxTexture* texture, int AxA, int x, int y, GfxTexture* render_target);
void DrawCirculRect    			( GfxTexture* texture, int x, int y, GfxTexture* render_target);
void mat2textUC		  		( const Mat&, GfxTexture* texture);
void mat2textF		 		( const Mat& mat, GfxTexture* texture);
void dibuixar				( GfxTexture* texture, void* data);


