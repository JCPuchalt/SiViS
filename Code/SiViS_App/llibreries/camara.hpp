#pragma once

#include "llibreries.hpp"
#include "graphics.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "lodepng.h"


using namespace cv;
using namespace std;

class sistema_visio;
class bufferCircular;
class Display;
class GfxTexture;
class objetos;

enum {espera_1, enviaTabla, enviaTextura };
enum {espera_2, enviaTablaInterseccion, enviaInterseccion };
enum {espera_3, quieroCalibrar, yaHeCalibrado};

/** 
 * @ Classe camara
 *
 *
*/
class camara
{

private:

	Display* display;
	clienteTCP_IP* cliente;

	pthread_t conexionServidor_id;
	sistema_visio* SV;

	int   idCam;
	int   idSV;
	int   num_camaras;
	int   numPort;
	int   config;

	int FYmin, FYmax;
	int maximIdx, minimIdx;

	const char* nameWin;

	// Enviar les imagens parcials al servidor
	bool tablaParcial_Enviada;
	

	unsigned char* evaluado;
	int estadoCalibracion;
	int ordenes1, ordenes2;
	int tamInterseccio;

	Mat interseccioGrisos;

	vector<objetos> objectes;
	vector<objetos>::iterator itObjectes;

	const char* ruta_rail;
	const char* ruta_proyecte;
	const char* ruta_dia;
	const char* ruta_proveta;
	const char* ruta_video;

	Mat dHi, dHiInv, MatGxP, MatGxPInv, dHiPlanoSuperior;
	Mat rectas, rectasFusionada;

	char ruta_patron[100];
	char ruta_homography[100];
	char ruta_homographyPlanoPlaca[100];
	char ruta_tablaCalibracion[100];
	char ruta_tablaInterseccion[100];

	int portMUX[3];

	bool existeArchivo;
	bool matriuCalibracioRebuda;
	bool matriuCalibracioEnviada;

	//
	bool tablaInterseccionParcial_Calculada;
	bool tablaInterseccionParcial_Enviada;
	bool tablaFusionadaParcial_Calculada;
	bool tablaInterseccionParcial_Recibida;

	double  erroCalibracio;
	int   CNTA, CNTFusion, It_Erro;
	float OFFSET_X; float OFFSET_Y; float escala;
	int   Iteracions_Compensacio, Iteracions_Imagens_Guardades;
	float corregir_x, corregir_y;
	float minim_error_compensacio, mija_error_compensacio, alpha;
	double errorQuadraticMig_Anterior;
	float MET_Ant; // Mija Error Total Anterior (Compensacio)
	bool  min_found;
	int   TAM;
	int tamInterseccionParcial;

	//Variables d'Estat
	bool calibrada, recalibrar;	int estat_calibracio;
	bool PREPARADA_COMUNICAR; bool enviaInfoAlMaster; bool enviaInfoAlPrincipal;
	bool reset_comp; bool compensada;

	GfxTexture  mostrar_tex, pantalla_grey;
	GfxTexture* ImagenDisplayTextura;

	char		*matriuRebuda;
	char		*tablaAenviar;
	char		*tablasXenviar;
	char		*tablaAenviarImageParcial;

	unsigned char	*ImagenDisplayVector;
	long int 	*tabla_calibracion;
	long int 	*tabla_calibracionDirecta;
	int 		*tablaInterseccionIndividual;
	long int	*tablaInterseccionParcial;
	long int	*tablaInterseccionParcial_C;
	long int 	*tabla_calibracionFusion;
	unsigned char	*texturaComprimida;
	unsigned char	*detectadaInterseccion;

	long int* tablaFusionadaParcial;
	int tamFusionadaParcial;

	unsigned char	*quadrat;
	unsigned char	*imageInterseccioComprimida;
	unsigned char	*imageInterseccioParcial;
	unsigned char	*imageInterseccio;
	unsigned char	*segmentada;
	unsigned char	*circul;
	unsigned char	*imageParcial;
	unsigned char 	*image_desti;
	unsigned char 	*vector_mostrar_ant;
	unsigned char 	*vector_mostrar_minim_error;
	unsigned char   *imagenPROVA;

