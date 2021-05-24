#pragma once

#include "llibreries.hpp"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "lodepng.h"


using namespace cv;
using namespace std;


class  camara;
class  bufferCircular;
class  sistema_visio;
struct relaciones;

struct argumentos
{
	sistema_visio* SV;
	int* scock;
};


/**
 * @ Classe Sistemes de Visio
 *
 * @ El Sistema de Referencia de la imagen de la cámara es (x,y)  
 * @ siendo x el eje vertical hacia abajo
 * @ siendo y el eje horizontal hacia la derecha
 * 
 * @ El Sistema de Referencia del Display es (x,y)  
 * @ siendo x el eje horizontal hacia la derecha
 * @ siendo y el eje vertical hacia la arriba
 *
 * @ El Sistema de Referencia de la Imagen Compuesta es (x,y)  
 * @ siendo x el eje vertical hacia abajo
 * @ siendo y el eje horizontal hacia la derecha
 * 
 * @ 

*/
class sistema_visio
{
protected:


private:
	int idSV;
	int Port_IP;
	int num_camaras;
	int codicImage;
	int numPort[4];
	int config;
	int numMuestras;

	bool minimFound;
	bool compensada;
	bool monoCamara;

	int CICLES;
	long int tamTabla1, tamTabla2;
	Point2f centroImagen;
	bool foto;
	pthread_t grabar_id, conexion_id;

	int dia;
	char condicion;
	int probeta;
	int NUM_CAMS;
	// Nombre que recibe del servidor
	char nombre[50];

	bufferCircular* buffer;

	char ruta_tablaCalibracionFusion[100];
	char ruta_idCAMARA[100];
	char ruta_tablaInterseccion[100];
	long long int CNTFusionInversa;
	int cntInterseccion;
	int anticTamanyTabla;

	camara* CAM;
	Display* display;

	GfxTexture* ImagenDisplayTextura;
	unsigned char* ImagenDisplayVector;

	unsigned char* fusionada;
	unsigned char* evaluado;
	unsigned char* imagen;

	Mat interseccioReconstruida;
	// Matriu en la relacio de CElegans
	vector <relaciones> relacions;

	long int* tabla_calibracionFusionInversa;
	int* tabla_Interseccion;
	int* NUEVA_TABLA;
	unsigned char* detectadaInterseccion;
	unsigned char* detectada;

	long int* tabla_POSIM;
	long int* tabla_POSFUS;
	unsigned char* idCAMARA;

	bool resultat_enviament;	int estat_enviament;
	bool resultat_guardar;		int estat_guardar;

	bool con_conexion, conectat, proyecte_nou, bloquejat;
	bool enviar, capturando, inici_local;
	bool inici_remot, iniciat, proyecto_creado;
	bool fin_captura;


public:
	sistema_visio ( int id, int conf, int num_camaras, int confPort[], Display* display, int cicles, int IP_Port );
	~sistema_visio() { delete[] CAM; }	 // delete[] grabar_id; delete[] conexion_id; 		// for( int i=0; i<num_camaras; i++ ) delete CAM[i];

	void texturaColor  (int Rc, int Gc, int Bc );
	
	void proceso 					( void );
	unsigned char* getImagenDisplay			( void )	{ return ImagenDisplayVector; }
	void inicialisarPinsGPIO			( void );
	void recibeDisplay				( void );

	void set_dia		( int  valor )	{ dia		= valor; }
	void set_condicion	( char valor )	{ condicion	= valor; }
	void set_probeta	( int  valor )	{ probeta	= valor; }
	void set_NUM_CAMS	( int  valor )	{ NUM_CAMS	= valor; }

	void fussio_imagens 				( void );
	void fussioInversa_imagensIndividual		( void );
	void fussioInversa_imagens			( void );
	void fusionarTablasDeFusion			( void );
	void detectarCElegans				( bool foto );
	void procesarInterseccionParcial		( bool foto );
	void reconstruirInterseccio 			( bool foto );
	void copiarImage 				( Mat& desti, Mat& orige, int llimit );
	void crearFils 					( void );
	void recompensar 				( void );
	bool isMinimFound 				( void );
	bool getEstatCompensada				( void );

	int  get_Port_IP				( void )	{ return Port_IP; }

