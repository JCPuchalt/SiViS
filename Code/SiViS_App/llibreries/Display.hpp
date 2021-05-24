#pragma once

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "lodepng.h"
#include "llibreries.hpp"

class subDisplay;

using namespace cv;
using namespace std;


/********************************************************************
* Classe Display
* Mostra en display les textures
* Tambe comunica (si es Master en les atres RASPBERRY )
*********************************************************************/
class Display
{

private:
	float ancho, alto;
	unsigned char* 	ImagenDisplayVector;
	unsigned char*  InterseccioOptimisada; 
	GfxTexture 	ImagenDisplayTextura;
	bool Master;
	int num_conexions;
	int token; 
	bool totsCalibrats;
	int temps_k;
	float pA;
	float maximaProvabilitat;
	bool  minimFound;
	bool refresca;
	pthread_t conexion_id, refresh_id;

	subDisplay *SUBdisplay;


protected:



public:
	Display(float ancho, float alto, bool esMaster);
	~Display() { free(ImagenDisplayVector); }

	void sumToken		( void )		{ token++; }
	int  getToken		( void )		{ return token; }
	int  getNumConexions 	( void )		{ return num_conexions; }
	void setTotsCalibrats 	( bool valor )		{ totsCalibrats=valor; }
	bool getTotsCalibrats	( void )		{ return totsCalibrats; }
	void setTemps_k		( int valor )		{ temps_k=valor; }

	unsigned char*  pImagenDisplayVector 	(void) { return ImagenDisplayVector; }
	GfxTexture* 	pImagenDisplayTextura  	(void) { return &ImagenDisplayTextura; }

	void  refrescarDisplay		( void );
	void  refrescar			( void ) 	{ refresca=true; } 
	void  conectar 			( void );

	void colorDisplay		( int R, int G, int B, int A );
	void dibuixar			( void );
	bool dibuixarInterseccio	( int idSV );
	void impr			( void );
	int  buscar			( int idC, int idSV );
	void posarPunt			( Point2f punt );
	void posarCircul 		( int centro_x, int centro_y, int radi );
	void crearLlinia		( double m, double b );
	void posarQuadrat		( int OrigeEnx, int OrigeEny, int AmpleQuadrat, int AltQuadrat );
	void borrarQuadrat		( int OrigeEnx, int OrigeEny, int AmpleQuadrat, int AltQuadrat );
	void dibuixarPatroCalibracio 	( float OFFSET_X, float OFFSET_Y, float escala );
	void mascara			( int posTEX );
	void resetInterseccio		( int idx, int roig, int vert, int blau );
	void pegarInterseccioOptimisada ( int idx );
	void copiarInterseccioOptimisada( int idx );

	void proceso 		(void);
	void mostrar_en_display (void);
	void fussio_imagens 	(void);

	subDisplay* crearSubDisplay ( int iD, int *sock );

};
 
static void*	refreshDisplay	( void* context );
void*		servidor	( void* context );
void*		SocketHandler	( void* lp );

/********************************************************************
*
* Classe subDisplay
* Es una classe per a cada RASPBERRY conectada
* Esta classe conté les Taules de Calibració de cada RPI
* Tambe rep i almagasena les textures de les atres RPI
*
*********************************************************************/
enum { calibra };
enum {espera1, recibeTabla, recibeTextura };
enum {espera2, recibeTablaInterseccion, recibeInterseccion };

struct Contexto
{
	Display* display;
	subDisplay* SUBdisplay;
	int* scock;
};

class subDisplay
{

private:
	int idCamara;
	int idSistemaVisio;
	int camaraMaster;
	float OFFSET_X, OFFSET_Y, escala;
	int estadoCalibracion;
	int ordeCalibracio;
	bool calibrada;
	bool reset;

	float ancho, alto;
	int *sock;
	unsigned char* 	ImagenDisplayVector;
	GfxTexture* 	ImagenDisplayTextura;
	Display* display;
	int ordenes1, ordenes2;
	char ruta_patron[100];

	// Tabla calibración
	bool isTablaRecibida;
	int tamTabla;
	int* tabla_calibracion;
	char* tablaRecibida;

	// Textura 
	unsigned char* texturaComprimida;

	// Tabla de intersección
	bool isTablaInterseccionRecibida;
	int tamInterseccio;
	int* tablaInterseccion;

	// Textura intersección
	unsigned char* imagenInterseccion;


protected:

public:
	subDisplay( Display* disp, int* csock, int ordeCal );
	~subDisplay() {}

	void setDisplay			( Display disp ) {};

	int  getOrdenes1		( void ) 		{ return ordenes1; }
	int  getOrdenes2		( void ) 		{ return ordenes2; }
	int  getEstadoCalibracion	( void )		{ return estadoCalibracion; }
	int  getIdCam			( void )		{ return idCamara; }
	int  getIdSV			( void )		{ return idSistemaVisio; }
	int  getOrdeCalibracio		( void )		{ return ordeCalibracio; }


	Display* getDisplay		( void )		{ return display; }
	int* getSock			( void )		{ return sock; } 

	int		get_TamTablaInterseccion	( void )	{ return tamInterseccio; }	
	int*		get_tablaInterseccion		( void )	{ return tablaInterseccion; } 
	unsigned char*	get_imagenInterseccion		( void )	{ return imagenInterseccion; }

	int  recibirCabecera 		( int* sock );
	int  recibirTablaCalibracion 	( int* sock );
	int  recibirTextura 		( int* sock );
	int  recibirTablaInterseccion 	( int* sock );
	bool recibirInterseccio		( int* sock );
	bool dibujarTextura 		( void );
	bool loadPatron 		( void );
	void crear_ruta_patron 		( void );
	void dibuixarPatroCalibracio 	( void );
	void dibuixar 			( void );
	void resetTextura		( int roig, int vert, int blau);

};