	bool bloquejat;
	bool capturando, inici_local;
	bool inici_remot, iniciat;
	bool fin_captura;

public:
	camara(int iD, int num_cam, sistema_visio* SiVi );
	camara(int iD, int num_cam );
	~camara() {}

	int  GetId		(void) 						{ return idCam; }
	void SetId		(int id) 					{ idCam=id; }
	void setCaracteristicas	(int numCams, int id, int nPort, int conf)	{ idSV=id; num_camaras=numCams; numPort=nPort; config=conf; }
	void setTamInterseccionParcial (int valor)				{ tamInterseccionParcial=valor; }
	int  get_FYmin		(void)						{ return FYmin; }
	int  get_FYmax		(void)						{ return FYmax; }
	int  get_maximIdx	(void)						{ return maximIdx; }
	int  get_minimIdx	(void) 						{ return minimIdx; }
	int  getLlimit		(void)						{ return (int)round(FYmax-FYmin)/2.0; }

	sistema_visio* get_SV	(void)						{ return SV; }

	long int* get_tablaInterseccionParcial	( void )			{ return tablaInterseccionParcial; }
	long int* get_tablaFusionadaParcial	( void )			{ return tablaFusionadaParcial;    }
	void 	  set_tamFusionadaParcial	( int valor )			{ tamFusionadaParcial = valor;     } 
	int 	  get_tamFusionadaParcial	( void )			{ return tamFusionadaParcial;	   }

	bool getEstatCompensada (void) 		{ return compensada; }
	int  getCapturando	(void)		{ return capturando; }

	int  getCNTA			(void)		{ return CNTA; }
	Mat* get_dHi			(void)		{ return &dHi; }
	int  getTamInterseccionParcial	(void)		{ return tamInterseccionParcial; }
	vector <objetos>* getObjectes	(void)		{ return &objectes; }
	Mat* get_interseccioGrisos	(void)		{ return &interseccioGrisos; }

	void Sum_It_Comp	(void)		{ Iteracions_Compensacio++; }
	int  Get_It_Comp	(void) 		{ return Iteracions_Compensacio; }
        void Init_It_Comp	(void)          { Iteracions_Compensacio=1; }

	void  crearFil (void);

	void  setDisplay			( Display* display );
	void* getImagenDisplayVector 		( void)						{ return ImagenDisplayVector;    }
	void  setPort 				( int p7, int p11, int p12 );
	void  setOFFSET				( float OFFX, float OFFY, float esc )		{ OFFSET_X = OFFX; OFFSET_Y = OFFY; escala = esc; }
	void  pEvaluado 			( unsigned char *eval )	 			{ evaluado=eval; }
	bool  ExisteTabla 			( void )					{ return existeArchivo; }
	bool  isCalibrada			( void )					{ return calibrada; }
	bool  isRecalibrada			( void )					{ return recalibrar; }
	void  pDetectarInterseccion		( unsigned char *puntero )	 		{ detectadaInterseccion = puntero; }
	bool  preparadaComunicar		( void )					{ return PREPARADA_COMUNICAR;}
	void  preparadaComunicar		( bool valor )					{ PREPARADA_COMUNICAR = valor; }
	void  finCapturaCamara			( void );

	void matrizCalibracionRecibida		( bool valor )					{ matriuCalibracioRebuda = valor; }
	bool isMatrizCalibracionRecibida	( void )					{ return matriuCalibracioRebuda;  }
	void matrizCalibracionEnviada		( bool valor )					{ matriuCalibracioEnviada = valor; }
	bool isMatrizCalibracionEnviada		( void )					{ return matriuCalibracioEnviada; }
	bool isTablaInterseccionParcial_Calculada (void)					{ return tablaInterseccionParcial_Calculada; }
	bool isTablaInterseccionParcial_Enviada	( void )					{ return tablaInterseccionParcial_Enviada; }