	int  Configuracio_Remota			( int sock, int TAM, float temperatura, int Id, int NUM_IMAGE);
	void sincronisacio				( void );
	void inicialisarCaptura 			( void );

	void calibracioDeFussio 			( void );
	int  calculo_Tabla_GXP_InversaX4CAM 		( void );
	int  calculo_Tabla_GXP_InversaX2CAM 		( void );
	int  calculo_Tabla_GXP_InversaX2CAMIndividual 	( void );
	void calcularCentroYRadioCirculo 		( Mat& nubePuntos, int *Cx, int *Cy, int *R,  long int totalPuntos );
	void calcularTransformacion 			( Mat& G, int Cx, int Cy, int radioImTextura );

	void detectarInterseccionesX4			( void );
	void detectarInterseccionesX2			( void );
	void detectarInterseccionesEnFusionadaX4CAM	( void );
	void detectarInterseccionesEnFusionadaX2CAM	( void );
	void calculoPixelesIntersectados 		( void );
	void compensacionInterseccion			( void );
	void generadorDeRuta				( char direccionDeGrabacion[] );

	void recibirMatrizCalibracion			(int  sock, int  idCamReb, int  idSVReb );
	void cabeceraSincronizacion			(int  sock, int* idCamReb, int* idSVReb, int* tipoDato);
	void enviarMatrizCalibracion			(int  sock, int  idCamReb, int  idSVReb );
	void recibirTablaInterseccionParcial 		(int  sock, int  idCamReb, int  idSVReb );
	void recibirInterseccionParcial 		(int  sock, int  idCamReb, int  idSVReb );

	void crear_ruta_tablaCalibracionFusion		( void );
	void crear_ruta_idCAMARA 			( void );
	void crear_ruta_tablaInterseccion		( void );
	bool loadTablaFusionInversa			( void );
	bool saveTablaFusionInversa			( void );
	bool loadTablaInterseccion			( void );
	bool saveTablaInterseccion 			( void );

	int  GetId			( void ) 		{ return idSV; }
	void SetId			( int id ) 		{ idSV=id; }
	int  buscar			( int idC, int IDSV );

	bool isCapturando (void) 		{ return capturando;  }
	void setCapturando (bool value)		{ capturando = value; }

	void setEstadoCompensando (bool valor)	{ compensada=valor; }

	bool isIniciat (void)		{ return iniciat; }
	void setIniciat (bool value)	{ iniciat=value; }

	void  setFinCaptura (bool value)	{ fin_captura = value; }
	void  finCapturaSV (void);

	bufferCircular* getBuffer	(void)		{ return buffer; }
	unsigned char* getEvaluado 	(void)		{ return evaluado; }
	unsigned char* 	Get_Vect_Im	(void)		{ return imagen;     }
	void Sum_It_Imagens		(void)		{ codicImage++;      }
	int  Get_It_Imagens		(void)          { return codicImage; }
	void Init_It_Imagens		(void)          { codicImage=0;      }

	int  get_numMuestras		(void)		{ return numMuestras; }
	void set_numMuestras		(int nMuestras) { numMuestras=nMuestras; }

	int  Get_Estat_Enviament	(void)		{ return estat_enviament; }
	void Set_Estat_Enviament	(int estat) 	{ estat_enviament=estat; }
	int  Get_Resultat_Enviament	(void)		{ return resultat_enviament; }
	void Set_Resultat_Enviament	(int resultat) 	{ resultat_enviament=resultat; }

	int  Get_Estat_Guardar		(void)		{ return estat_guardar; }
	void Set_Estat_Guardar		(int estat) 	{ estat_guardar=estat; }
	int  Get_Resultat_Guardar	(void)		{ return resultat_guardar; }
	void Set_Resultat_Guardar	(int resultat) 	{ resultat_guardar=resultat; }


};



void*		Reproduir_Audio 			( void* );
void* 		crear_Servidor_SistemaVison_Master	( void* SV );
void* 		gestionComunicacion_SistemaVision	( void* context );
void*		CE_Guardar 				( void* SV );


int		getdir				( string dir, vector<string> &files);
static struct	udev_device* get_child		( struct udev* udev, struct udev_device* parent, const char* subsystem);
static void	enumerate_usb_mass_storage	( struct udev* udev);
void*		USB_thread			( void* );
int 		contadorCiclos			( void );
void 		cargarConfiguracio 		( void );