	void texturaBlanca (void);

	bool comunicacionRaspberries 		( int sock );
	bool enviarCabecera			( int sock );
	bool enviarTablaCalibracion		( int sock );
	void solicitarDibuixarPatro		( void );
	bool enviarTextura			( int sock );
	bool enviarTablaInterseccion		( int sock );
	bool enviarInterseccio			( int sock );
	void esperaEnviar			( void );
	void enviaAlMaster			( void )	{ enviaInfoAlMaster=true; }

	void cabeceraSincronizacion		( int sock, int tipoDato );
	void recibirMatrizCalibracion		( int sock, Mat *matriu, int m, int n );
	void enviarMatrizCalibracion		( int sock, Mat *matriu, int m, int n );

	// Enviar al Servidor
	bool enviarTablaImagenParcial		( int sock );
	bool enviarImageParcial			( int sock );
	void enviarInterseccioReconstruida 	( unsigned char* imageParcialEnInterseccio, int nouTamanyTabla  );

	// Enviar a l'atra Raspberry pi
	void enviarTablaInterseccionParcial	( int sock );
	void enviarInterseccionParcial		( int sock );
	void recibirTablaInterseccionParcial	( int sock );
	void recibirInterseccionParcial		( int sock );
	void enviaAl_SV_Principal 		( void )	{ enviaInfoAlPrincipal=true; }
	void esperarEnviarPrincipal		( void );
 
	void enviarImage			( void );

	void savePatron		( Mat Mpatron );
	void Sum_It_Imagens	(void)		{Iteracions_Imagens_Guardades++;}
	int  Get_It_Imagens	(void)          {return Iteracions_Imagens_Guardades;}
	void Init_It_Imagens	(void)          {Iteracions_Imagens_Guardades=0;}

        bool  found_min		(void)          { return min_found; }
        void  found_min		(bool found)    { min_found = found;  }

	float Get_Minim_Erro	(void)          { return minim_error_compensacio; }
	void  Set_Minim_Erro	(float minim)   { minim_error_compensacio=minim; }
	void  Init_Erro_Minim	(void)          { minim_error_compensacio=1000.0; }

	Point2f Px		( double tx,   double ty );
	Point2f PinvX		( double imx,  double imy );
	Point2f Inv_GxPx	( double FusX, double FusY );
	Point2f GxPx		( double Imx,  double Imy );


        void  Init_Erro_Mija	(void)		{ mija_error_compensacio=0.0; }
        void  Init_MET_Ant	(void)		{ MET_Ant=0.0; }

	float Get_It_Error	(void)          { return It_Erro; }
	void  Inc_It_Error	(void)          { It_Erro++; }
	void  Init_It_Error	(void)          { It_Erro=0; }
	void  Set_Reset_Comp	(bool valor)	{ reset_comp=valor; }
	bool  Get_Reset_Comp	(void)		{ return reset_comp; }

	void crear_ruta_homografia 		(void);
	void crear_ruta_homografiaPlanoPlaca 	(void);
	void crear_ruta_patron 			(void);
	void crear_ruta_tablaCalibracion	(void);
	void crear_ruta_tablaInterseccion 	(void);


	GfxTexture* 	Get_Textura		(void)	{ return &mostrar_tex;}
	unsigned char* 	Get_Vect_Im		(void)	{ return quadrat;}
        unsigned char* 	Get_Circulo		(void)	{ return circul;}
        int  		Get_TAM			(void)  { return TAM;}
	unsigned char* 	Get_Vect_Compensada	(void)	{ return vector_mostrar_ant; }
	unsigned char* 	Get_Vect_Opt_Comp 	(void)  { return vector_mostrar_minim_error; }

	bool  Init					( void );
	bool  Seleccio_CAM				( void );
	bool  CE_Calibrar				( void );
	bool  CE_CalibrarRemoto				( void );
	void  dibuixar_patro_calibracio 		( void );
	void  dibuixar_patro_segmentat			( void );

	bool  calcular_matriz_calibracion_inversa	( void );
	int   Calculo_Tabla_Calibracion 		( void );
	int   Calculo_Tabla_Calibracion 		( bool monoCamara );
	int   Calculo_Tabla_CalibracionDirecta		( void );
	int   calculo_Tabla_Fusionada_Parcial		( unsigned char* detectada, Point2f centroImagen );

	void  fussioParcial				( unsigned char* imagen, bool foto );
	void  fussioInterseccioParcial 			( unsigned char* imagen );

	int   calculo_Tabla_GXP 			( void );
	bool  CE_Capturar				( bool unaCaptura );
	void  fusionarImagen				( unsigned char* fusionada );
	long int  detectarCircunferencia		( Mat& nubePuntos, long int totalPuntos );
	int   getPixelImage 				( int idxTexel );
	void  crear_TablaInterseccion			( int idxTexel, int idxImage, int cntCam);
	void  leer_TablaInterseccion			( void );
	void  compresionImagenInterseccion		( void );

	void  procesarInterseccionParcial		( bool foto );
	bool  detectarObjecto 				( Mat *imageOut        , Mat *imageIn          , bool foto );
	void  thresh_callback				( Mat &image_output    , Mat &threshold_output , Mat &imageGrisos, bool grava );
	void  binarisar 				( Mat &image_binarisada, Mat &image_grey       , bool grava);

	void obtenerEcuacionDeLaRecta			( Point2f puntoA , Point2f puntoB, Mat& recta, int idRecta );
	void obtenerEcuacionDeLaRectaFusionada		( Point2f puntoA , Point2f puntoB, Mat& recta, int idRecta );

	double getM 				(int id) {return rectas.at<double>(id,0);}
	double getB 				(int id) {return rectas.at<double>(id,1);}

	void setMatGxP					( Mat& G );
	void prova					( void );
	void guardarTest				( const char* nombre, unsigned char* im, int colores, int Wi, int Hi );
	void guardarTest				( const char* nombre, unsigned char* im );
	void guardarTest				( const char* nombre );


	double computeReprojectionErrorsH		(const vector<Point2f>& imagePoints,
							const vector<Point2f>& displayPoints, 
							const Mat& dHi, float* corregir_x, float* corregir_y);

	void saveInverseHomographyParams		(vector<Point2f> displayPoints, vector<Point2f> imagePoints,
							 Mat dHi, double displayPointsErr);

	void loadHomographyInverseParams(void);
	void loadHomographyNivelPlaca	(void);
	void savePatron			(void);
	void loadPatron			(void);
	bool loadTablaCalibracion	(void);
	bool saveTablaCalibracion	(void);
	void save_TablaInterseccion	(void);
	bool load_TablaInterseccion	(void);

	bool  CE_Compensar			 	( void );
        bool  CE_Compensar_Rapido			( void );
	bool  CE_Error_Compensacion 			( void );
	float Calcular_Error_Compensacion 		( void );
	float Calcular_Luz_Media 			( void );
	long int cannyUmbral				( Mat& nubePuntos, long int totalPuntos );
	void  segmentar					( int umbralColor );

	int TransformarImagen_Y_CompensarDisplay	( bool atomaticStop );
	int TransformarImagen_Y_CompensarDisplay	( void );

	void  Init_Textura_Groc 			( void );
        void  Init_Textura     			  	( void );


	void		mensaje 			( unsigned char* circul, int sock );
	
	int  		Configuracio_Remota		( int sock, int tamanyo, float temperatura, int Id, int NUM_IMAGE );
	void		inicialisarCaptura		( void );
	void 		sincronisacio			( void );
};

void*	clienteRASPBERRY	( void* SistVis );
void* 	servidorRASPBERRY	( void* SistVis );
void*	clienteSistemaVision 	( void* CAMARA );
void*	Conexion_Servidor	( void* CAMARA );


