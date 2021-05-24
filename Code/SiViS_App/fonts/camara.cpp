#pragma once

#include "../llibreries/llibreries.hpp"
#include "../llibreries/camara.hpp"


#ifdef _DEBUG    
#pragma comment(lib, "opencv_core246d.lib")     
#pragma comment(lib, "opencv_imgproc246d.lib")   //MAT processing    
#pragma comment(lib, "opencv_highgui246d.lib")    
#pragma comment(lib, "opencv_stitching246d.lib");  
  
#else    
#pragma comment(lib, "opencv_core246.lib")    
#pragma comment(lib, "opencv_imgproc246.lib")    
#pragma comment(lib, "opencv_highgui246.lib")    
#pragma comment(lib, "opencv_stitching246.lib");  
#endif  

using namespace cv;
using namespace std;

/** Histograma de Referencia **/
double H_Referencia[256]={0};


float puntos_pantalla[90][2]=
{
			     {0, 0}, {42*1*texelRatio, 7*42},	{0, 0}, {42*3*texelRatio, 7*42}, 	{0, 0}, {42*5*texelRatio,  7*42},   {0, 0},  {42*7*texelRatio,  7*42},   {0, 0},  {42*9*texelRatio,   7*42},   {0, 0},
	{0, 42*6}, 	     {0, 0}, {42*2*texelRatio, 6*42}, 	{0, 0}, {42*4*texelRatio, 6*42}, 	{0, 0}, {42*6*texelRatio,  6*42},   {0, 0},  {42*8*texelRatio,  6*42},   {0, 0},  {42*10*texelRatio,  6*42},
	
			     {0, 0}, {42*1*texelRatio, 5*42},	{0, 0}, {42*3*texelRatio, 5*42},	{0, 0}, {42*5*texelRatio,  5*42},   {0, 0},  {42*7*texelRatio,  5*42},   {0, 0},  {42*9*texelRatio,   5*42},   {0, 0},
	{0, 42*4}, 	     {0, 0}, {42*2*texelRatio, 4*42}, 	{0, 0}, {42*4*texelRatio, 4*42},	{0, 0}, {42*6*texelRatio,  4*42},   {0, 0},  {42*8*texelRatio,  4*42},   {0, 0},  {42*10*texelRatio,  4*42},

			     {0, 0}, {42*1*texelRatio, 3*42}, 	{0, 0}, {42*3*texelRatio, 3*42},	{0, 0}, {42*5*texelRatio,  3*42},   {0, 0},  {42*7*texelRatio,  3*42},   {0, 0},  {42*9*texelRatio,   3*42},   {0, 0},
	{0, 42*2},  	     {0, 0}, {42*2*texelRatio, 2*42},  	{0, 0}, {42*4*texelRatio, 2*42},	{0, 0}, {42*6*texelRatio,  2*42},   {0, 0},  {42*8*texelRatio,  2*42},   {0, 0},  {42*10*texelRatio,  2*42},

			     {0, 0}, {42*1*texelRatio,   42}, 	{0, 0}, {42*3*texelRatio, 1*42},	{0, 0}, {42*5*texelRatio,  1*42},   {0, 0},  {42*7*texelRatio,  1*42}, 	 {0, 0},  {42*9*texelRatio,   1*42},   {0, 0},
	{0,    0}, 	     {0, 0}, {42*2*texelRatio,    0}, 	{0, 0}, {42*4*texelRatio,    0},	{0, 0}, {42*6*texelRatio,     0},   {0, 0},  {42*8*texelRatio,     0},   {0, 0},  {42*10*texelRatio,     0}                     

};


/***************************************************************************************
Constructuor de Clase: "camara". Inicio de clase. Crea, inicializa... variables
****************************************************************************************/
camara::camara( int iD, int numero_camaras, sistema_visio* SiVi )
{
	H_Referencia[43]=0.1;
	H_Referencia[44]=0.3;
	H_Referencia[45]=1.2;
	H_Referencia[46]=5.5;
	H_Referencia[47]=16.5;
	H_Referencia[48]=28.0;
	H_Referencia[49]=25.5;
	H_Referencia[50]=13.8;
	H_Referencia[51]=5.6;
	H_Referencia[52]=1.8;
	H_Referencia[53]=0.5;
	H_Referencia[54]=0.1;

	corregir_x=0; corregir_y=0; CNTA=0;
    	Iteracions_Compensacio = 1;
	minim_error_compensacio = 100.0;
    	MET_Ant=0; mija_error_compensacio=0;
    	It_Erro=0; min_found=false;
    	alpha=0; tamInterseccio=0; tamFusionadaParcial=0;
	errorQuadraticMig_Anterior=0;
	tamInterseccionParcial=0;
	enviaInfoAlMaster=false;
	enviaInfoAlPrincipal=false;
	tablaParcial_Enviada=false;
	num_camaras = numero_camaras;

	bloquejat=false; capturando=false; inici_local=false;
	inici_remot=false; iniciat=false; fin_captura=false;

	compensada=true;
	SV = SiVi; 

	FYmin=0; FYmax=0;
	minimIdx = 10000000;
	maximIdx = 0;

	reset_comp=false;
	calibrada=false; recalibrar=false;
	matriuCalibracioRebuda=false;
	matriuCalibracioEnviada=false;
	PREPARADA_COMUNICAR = false;

	tablaInterseccionParcial_Calculada=false; tablaInterseccionParcial_Enviada=false;
	tablaFusionadaParcial_Calculada=false;   tablaInterseccionParcial_Recibida=false;

	idCam = iD;
	nameWin="visor";

	//
	if(MODO_ENFOQUE) {
		//namedWindow(nameWin,0);
	}

	escala=1;
	ordenes1=0;
	ordenes2=0;
	estadoCalibracion=0;

	mostrar_tex.CreateRGBA ( 431, 480 ); //WIDTH_PANT, HEIGHT_PANT );
	mostrar_tex.GenerateFrameBuffer();


	strcpy(ruta_homography		, "./configuracio/homografia");
	strcpy(ruta_homographyPlanoPlaca, "./h");
	strcpy(ruta_patron		, "./configuracio/p");
	strcpy(ruta_tablaCalibracion    , "./configuracio/tablaCalibracion");
	strcpy(ruta_tablaInterseccion   , "./configuracio/tablaInterseccion");

	//
	matriuRebuda			= (char*) malloc ( sizeof(char) * 300 );
	tablaAenviar 			= (char*) malloc ( sizeof(int) * 900*480*2 );
	tablasXenviar			= (char*) malloc ( sizeof(int) * 500000);
	tablaAenviarImageParcial 	= (char*) malloc ( sizeof(int) * 3000*3000 );
	imageParcial			= (unsigned char*) malloc ( sizeof(char) * 3000*3000  );

	tablaFusionadaParcial 		= (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * 2);
	memset ( tablaFusionadaParcial, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * 2);

	tablaInterseccionParcial	= (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);
	memset ( tablaInterseccionParcial, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);

	tablaInterseccionParcial_C	= (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);
	memset ( tablaInterseccionParcial_C, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);

	tabla_calibracion		= (long int*) 	   malloc (sizeof(long int)  * 900*480*2);
	//tabla_calibracionDirecta	= (long int*) 	   malloc (sizeof(long int)  * WIDTH_FRAME*HEIGHT_FRAME);
	tabla_calibracionFusion		= (long int*) 	   malloc (sizeof(long int)  * WIDTH_FRAME*HEIGHT_FRAME*4);
	tablaInterseccionIndividual	= (int*) 	   malloc (sizeof(int)  * 200000);


	//segmentada			= (unsigned char*) malloc (sizeof(char)      * WIDTH_FRAME*HEIGHT_FRAME );
	// Buffer de la imagen a tratar (capturada) Su resolución puede variar
	quadrat				= (unsigned char*) malloc (sizeof(char) * 12182016 ); //2592*2592); //WIDTH_FRAME * HEIGHT_FRAME ); //12182016
	memset(quadrat,  0, sizeof(char)      *  12182016 );

	// Imagen comprimida (de quadrat) de la zona de intersección. Esta zona es pequeña,
	// la dividimos por 30 del tamaño total (es un porcentaje estimado )
	imageInterseccioComprimida	= (unsigned char*) malloc (sizeof(char)      * 431*480*4 );
	memset(imageInterseccioComprimida, 0, sizeof(char) * 431*480*4 );

	// Imagen de la intersección
	imageInterseccioParcial	= (unsigned char*) malloc (sizeof(char)      * 500000 );
	memset(imageInterseccioParcial, 0, sizeof(char)*500000 );

	vector_mostrar_ant 		= (unsigned char*) malloc (sizeof(char)      * 431*480*4);
	vector_mostrar_minim_error 	= (unsigned char*) malloc (sizeof(char)      * 431*480*4);

	texturaComprimida		= (unsigned char*) malloc (sizeof(char)      * 900*480*2);
	imagenPROVA 			= (unsigned char *) malloc(sizeof(char)	     *2592*(500) );

	//Init_Textura_Groc();

	memset(tabla_calibracion,        0, sizeof(int)*861*480*2);
			//memset(tabla_calibracionDirecta, 0, sizeof(int)*WIDTH_FRAME*HEIGHT_FRAME);
	memset(tablaInterseccionIndividual, 0, sizeof(int)*200000 );
	memset(tabla_calibracionFusion,  0, sizeof(int)*WIDTH_FRAME*HEIGHT_FRAME*4);
	memset(texturaComprimida, '\0', sizeof(char)*900*480*2 );

//	pthread_t reproduir_id;
//	pthread_create(&reproduir_id, NULL, Reproduir_Audio, NULL );


	//if ( idCam == idCam_Ppal ||  withMUX) 
	// Si la RPI NO es Master y se trabaja sin MUX se crearán hilos de conexión 
	// con el display del Máster
	if( idCam==idCam_Ppal && !withMUX ) //!esMaster && idCam==idCam_Ppal && !withMUX)
	{
		cout << "\033[1;31mConectando: " << idCam << " con el Display Master" << "\033[0m"<< endl;
		pthread_t clienteRASP_id;
		pthread_create(&clienteRASP_id, NULL, clienteRASPBERRY, this );
	}

	// Se crea si es Sistema Vision Secundario y se trabaja sin MUX
	if( SV_Secundario )  // idCam == idCam_Ppal && idCam!=1
	{
		cout << "\033[1;31mConectando " << idCam << " con Sistema de Visión Principal" << "\033[0m" << endl;
		pthread_t clienteSV_id;
		pthread_create(&clienteSV_id, NULL, clienteSistemaVision, this );
	}

	// Se crea una conexio en atre programa per a poder enfocar les imagens
	if(MODO_ENFOQUE) {
		cliente = new clienteTCP_IP(this);
	}

	Init_It_Imagens();

	Init_Erro_Minim();
	Init_It_Error();
	Init_Erro_Mija();
	
	// Si se treballa en MUX <<<<<<<<<<<<<<<----------------------- Si es MONO-CAMARA
	if( idCam==idCam_Ppal || withMUX ) {
		crearFil();
	}

}


/***************************************************************************************
Constructuor de Clase: "camara". Inicio de clase. Crea, inicializa... variables
****************************************************************************************/
camara::camara( int iD, int numero_camaras )
{
	H_Referencia[43]=0.1;
	H_Referencia[44]=0.3;
	H_Referencia[45]=1.2;
	H_Referencia[46]=5.5;
	H_Referencia[47]=16.5;
	H_Referencia[48]=28.0;
	H_Referencia[49]=25.5;
	H_Referencia[50]=13.8;
	H_Referencia[51]=5.6;
	H_Referencia[52]=1.8;
	H_Referencia[53]=0.5;
	H_Referencia[54]=0.1;

	corregir_x=0; corregir_y=0; CNTA=0;
    	Iteracions_Compensacio = 1;
	minim_error_compensacio = 100.0;
    	MET_Ant=0; mija_error_compensacio=0;
    	It_Erro=0; min_found=false;
    	alpha=0; tamInterseccio=0; tamFusionadaParcial=0;
	errorQuadraticMig_Anterior=0;
	tamInterseccionParcial=0;
	enviaInfoAlMaster=false;
	enviaInfoAlPrincipal=false;
	num_camaras = numero_camaras;
	compensada=false;

	FYmin=0; FYmax=0;
	minimIdx = 10000000;
	maximIdx = 0;

	reset_comp=false;
	calibrada=false; recalibrar=false;
	matriuCalibracioRebuda=false;
	matriuCalibracioEnviada=false;
	PREPARADA_COMUNICAR = false;

	tablaInterseccionParcial_Calculada=false; tablaInterseccionParcial_Enviada=false;
	tablaFusionadaParcial_Calculada=false;   tablaInterseccionParcial_Recibida=false;

	idCam = iD;
	nameWin="visor";

	//
	if(MODO_ENFOQUE) {
		//namedWindow(nameWin,0);
	}

	escala=1;
	ordenes1=0;
	ordenes2=0;
	estadoCalibracion=0;

	mostrar_tex.CreateRGBA ( 431, 480 ); //WIDTH_PANT, HEIGHT_PANT );
	mostrar_tex.GenerateFrameBuffer();


	strcpy(ruta_homography		, "./configuracio/homografia");
	strcpy(ruta_homographyPlanoPlaca, "/home/pi/h");
	strcpy(ruta_patron		, "./configuracio/p");
	strcpy(ruta_tablaCalibracion    , "./configuracio/tablaCalibracion");
	strcpy(ruta_tablaInterseccion   , "./configuracio/tablaInterseccion");

	//
	matriuRebuda			= (char*) malloc ( sizeof(char) * 300 );
	tablaAenviar 			= (char*) malloc ( sizeof(int) * 900*480*2 );
	tablaAenviarImageParcial 	= (char*) malloc ( sizeof(int) * 3000*3000 );
	tablasXenviar			= (char*) malloc ( sizeof(int) * 500000);
	imageParcial			= (unsigned char*) malloc ( sizeof(char) * 3000*3000  );

	tablaFusionadaParcial 		= (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * 2);
	memset ( tablaFusionadaParcial, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * 2);

	tablaInterseccionParcial	= (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);
	memset ( tablaInterseccionParcial, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);

	tablaInterseccionParcial_C	= (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);
	memset ( tablaInterseccionParcial_C, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras);

	tabla_calibracion		= (long int*) 	   malloc (sizeof(long int)  * 900*480*2);
	//tabla_calibracionDirecta	= (long int*) 	   malloc (sizeof(long int)  * WIDTH_FRAME*HEIGHT_FRAME);
	tabla_calibracionFusion		= (long int*) 	   malloc (sizeof(long int)  * WIDTH_FRAME*HEIGHT_FRAME*4);
	tablaInterseccionIndividual	= (int*) 	   malloc (sizeof(int)  * 200000);


	//segmentada			= (unsigned char*) malloc (sizeof(char)      * WIDTH_FRAME*HEIGHT_FRAME );
	// Buffer de la imagen a tratar (capturada) Su resolución puede variar
	quadrat				= (unsigned char*) malloc (sizeof(char) * 12182016 ); //2592*2592); //WIDTH_FRAME * HEIGHT_FRAME ); //12182016
	memset(quadrat,  0, sizeof(char)      *  12182016 );

	// Imagen comprimida (de quadrat) de la zona de intersección. Esta zona es pequeña,
	// la dividimos por 30 del tamaño total (es un porcentaje estimado )
	imageInterseccioComprimida	= (unsigned char*) malloc (sizeof(char)      * 431*480*4 );
	memset(imageInterseccioComprimida, 0, sizeof(char) * 431*480*4 );

	// Imagen de la intersección
	imageInterseccioParcial	= (unsigned char*) malloc (sizeof(char)      * 500000 );
	memset(imageInterseccioParcial, 0, sizeof(char)*500000 );

	vector_mostrar_ant 		= (unsigned char*) malloc (sizeof(char)      * 431*480*4);
	vector_mostrar_minim_error 	= (unsigned char*) malloc (sizeof(char)      * 431*480*4);

	texturaComprimida		= (unsigned char*) malloc (sizeof(char)      * 900*480*2);
	imagenPROVA 			= (unsigned char *) malloc(sizeof(char)	     *2592*(500) );

	//Init_Textura_Groc();

	memset(tabla_calibracion,        0, sizeof(int)*861*480*2);
			//memset(tabla_calibracionDirecta, 0, sizeof(int)*WIDTH_FRAME*HEIGHT_FRAME);
	memset(tablaInterseccionIndividual, 0, sizeof(int)*200000 );
	memset(tabla_calibracionFusion,  0, sizeof(int)*WIDTH_FRAME*HEIGHT_FRAME*4);
	memset(texturaComprimida, '\0', sizeof(char)*900*480*2 );

//	pthread_t reproduir_id;
//	pthread_create(&reproduir_id, NULL, Reproduir_Audio, NULL );


	//if ( idCam == idCam_Ppal ||  withMUX) 
	// Si la RPI NO es Master y se trabaja sin MUX se crearán hilos de conexión 
	// con el display del Máster
	if( idCam==idCam_Ppal && !withMUX ) //!esMaster && idCam==idCam_Ppal && !withMUX)
	{
		cout << "\033[1;31mConectando: " << idCam << " con el Display Master" << "\033[0m"<< endl;
		pthread_t clienteRASP_id;
		pthread_create(&clienteRASP_id, NULL, clienteRASPBERRY, this );
	}

	// Se crea si es Sistema Vision Secundario y se trabaja sin MUX
	if( SV_Secundario )  // idCam == idCam_Ppal && idCam!=1
	{
		cout << "\033[1;31mConectando " << idCam << " con Sistema de Visión Principal" << "\033[0m" << endl;
		pthread_t clienteSV_id;
		pthread_create(&clienteSV_id, NULL, clienteSistemaVision, this );
	}

	// Se crea una conexio en atre programa per a poder enfocar les imagens
	if(MODO_ENFOQUE) {
		cliente = new clienteTCP_IP(this);
	}

	Init_It_Imagens();

	Init_Erro_Minim();
	Init_It_Error();
	Init_Erro_Mija();

}


void camara::crearFil (void)
{
	// Crear Una conexio al Servidor. Per a cada Sistema de Visio
	//fils* fil = new fils();
	//fil->StartInternalThread(this);
	cout << "GetId(): " << this->GetId() << endl;
	pthread_create ( &conexionServidor_id, NULL, Conexion_Servidor, this );
	cout << "pthread: " << conexionServidor_id << endl;
}



/************************************************
Fixar el Display
************************************************/
void camara::setDisplay (Display* disp)
{
	display=disp;

	ImagenDisplayVector  = display->pImagenDisplayVector();
	ImagenDisplayTextura = display->pImagenDisplayTextura();

}


/************************************************
Crear la ruta de la homografia
************************************************/
void camara::crear_ruta_homografia (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_homography, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_homography, sistVis.c_str() );

	string sancer = int2string(idCam);
	strcat( ruta_homography, sancer.c_str() );

	strcat( ruta_homography,".xml");

	cout << ruta_homography << endl;
}


/************************************************
Crear la ruta de la homografia
************************************************/
void camara::crear_ruta_homografiaPlanoPlaca (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_homographyPlanoPlaca, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_homographyPlanoPlaca, sistVis.c_str() );

	string sancer = int2string(idCam);
	strcat( ruta_homographyPlanoPlaca, sancer.c_str() );

	strcat( ruta_homographyPlanoPlaca,".xml");

	//cout << ruta_homographyPlanoPlaca << endl;
}



/************************************************
Crear la ruta del patron
************************************************/
void camara::crear_ruta_patron (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_patron, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_patron, sistVis.c_str() );

	string sancer = int2string(idCam);
	strcat( ruta_patron, sancer.c_str() );

	strcat( ruta_patron,".xml");

	//cout  << ruta_patron << endl;
}


/************************************************
Crear la ruta de la tabla de calibracion
************************************************/
void camara::crear_ruta_tablaCalibracion (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_tablaCalibracion, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_tablaCalibracion, sistVis.c_str() );

	string sancer = int2string(idCam);
	strcat( ruta_tablaCalibracion, sancer.c_str() );

	strcat( ruta_tablaCalibracion,".cnf");

	//cout  << ruta_tablaCalibracion << endl;

}


/************************************************
Crear la ruta de la tabla de interseccion
************************************************/
void camara::crear_ruta_tablaInterseccion (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_tablaInterseccion, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_tablaInterseccion, sistVis.c_str() );

	string sancer = int2string(idCam); //numPort
	strcat( ruta_tablaInterseccion, sancer.c_str() );

	strcat( ruta_tablaInterseccion,".cnf");

	//cout  << ruta_tablaInterseccion << endl;

}

/************************************************
Fixar el port de la camara
************************************************/
void camara::setPort 	(int p7, int p11, int p12)
{
	portMUX[0] = p7; portMUX[1] = p11; portMUX[2] = p12;
}


/************************************************
Inicialisar Textura
************************************************/
void camara::Init_Textura_Groc(void)
{
    for(int k=0; k<431*480*4; k=k+4)
    {
        vector_mostrar_ant[k]  =   0;
        vector_mostrar_ant[k+1]=   0;
        vector_mostrar_ant[k+2]=   0;
        vector_mostrar_ant[k+3]= 255;
    }

    for(int k=0; k<431*480*4; k=k+4)
    {
        vector_mostrar_minim_error[k]  = 255;
        vector_mostrar_minim_error[k+1]= 127;
        vector_mostrar_minim_error[k+2]=   0;
        vector_mostrar_minim_error[k+3]= 255;
    }

    mostrar_tex.SetPixels( vector_mostrar_ant );

}


/************************************************
Textura Blanca
************************************************/
void camara::texturaBlanca(void)
{

    for(int cnt=0; cnt<CNTA; cnt=cnt+2 )
    {
        long int posIM  = tabla_calibracion[cnt];
        long int posTEX = tabla_calibracion[cnt+1];

         ImagenDisplayVector[posTEX]   = 255;
         ImagenDisplayVector[posTEX+1] = 255;
         ImagenDisplayVector[posTEX+2] = 255;
         ImagenDisplayVector[posTEX+3] = 255;
    }

}

/************************************************
Textura Blanca
************************************************/
void sistema_visio::texturaColor(int Rc, int Gc, int Bc )
{

    for(int k=0; k<855*480*4; k=k+4 )
    {

         ImagenDisplayVector[k]   = Rc;
         ImagenDisplayVector[k+1] = Gc;
         ImagenDisplayVector[k+2] = Bc;
         ImagenDisplayVector[k+3] = 255;
    }

}


/************************************************
Inicialisar Fondo
************************************************/
void camara::Init_Textura(void)
{

	for(int cnt=0; cnt<CNTA; cnt=cnt+2 )
	{
		long int posIM  = tabla_calibracion[cnt];
		long int posTEX = tabla_calibracion[cnt+1];

		ImagenDisplayVector[posTEX]   = 255;
		ImagenDisplayVector[posTEX+1] =  50;
		ImagenDisplayVector[posTEX+2] =   0;
		ImagenDisplayVector[posTEX+3] = 255;
	}
}


/************************************************
Selección de CAMARA
************************************************/
bool camara::Seleccio_CAM(void)
{

	digitalWrite( pin_7 ,  portMUX[0]);	//Selection
	digitalWrite( pin_11,  portMUX[1]);	//Enable 1
	digitalWrite( pin_12,  portMUX[2]);	//Enable 2

}


/************************************************
 Procedimiento para Calibrar Display/Cámara
************************************************/
bool camara::CE_Calibrar (void)
{

	existeArchivo = existe(ruta_tablaCalibracion);

	Seleccio_CAM();
	cout << portMUX[0] << portMUX[1] << portMUX[2] << endl;
	int N_Ite=4;

	if(esMaster || withMUX ) {
		//dibuixar_patro_calibracio();
		cout << "Dibuixa patro de calibracio" << endl;
		solicitarDibuixarPatro();
		display->dibuixarPatroCalibracio(OFFSET_X, OFFSET_Y, escala);
		display->dibuixar();
	}
	if(!esMaster) {
		solicitarDibuixarPatro();
	}
	usleep(500000);
	CE_Capturar(false);

	if(!calibrada)
	{
		while( estadoCalibracion == quieroCalibrar )
		//for ( int cnt_bucle=0 ; cnt_bucle<=N_Ite ; cnt_bucle++ )
		{
			cout << "Calibrando cámara local id: " << idCam << " -> " << ruta_homography << endl;
			bool unaCaptura=true;
			display->dibuixar();
			StopCamera();
			cam = StartCamera( WIDTH_HD, HEIGHT_HD, 7,1,false );
			CE_Capturar(unaCaptura);
			guardarTest("./PatroNou", quadrat);
			//sleep(2);
			// CALCULA HOMOGRAFIA (MATRIU DE CALIBRACIO)
			if( calcular_matriz_calibracion_inversa() )
			{
				estadoCalibracion = yaHeCalibrado;
				calibrada=true;		
				//break;
			}
		}
	}
	display->colorDisplay(0,0,0,255);
	display->dibuixar();
/*
	// CALCULA LA TABLA ASSOCIADA A LA MATRIU DE CALIBRACIO (MENYS TEMPS COMPUTACIONAL)
	if( (calibrada && recalibrar) || !existeArchivo ){
		Calculo_Tabla_Calibracion();
				//Calculo_Tabla_CalibracionDirecta();
		Init_Textura();
	}

//	if(!recalibrar){
		loadTablaCalibracion();
//	}
*/
	return calibrada;

}


bool camara::CE_CalibrarRemoto (void)
{
	cout << "Comunicando calibracion id: " << idCam << " -> " << ruta_homography << endl;
	dHiInv=dHi.inv();
	//display->colorDisplay(0,0,0,255);
	display->dibuixar();
	// Falta la Matriz de Calibracion del plano de los C-Elegans
	
	return true;
}

/*******************************************************************
	Dibuixa el patro en el display per a calcular la calibracio
********************************************************************/
void camara::dibuixar_patro_calibracio (void)
{
	float pantalla_x, pantalla_y;

	BeginFrame();

	int fonfo=0;
	ILLUMINACIO (ImagenDisplayTextura, 0,0,0, NULL);
	for(int ite=0; ite<90; ite++) //42
	{
		if(ite%2) //UNO SÍ UNO NO
		{
			pantalla_x = puntos_pantalla[ite][0]*escala +  OFFSET_X;
			pantalla_y = puntos_pantalla[ite][1]*escala +  OFFSET_Y;

			DrawPuntRect    ( ImagenDisplayTextura, pantalla_x, pantalla_y, ImagenDisplayTextura );
			DrawTextureRect ( ImagenDisplayTextura, -1.0f, -1.0f, 1.0f, 1.0f, NULL);
		}
	}	
	EndFrame();
}


/*******************************************************************
 Calcular la Matriz de Calibracion Inversa entre Display y Cámara
********************************************************************/
bool camara::calcular_matriz_calibracion_inversa( void )
{
	bool found = false;
	unsigned char* image_wb = (unsigned char*) malloc (sizeof(char)*WIDTH_FRAME*HEIGHT_FRAME);
	memset( image_wb, 255, WIDTH_FRAME*HEIGHT_FRAME);

	Size imageSize;

	vector<Point2f> imagePoints;
	vector<Point2f> displayPoints;

	Size boardSize;
	boardSize.height = 11; //7;
	boardSize.width  = 4;  //4; //3;

	float squareSize;            // The size of a square in your defined unit (point, millimeter,etc).
	squareSize = 42*escala;
	
	for( int cnt=0; cnt<WIDTH_FRAME*HEIGHT_FRAME; cnt++ )
		if( quadrat[cnt]>20 )
			image_wb[cnt]=0;

	guardarTest("./Patern", image_wb);

	Mat view = Mat (Size(WIDTH_FRAME,HEIGHT_FRAME), CV_8UC1, image_wb );

	if(!withMUX) {
		loadHomographyInverseParams();
//		loadHomographyNivelPlaca();
	} 

	if (!view.empty())
	{
		imageSize = view.size();  // Format input image.
		
		SimpleBlobDetector::Params params;
		params.filterByArea=true;
		params.minArea=250;
		params.maxArea=50000;
		params.filterByCircularity = false;
		params.filterByInertia = false;
		params.filterByConvexity = false;
		cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params); 
		
		found = findCirclesGrid(view, boardSize, imagePoints, CALIB_CB_ASYMMETRIC_GRID, blobDetector);
		for (int kk=0; kk<imagePoints.size();kk++) {
			imagePoints.at(kk) = imagePoints.at(kk)*3.0;
			cout << imagePoints.at(kk) << endl;
		}
		
		if (found)             
		{
			cout << "Patrón Encontrado!!! Calibrando... " << imagePoints.size() << " puntos" << endl;
			// Crea Patrón
			for (int i=0; i<boardSize.height ; i++)
				for ( int j=0; j<boardSize.width; j++)
					displayPoints.push_back(Point2f( OFFSET_X + (i*squareSize*texelRatio), OFFSET_Y + ( ( 2*j + i%2)*squareSize) )); // 105.191061452

			// Crea la homografía
			Mat dHiActual = findHomography(displayPoints, imagePoints);
			double displayPointsErr = this->computeReprojectionErrorsH( displayPoints, imagePoints, dHiActual, &corregir_x, &corregir_y);

			//cout << "Error Actual: " << displayPointsErr << ", Error Guardado: " << erroCalibracio << endl;

			// Si el error calculado es más grande que el guardo
			if( 1 ) //displayPointsErr > erroCalibracio)
			{
				cout << "Aplicar nueva calibración..." <<endl;
				dHi=dHiActual;
				this->saveInverseHomographyParams( imagePoints, displayPoints, dHiActual, displayPointsErr);

				// Aplicar desplaçament a l'homografia per a corregir l'error sistematic 
				/*Mat Trans = Mat::zeros( 3, 3, CV_64FC1);
				Trans.at<double>(0,0)=1;				Trans.at<double>(0,2)= corregir_x;
							   Trans.at<double>(1,1)= 1;	Trans.at<double>(1,2)= corregir_y;
											Trans.at<double>(2,2)= 1;
				dHi=Trans*dHiActual;
				displayPointsErr = this->computeReprojectionErrorsH( displayPoints, imagePoints, dHi, &corregir_x, &corregir_y);
				this->saveInverseHomographyParams( imagePoints, displayPoints, dHi, displayPointsErr);
				*/
				recalibrar=true;
			}

		}
		else{
			cout << "No ha calibrado!!! Cargando calibración guardada" << endl;
			loadHomographyInverseParams();
		}
	}

	dHiInv=dHi.inv();


	free(image_wb);
	return found;
}


/**************************************************************************************
 * @ Convierte la Matriz de calibracion en una Tabla de Asignacion (Tabla de Calibracion)
 * @ Lo hace para aquellos texels 
***************************************************************************************/
int camara::Calculo_Tabla_Calibracion ( void )
{
	
	//if(recalibrar || !existeArchivo)
	{
		int Ancho_Pant = (int)floor(ImagenDisplayTextura->GetWidth());
		int Alto_Pant  = (int)floor(ImagenDisplayTextura->GetHeight());

		Mat punt = Mat(3, 1, CV_64FC1);

		//loadHomographyInverseParams();

		int cnt = 0;
		double limiteEnY1, limiteEnY2, m1, b1;
		if(rectas.rows>0)
		{
			m1 = rectas.at<double>(1,0);
			b1 = rectas.at<double>(1,1);
		}

		// En función del Texel (del Display)
		for(int cnt_i=0; cnt_i<Ancho_Pant; cnt_i++ )
		{
			for(int cnt_j=0; cnt_j<Alto_Pant; cnt_j++ )
			{
				punt.at<double>(0,0)= (double)cnt_i; 
				punt.at<double>(1,0)= (double)cnt_j; 
				punt.at<double>(2,0)= 1.0;


				Mat punt_T = dHi*punt;

				int y = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
				int x = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

				if(rectas.rows>0 && idCam==1)
				{
					limiteEnY1 = (int)floor(m1 * cnt_i + b1 + 5);
					limiteEnY2 = (int)floor(m1 * cnt_i + b1);
				}
				if(rectas.rows>0 && idCam==2)
				{
					limiteEnY1 = (int)floor(m1 * cnt_i + b1 - 5);
					limiteEnY2 = (int)floor(m1 * cnt_i + b1);
				}
				//else limiteEnY1=cnt_j-100;


				if(x>=0 && x<HEIGHT_FRAME && y>=0 && y<WIDTH_FRAME && cnt_j>limiteEnY1 && idCam==1 )
				{
					
					long int posIM  = x*WIDTH_FRAME + y;
					long int posTEX = (cnt_j*floor(Ancho_Pant) + cnt_i)*4;

					tabla_calibracion[cnt]   = posIM;
					tabla_calibracion[cnt+1] = posTEX;

					cnt=cnt+2;
				}
				if(x>=0 && x<HEIGHT_FRAME && y>=0 && y<WIDTH_FRAME && cnt_j<=limiteEnY1 && cnt_j>=limiteEnY2 && idCam==1 )
				{
					long int posTEX = (cnt_j*floor(Ancho_Pant) + cnt_i)*4;
					detectadaInterseccion[posTEX] = 1;
				}


				if(x>=0 && x<HEIGHT_FRAME && y>=0 && y<WIDTH_FRAME && cnt_j<limiteEnY1 && idCam==2 )
				{
				
					long int posIM  = x*WIDTH_FRAME + y;
					long int posTEX = (cnt_j*floor(Ancho_Pant) + cnt_i)*4;

					tabla_calibracion[cnt]   = posIM;
					tabla_calibracion[cnt+1] = posTEX;

					cnt=cnt+2;
				}

				if(x>=0 && x<HEIGHT_FRAME && y>=0 && y<WIDTH_FRAME && cnt_j>=limiteEnY1 && cnt_j<=limiteEnY2 && idCam==2 ) //&& cnt_j>limiteEnY2
				{
					long int posTEX = (cnt_j*floor(Ancho_Pant) + cnt_i)*4;
					detectadaInterseccion[posTEX] = 1;
				}
			}
		}

		CNTA = cnt;
		cout << "Tabla construida con tamaño: " << CNTA << " Bytes" << endl;
		saveTablaCalibracion();

		return cnt;
	}
}

int camara::Calculo_Tabla_Calibracion ( bool monoCamara )
{
	
	//if(recalibrar || !existeArchivo)
	{
		int Ancho_Pant = (int)floor(ImagenDisplayTextura->GetWidth());
		int Alto_Pant  = (int)floor(ImagenDisplayTextura->GetHeight());

		Mat punt = Mat(3, 1, CV_64FC1);

		//loadHomographyInverseParams();

		int cnt = 0;


		// En función del Texel (del Display)
		for(int cnt_i=0; cnt_i<Ancho_Pant; cnt_i++ )
		{
			for(int cnt_j=0; cnt_j<Alto_Pant; cnt_j++ )
			{
				punt.at<double>(0,0)= (double)cnt_i; 
				punt.at<double>(1,0)= (double)cnt_j; 
				punt.at<double>(2,0)= 1.0;

				Mat punt_T = dHi*punt;

				int y = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
				int x = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

				if(x>=0 && x<HEIGHT_FRAME && y>=0 && y<WIDTH_FRAME )
				{	
					long int posIM  = x*WIDTH_FRAME + y;
					long int posTEX = (cnt_j*floor(Ancho_Pant) + cnt_i)*4;

					tabla_calibracion[cnt]   = posIM;
					tabla_calibracion[cnt+1] = posTEX;

					cnt=cnt+2;
				}
				
			}
		}

		CNTA = cnt;
		cout << "Tabla construida con tamaño: " << CNTA << " Bytes" << endl;
		saveTablaCalibracion();

		return cnt;
	}
}

/************************************************************************
Calculo tabla de Fusion 2 CAM
*************************************************************************/
int camara::calculo_Tabla_Fusionada_Parcial ( unsigned char* detectada, Point2f centroImagen )
{

	int AnchoOrigen = WIDTH_FRAME;
	int AltoOrigen  = HEIGHT_FRAME;

	int AnchoFusion = 2592;
	int AltoFusion  = 2592;

	cout << "Creando tabla de Fusión Parcial: " << idCam << "... (para 2 cámaras)" << endl;
	

	double limiteEnY1=0, limiteEnY2=0;
	long int cnt = 0;
	long int posIM, posFus;
	Point2f puntImageFloat;

	double m1, b1;
	tamFusionadaParcial=0;

	m1=getM(0);
	b1=getB(0);

	tamInterseccionParcial=0;

	Point2f centro = Px(centroImagen.x,centroImagen.y);
	cout << "Centro de la imagen en Cámara: " << centro << endl; 
	if(rectas.rows>0 && idCam==1)
	{		
		limiteEnY1 = (int)floor(centro.y) + 70;
		limiteEnY2 = (int)floor(centro.y) - 70;
	}
	if(rectas.rows>0 && idCam==2)
	{
		limiteEnY1 = (int)floor(centro.y) - 70;
		limiteEnY2 = (int)floor(centro.y) + 70;
	}

	minimIdx = 10000000;
	maximIdx = 0;
	int idxMin=0, idxMax=0;	

	for(int FusionadaX=0; FusionadaX<AnchoFusion; FusionadaX++ )
	{
		for(int FusionadaY=0; FusionadaY<AltoFusion; FusionadaY++ )
		{
			puntImageFloat = Inv_GxPx((double)FusionadaX,(double)FusionadaY);
			Point2i puntImage;
			puntImage.x = (int)floor(puntImageFloat.x);
			puntImage.y = (int)floor(puntImageFloat.y);

			if(puntImage.x>=0 && puntImage.x<AnchoOrigen && puntImage.y>=0 && puntImage.y<AltoOrigen)
			{
				posFus = FusionadaY  *AnchoFusion + FusionadaX;
				posIM  = puntImage.y *AnchoOrigen + puntImage.x;
 
				if( idCam == 1 ) 
				{
					if( puntImage.y>=limiteEnY1 )
					{
						tablaFusionadaParcial [tamFusionadaParcial]   =  posIM;
						tablaFusionadaParcial [tamFusionadaParcial+1] =  posFus;
						tamFusionadaParcial = tamFusionadaParcial+2;
					}
					// Detectada Intersección
					if ( puntImage.y<limiteEnY1 && puntImage.y>limiteEnY2 )
					{
						//Crear_Tabla_Interseccion_Parcial
						tablaInterseccionParcial_C [tamInterseccionParcial] = posIM;
						tablaInterseccionParcial   [tamInterseccionParcial] = posFus;
						if( posFus < minimIdx )
						{
							minimIdx = posFus;
							idxMin   = tamInterseccionParcial;
						}
						if( posFus > maximIdx )
						{
							maximIdx = posFus;
							idxMax   = tamInterseccionParcial;
						} 
						tamInterseccionParcial++;
					}
							
				}
				if( idCam == 2 )
				{
					if( puntImage.y<=limiteEnY1 ) //puntImage.y<limiteEnY1
					{
						tablaFusionadaParcial [tamFusionadaParcial+0]  =  posIM;
						tablaFusionadaParcial [tamFusionadaParcial+1]  =  posFus;
						tamFusionadaParcial = tamFusionadaParcial+2;
					}
					// Detectada Intersección
					if ( puntImage.y>limiteEnY1 && puntImage.y<limiteEnY2 )
					{
						//Crear_Tabla_Interseccion_Parcial
						tablaInterseccionParcial_C [tamInterseccionParcial] = posIM;
						tablaInterseccionParcial   [tamInterseccionParcial] = posFus;
						if( posFus < minimIdx )
						{
							minimIdx = posFus;
							idxMin   = tamInterseccionParcial;
						}
						if( posFus > maximIdx )
						{
							maximIdx = posFus;
							idxMax   = tamInterseccionParcial;
						} 
						tamInterseccionParcial++;
					}	
				}
			}
		}
	}

	FYmin = (int)floor(minimIdx/2592.0);
	FYmax = (int)floor(maximIdx/2592.0);

	cout << "·" << idxMin << "-> minim: " << minimIdx << " | ·" << idxMax << "-> maxim: " << maximIdx << endl;
	tablaFusionadaParcial_Calculada = true;
	tablaInterseccionParcial_Calculada=true;
	cout << "pixelesConvert Iterseccion Parcial: " << tamFusionadaParcial << endl;
	
	return tamFusionadaParcial;
}


/*
* @ Fusiona la imagen de una sola cámara en una Imagen Mayor
* @ También comprime la fusionada para poder enviarla al servidor
* @ La fusionada de la cámara principal además añade a la misma la Intersección
*/
void camara::fussioParcial (unsigned char* imagen, bool grava)
{
	memset(imagen, 255, 2592*2592);

	cout << "Fusionando " << idCam << " Individualmente... " << endl;
	int contador=0;

	for(long long int cnt=0; cnt<tamFusionadaParcial; cnt=cnt+2 )
	{
		long int posIM  =   tablaFusionadaParcial[cnt];
		long int posImFus = tablaFusionadaParcial[cnt+1];

		imagen[posImFus] 	= quadrat[posIM];
		imageParcial[contador]  = quadrat[posIM];
		contador++;
	}


	if(grava) {

		char rutaIm1[100]="./Im1.bmp";
		Mat Im1(Size(2592, 1944), CV_8UC1, quadrat);
		imwrite( rutaIm1, Im1);

		char rutaIm[100]="./fusionadaParcial.bmp";
		Mat dig1(Size(2592, 2592), CV_8UC1, imagen);
		imwrite( rutaIm, dig1);
	}

}

/*
* @ Según lo anterior (la fusión) esta función añade la Intersección a la misma
* @ 
*
*/
void camara::enviarInterseccioReconstruida ( unsigned char* imageParcialEnInterseccio, int anticFi  )
{
	//Comprimir Interseccio per a enviar al Servidor
	int contador = (int)floor(anticFi/2.0);

	for(long int cnt=anticFi; cnt<tamFusionadaParcial; cnt=cnt+2 )
	{
		long int posFUS		= tablaFusionadaParcial[cnt+1]; 	// tablaCompuestaParcial[cnt];
		imageParcial[contador]  = imageParcialEnInterseccio[posFUS]; 	// quadrat[posIM];
		contador++;
	}

}

/*
 * @ Comprime la intersección Parcial
 * @ a partir de la tabla calculada
*/
void camara::fussioInterseccioParcial (unsigned char* imagen)
{
	//cout << "Tamaño INTERSECCION ---> " << tamInterseccionParcial << endl;

	for (int cont=0; cont < tamInterseccionParcial ; cont++ )
	{
		long int posFus = tablaInterseccionParcial   [cont];
		long int posIm  = tablaInterseccionParcial_C [cont];
		imageInterseccioParcial[cont] = quadrat[posIm]; 
	}

}


/*
*
*
*/
void camara::procesarInterseccionParcial(bool grava)
{
	if((FYmax-FYmin)>0) {
		memset(imagenPROVA, 255, sizeof(char)*2592*(FYmax-FYmin) );
		
		int diferencia = minimIdx % 2592;

		for (int cont=0; cont < tamInterseccionParcial ; cont++ )
		{
			long int posFus = tablaInterseccionParcial[cont];
			imagenPROVA[posFus-minimIdx+diferencia] = imageInterseccioParcial[cont];
		}
		
		//cout << "Mija de l'image: " << media/tamInterseccionParcial << endl;
		char rutaImGray[100]="";
		if(idCam==1) strcat(rutaImGray,"./InterseccionGray1.bmp" );
		if(idCam==2) strcat(rutaImGray,"./InterseccionGray2.bmp" );
		char rutaImGrayAux[100]="";
		if(idCam==1) strcat(rutaImGrayAux,"./InterseccionGrayAux1.bmp" );
		if(idCam==2) strcat(rutaImGrayAux,"./InterseccionGrayAux2.bmp" );

		// Reconstruida en Grisos
		Mat dig1(Size(2592, (FYmax-FYmin) ), CV_8UC1, imagenPROVA);
		Mat grisaux;
		dig1.copyTo(grisaux);
		dig1.copyTo(interseccioGrisos);


		if(grava) {
			// Dibuixa Circul del centro i llinia divisoria
			Point2i centroIm(1296, (int)round((FYmax-FYmin)/2.0) );
			Point2i pt1(0, (int)round((FYmax-FYmin)/2.0)), pt2(2592, (int)round((FYmax-FYmin)/2.0));
			int thickness=2, lineType=8, shift=0;
			line( grisaux, pt1, pt2, Scalar(255), thickness, lineType, shift );
			circle ( grisaux, centroIm, 10, Scalar(255), 4, 8, 0 );
			imwrite( rutaImGrayAux, grisaux);
		}

		// Binarisada
		for (int cont=0; cont < 2592*(FYmax-FYmin) ; cont++ ) {
			if(imagenPROVA[cont]<=34) {
				imagenPROVA[cont] = 0; 
			}
			else {
				imagenPROVA[cont] = 255;
			}
		}
		Mat processada(Size(2592, (FYmax-FYmin) ), CV_8UC1, imagenPROVA);

		//detectarObjecto(&dig1, &dig1, grava);


		char rutaIm[100]="";
		if(idCam==1) strcat(rutaIm,"./Interseccion1.bmp" );
		if(idCam==2) strcat(rutaIm,"./Interseccion2.bmp" );
		//binarisar ( processada, processada, grava);
		if(grava) {
			imwrite( rutaIm, processada);
		}
		thresh_callback(processada, processada, interseccioGrisos, grava);

		if(grava) {
			imwrite( rutaImGray, interseccioGrisos);
		}
		//imshow(nameWin, dig1);

		//usleep(1000000);
	}
}


void camara::binarisar ( Mat &image_binarisada, Mat &image_grey, bool grava)
{
	int blockSize=11;
	double C = 3.0;
	double maxValue=255.0; 
	adaptiveThreshold(image_grey, image_binarisada, maxValue, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, blockSize, C );
}


bool camara::detectarObjecto (Mat *imageOut, Mat *imageIn, bool grava)
{

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 30;   //30
	params.maxArea = 600;  //600

	// Filter by Circularity
	params.filterByCircularity = false;
	params.minCircularity = 0.001;

	// Filter by Convexity
	params.filterByConvexity = false;
	params.minConvexity = 0.001;

	// Filter by Inertia
	params.filterByInertia = false;
	params.minInertiaRatio = 0.001;


	// Storage for blobs
	vector<KeyPoint> keypoints;

	#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2
		// Set up detector with params
		SimpleBlobDetector detector(params);
		// Detect blobs
		detector.detect( *imageIn, keypoints);
	#else 
		// Set up detector with params
		Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);   
		// Detect blobs
		detector->detect( *imageIn, keypoints);
	#endif 


	if(!keypoints.empty())
	{
		objectes.clear();

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob
		Mat im_with_keypoints;
		drawKeypoints( *imageIn, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );


		if(idCam==1 && grava) imwrite( "./Objecte1.bmp", im_with_keypoints); 
		if(idCam==2 && grava) imwrite( "./Objecte2.bmp", im_with_keypoints);

		// Crear una lista con objetos (posibles CElegans) 
		cout << "Objetos: " << keypoints.size() << endl;
		for (int iO=0; iO<(int)keypoints.size(); iO++  ) {
			//cout << "-" << idCam <<" Objeto " << iO << ": " << keypoints[iO].pt << endl;
			objetos objecte(keypoints[iO].pt);
			objectes.push_back( objecte );
		}
		
		im_with_keypoints.release();
	}
}


void camara::thresh_callback( Mat &image_output , Mat &threshold_output, Mat &imageGrisos , bool grava )
{
	//int thresh = 100;
	//int max_thresh = 255;
	int eix = (int)round((float)(FYmax-FYmin)/2.0);
	RNG rng(12345);

	//Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using Threshold
	//threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
	/// Find contours
	findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect	( contours.size() );
	vector<Point2f>center	( contours.size() );
	vector<float>radius	( contours.size() );

	objectes.clear();
	objectes.reserve(10000);

	cout << "Blobs detectados: " << contours.size() << endl;
	for( int i = 0; i < contours.size(); i++ )
	{
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );

		// Almacena objetos candidatos a ser CElegans
		int xbox = boundRect[i].br().x - boundRect[i].tl().x;
		int ybox = boundRect[i].br().y - boundRect[i].tl().y;

		// Seleccionar els possibles CElegans
		if   (	contours[i].size()>5 && contours[i].size()<600 &&
			xbox>1 && xbox<600 && ybox>1 && ybox<600 )
		{
			objetos objecte( center[i], contours[i], boundRect[i], radius[i], FYmin );
			objecte.addPixels(imageGrisos); //, 30
			if( objecte.talla(eix) ) {
				objectes.push_back (objecte);
			}
		}
	}


	// Dibuixar Contorns
	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ ) {
		/// Draw polygonal contour
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
	}


	//cout << "Objetos detectados: " << objectes.size() << endl;
	if(objectes.size()>0) {
		///bonding rects + circles
		for( int i = 0; i< objectes.size(); i++ ) {
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

			rectangle	( drawing, objectes[i].boundRect().tl()	 , objectes[i].boundRect().br()	, color, 1, 8, 0 );
			circle		( drawing, objectes[i].centroide()	 , (int)objectes[i].radius()	, color, 1, 8, 0 );
			double fontScale = 1.0;
			char str[10]="";
			sprintf(str,"%i",i);
			putText		( drawing, str, objectes[i].centroide(), 1, FONT_HERSHEY_PLAIN, Scalar(0,255,255), 1, 8, false );
		}

		if(idCam==1 && grava) imwrite( "./ObjecteContourns1.bmp", drawing); 
		if(idCam==2 && grava) imwrite( "./ObjecteContourns2.bmp", drawing);
	}

	/// Show in a window
	//namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	//imshow( "Contours", drawing );
}


/*
*
* Crear la tabla de intersección en función de Texel, a nivel individualizado (por cámara)
* 
*/
void camara::crear_TablaInterseccion ( int idxTexel, int idxImage, int cntCam)
{
	tablaInterseccionIndividual [cntCam +0] = (long int)idxTexel;
	tablaInterseccionIndividual [cntCam +1] = (long int)idxImage;

	//cout << cntCam +1 << " · posIm: " << tablaInterseccionIndividual[cntCam +1] << endl;

	if ( cntCam >= tamInterseccio )
		tamInterseccio = cntCam+1;

	//cout << "Total: "<< cntCam << endl;
}


void camara::leer_TablaInterseccion ( )
{
	for(int cnt=0; cnt < tamInterseccio; cnt=cnt+1)
	{
		//tablaInterseccionIndividual[cntCam +0] = idxTexel;
		long int idxImage = tablaInterseccionIndividual[cnt +1] ;

		cout << cnt +1 << " · posIm: " << idxImage << endl;
	}

}

/*
*
* Guardar tabla de Intersecion
*
*/
void camara::save_TablaInterseccion()
{


	FILE *f = fopen( ruta_tablaInterseccion,"w");

	if (f == 0) 
		printf("*No he pogut obrir %s\n", ruta_tablaInterseccion );
	else
	{
		fwrite( tablaInterseccionIndividual, sizeof(int), tamInterseccio , f);
		fclose(f);
	}
	
}


/*
Cargar tabla de calibración
*/
bool camara::load_TablaInterseccion (void)
{
	
	FILE * pFile;
	long lSize;
		
	size_t result;

	if((pFile = fopen ( ruta_tablaInterseccion , "rb" ))!=NULL) //dir_rep
	{

		if (pFile==NULL) {fputs ("File error",stderr); }

		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		if (tabla_calibracion == NULL) {fputs ("Memory error!!!! ",stderr); }

		result = fread ( tablaInterseccionIndividual, sizeof(int), lSize, pFile);	
	}
	else
	{
		cout << "No hay archivo" << endl;
		return false;
	}
	free (pFile);

	tamInterseccio = lSize/sizeof(int);

	cout << "CARGANDO TABLA NUEVA: " << ruta_tablaInterseccion << " Tam: " << tamInterseccio << endl;

	return true;

}

/**************************************************************************************
 Convierte la Matriz de calibracion en una Tabla de Asignacion (Tabla de Calibracion)
***************************************************************************************/
/*
int camara::Calculo_Tabla_CalibracionDirecta ( void )
{
	int Ancho_Pant = (int)floor(ImagenDisplayTextura->GetWidth());
	int Alto_Pant  = ImagenDisplayTextura->GetHeight();

	Mat punt = Mat(Size(1, 3), CV_64FC1, Scalar(0,0,1));


	//Mat dHi;
	loadHomographyInverseParams();
	cout << "dHi = " << dHi << endl;
	dHiInv=dHi.inv();

	int posTEX = 0;
	
	for(int cnt_i=0; cnt_i<1944; cnt_i++ )
	{
		for(int cnt_j=0; cnt_j<1944; cnt_j++ )
		{
			punt.at<double>(0,0)=  cnt_i;
			punt.at<double>(0,1)=  cnt_j;
			punt.at<double>(0,2)=    1.0;

			Mat punt_T = dHiInv*punt;
			int y = (int)(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
			int x = (int)(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

			if(x>=0 && x<Ancho_Pant && y>=0 && y<Alto_Pant)
			{
				long int posIM = (y*floor(Ancho_Pant) + x);
				tabla_calibracionDirecta[posTEX] = posIM;
				//cout << x << ", " << y << " = " << posTEX << endl;
				posTEX++;	
			}		
		}
	}

	cout << posTEX << endl;

	return posTEX;
}

*/
/************************************************
 Captura imagen
************************************************/
bool camara::CE_Capturar ( bool unaCaptura )
{
	const void* frame_data; int frame_sz;

	// Si NO tiene Multiplexor coge solo una imagen
	if( !withMUX || unaCaptura)
	{
		//StopCamera();
		//cam = StartCamera( WIDTH_HD, HEIGHT_HD, 7,1,false );
		while( !cam->BeginReadFrame(0,frame_data,frame_sz) ) {};			
		cam->EndReadFrame(0);
		//cout << "Una captura" << endl;
		//usleep(10000);
	}
	// Si tiene Multiplexor toma más de una imagen
	else if( withMUX ) 
	{
		Seleccio_CAM();
		struct timespec t0 = Inicia_Temporisador();

		float TEMPS=0.0;
		//for(int k=0; k<4; k++)
		while (TEMPS<0.4) {
		//	StopCamera();
		//	cam = StartCamera( WIDTH_HD, HEIGHT_HD, 7,1,false );
			while(!cam->BeginReadFrame(0,frame_data,frame_sz)) {}		
			cam->EndReadFrame(0);
			TEMPS = Medix_temps(t0);
		}

		cout << "Varias capturas: " <<  TEMPS << " segundos" << endl;
	}


	if(WIDTH_HD==WIDTH_FRAME) {
		//cout << "Tam Image: " << frame_sz << endl;
		memcpy(quadrat,(unsigned char*)frame_data, frame_sz );
	}
	else {  
		QUADRAT (quadrat, (unsigned char*)frame_data);
	}

	if(MODO_ENFOQUE)
	{
		//Mat dig1( Size(WIDTH_FRAME, HEIGHT_FRAME), CV_8UC1, quadrat);
		//imshow(nameWin, dig1);
		//waitKey(15);
	}


	//guardarTest("./Im", quadrat);
	//guardarTest("./Im", quadrat, CV_8UC1, WIDTH_FRAME, HEIGHT_FRAME );
	
}


/**********************************************************************
Calculo de Error de Compensacion
***********************************************************************/
// Llama a la función calcular error compensación y crea un archivo
bool camara::CE_Error_Compensacion (void)
{
/*
	if(CNTA)
	{
		//float error = this->Calcular_Error_Compensacion();
		float error = this->Calcular_Luz_Media();
		cout << "Media " << idSV << ": " << error << endl;

		if(idSV==1) ficherEstudiBateriaMatlab1 << error << ", " << endl;
		if(idSV==2) ficherEstudiBateriaMatlab2 << error << ", " << endl;

		char buf[80];
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buf, sizeof(buf), "%X", timeinfo);
		//Hora_Actual(buf);

		if(idSV==1) ficherEstudiBateria1 << buf << ", " << error << endl;
		if(idSV==2) ficherEstudiBateria2 << buf << ", " << error << endl;
	}*/
}


// Calcula el error de compensación 
float camara::Calcular_Error_Compensacion ( void )
{
	long int error_total=0;
	int N= CNTA/2;

	for( int cnt=0 ; cnt < CNTA ; cnt=cnt+2 ) 
	{
		long int posIM  = tabla_calibracion[cnt];
		error_total = error_total + abs ( 48-quadrat[posIM] );
	}

	float Mija_error = (float)(  (double)error_total/(double)N );

	return Mija_error;
}



/************************************************
Calcula intensidad de luz meddia
************************************************/
float camara::Calcular_Luz_Media ( void )
{
	long int total=0;
	int N= CNTA/2;

	for( int cnt=0 ; cnt < CNTA ; cnt=cnt+2 ) 
	{
		long int posIM  = tabla_calibracion[cnt];
		total += quadrat[posIM];
	}

	float Mija = (float)(  (double)total/(double)N );

	return Mija;
}


/************************************************
Compensa la iluminación del Display
************************************************/
bool camara::CE_Compensar ( void )
{

	// UNA VOLTA CALCULADA LA TABLA COMPENSA D'UNA FORMA MES EFICIENT
	if(CNTA)
        	TransformarImagen_Y_CompensarDisplay();

	//guardarTest("./Text", ImagenDisplayVector, CV_8UC4, (int)floor(ImagenDisplayTextura->GetWidth()), ImagenDisplayTextura->GetHeight() );
	Sum_It_Comp();
}


/************************************************
Compensa Rápido la iluminación del Display
************************************************/
bool camara::CE_Compensar_Rapido ( void )
{

	bool unaCaptura = false;

    	CE_Capturar(unaCaptura);
	compensada=false;

	// UNA VOLTA CALCULADA LA TABLA COMPENSA D'UNA FORMA MES EFICIENT
	if(CNTA)
	{
		Seleccio_CAM();
		unaCaptura=true;
		usleep(3000);
		
        	while(!min_found)
        	{
			CE_Capturar( unaCaptura );
			TransformarImagen_Y_CompensarDisplay ( true );
			Sum_It_Comp();
			display->dibuixar();
		}
	}
	compensada=true;

    return min_found;
}

/*
// Transformar y Compensar Imagen
int camara::TransformarImagen_Y_CompensarDisplay ( bool automaticStop )
{
	long int error_total=0;
	double N = CNTA/2;
	int idxEnviarTextura=0;
	memset(texturaComprimida, '\0', sizeof(char)*900*480*2 );

	int MODA[256]={0};

	for(int cnt=0; cnt<CNTA; cnt=cnt+2 )
	{
		long int posIM  = tabla_calibracion[cnt];
		long int posTEX = tabla_calibracion[cnt+1];

		int roig = ImagenDisplayVector[posTEX];
		int vert = ImagenDisplayVector[posTEX+1];
		int blau = ImagenDisplayVector[posTEX+2];
		//int alfa = ImagenDisplayVector[posTEX+3];
				
		int error = (int)(floor(0.9*( 48-quadrat[posIM])));
		MODA[quadrat[posIM]]++;
		int col_global= roig + vert + blau + error;

		error_total = error_total + abs(48 - quadrat[posIM]);

		ImagenDisplayVector[posTEX]   = col_global;
		ImagenDisplayVector[posTEX+1] = col_global-255;
		ImagenDisplayVector[posTEX+2] = col_global-510;
		ImagenDisplayVector[posTEX+3] = 255;

		if (col_global-510<0)   ImagenDisplayVector[posTEX+2] =0;
		if (col_global-255<0)   ImagenDisplayVector[posTEX+1] =0;
		if (col_global<0)       ImagenDisplayVector[posTEX]   =0;
		if (col_global-510>255) ImagenDisplayVector[posTEX+2] =255;
		if (col_global-255>255) ImagenDisplayVector[posTEX+1] =255;
		if (col_global>255)     ImagenDisplayVector[posTEX]   =255;

		texturaComprimida[idxEnviarTextura+0] = ImagenDisplayVector[posTEX+0];
		texturaComprimida[idxEnviarTextura+1] = ImagenDisplayVector[posTEX+1];
		texturaComprimida[idxEnviarTextura+2] = ImagenDisplayVector[posTEX+2];

		idxEnviarTextura+=3;
	}
//	cout 	<< idCam << " Pixels de TABLA: " << CNTA/2 
//		<< ". Bytes de textura: " << idxEnviarTextura << endl;

	float Mija_error 	= (float)( (double)error_total/(double)N);										
	float Mija_error_ant	= mija_error_compensacio;

	Inc_It_Error();

	// MET = Mija Error Total
	float MET_Act = (MET_Ant*( Iteracions_Compensacio-1) + Mija_error) / Iteracions_Compensacio;
	float dMET = MET_Ant - MET_Act;
	float dM = Mija_error_ant - Mija_error;


	//if(Iteracions_Compensacio==1) Set_alpha( alpha );


	// Calcul del minim
	if( (Mija_error < minim_error_compensacio) && !min_found )
	{
		minim_error_compensacio = Mija_error;
        	memcpy( vector_mostrar_minim_error, vector_mostrar_ant, 480*431*4);
        	Init_It_Error();
//        	cout << "Encontrado." << endl;

	}
//	cout << "Media: " << Mija_error <<  ", minimo: " << minim_error_compensacio << endl;

	// Bloqueig
	if( !min_found && It_Erro>15) //abs(dMET)<0.1 && abs(dM)<0.1 &&
	{
		min_found=true;
		//codicImage=0;
		Init_It_Imagens();
		//pSVx->Init_It_Imagens();
	}

	// Reset
	if( reset_comp ) //if( abs(dM)>6 && min_found )
	{
		reset_comp=false; //Set_Reset_Comp( false );
		Init_Textura();
		//Init_Textura_Groc();
		Init_Erro_Minim();
		Init_Erro_Mija();
		Init_It_Error();
		Init_MET_Ant(); //Mija Error Total An
		Init_It_Comp();
		min_found = false;
	}

	//cout << "Mija Ant: " << Mija_error_ant << ", Mija Actual: " << Mija_error  << ", dM: " << dM << endl;
	//cout << "MET_Ant: "  << MET_Ant << ", MET_Act: "  << MET_Act << ", dMET: "  << dMET << endl;
	//cout << "Minim      : "  << minim_error_compensacio << ", Alpha: " << alpha << endl;

	MET_Ant = MET_Act;
	mija_error_compensacio = Mija_error;


	//mostrar_tex->SetPixels( vector_mostrar_ant );  ImagenDisplayTextura
	if ( !min_found ) mostrar_tex.SetPixels( vector_mostrar_ant );
	if (  min_found )
	{
		mostrar_tex.SetPixels( vector_mostrar_minim_error );
		cout << "Bloqueado" << endl;
	}

	return min_found;
}*/


// Transformar y Compensar Imagen
int camara::TransformarImagen_Y_CompensarDisplay ( bool automaticStop )
{
	long int error_total=0;
	double N = CNTA/2;
	int idxEnviarTextura=0;
	memset(texturaComprimida, '\0', sizeof(char)*900*480*2 );

	double MODA[256]={0};
	double F_RELATIVAS[256]={0};

	for(int cnt=0; cnt<CNTA; cnt=cnt+2 )
	{
		long int posIM  = tabla_calibracion[cnt];
		long int posTEX = tabla_calibracion[cnt+1];

		int roig = ImagenDisplayVector[posTEX];
		int vert = ImagenDisplayVector[posTEX+1];
		int blau = ImagenDisplayVector[posTEX+2];
		//int alfa = ImagenDisplayVector[posTEX+3];
				
		int error = (int)(floor(0.9*( 48-quadrat[posIM])));
		MODA[quadrat[posIM]]++;
		int col_global= roig + vert + blau + error;

		error_total = error_total + abs(48 - quadrat[posIM]);

		ImagenDisplayVector[posTEX]   = col_global;
		ImagenDisplayVector[posTEX+1] = col_global-255;
		ImagenDisplayVector[posTEX+2] = col_global-510;
		ImagenDisplayVector[posTEX+3] = 255;

		if (col_global-510<0)   ImagenDisplayVector[posTEX+2] =0;
		if (col_global-255<0)   ImagenDisplayVector[posTEX+1] =0;
		if (col_global<0)       ImagenDisplayVector[posTEX]   =0;
		if (col_global-510>255) ImagenDisplayVector[posTEX+2] =255;
		if (col_global-255>255) ImagenDisplayVector[posTEX+1] =255;
		if (col_global>255)     ImagenDisplayVector[posTEX]   =255;

		texturaComprimida[idxEnviarTextura+0] = ImagenDisplayVector[posTEX+0];
		texturaComprimida[idxEnviarTextura+1] = ImagenDisplayVector[posTEX+1];
		texturaComprimida[idxEnviarTextura+2] = ImagenDisplayVector[posTEX+2];

		idxEnviarTextura+=3;
	}

	float Mija_error 	= (float)( (double)error_total/(double)N);										
	float Mija_error_ant	= mija_error_compensacio;

	It_Erro++;

	// Calcular les freqüencies Relatives
	for(int i=0; i<256; i++ ) F_RELATIVAS[i] = MODA[i]/N*100.0;

	// Calcular l'error
	double errorHistograma[256]={0.0};
	for(int i=0; i<256; i++ ) errorHistograma[i] = H_Referencia[i] - F_RELATIVAS[i];

	// Error quadratic
	double errorQuadratic[256]={0.0};
	for(int i=0; i<256; i++ ) errorQuadratic[i] = errorHistograma[i]*errorHistograma[i];

	// Error quadratic mig
	double errorQuadraticMig=0;
	for(int i=0; i<256; i++ ) errorQuadraticMig = errorQuadraticMig + errorQuadratic[i];
	errorQuadraticMig = errorQuadraticMig/100.0;

	// Calcul de la derivada de la funcio de l'error quadratic mig
	int derv_error = (int)(errorQuadraticMig_Anterior - errorQuadraticMig);
	errorQuadraticMig_Anterior = errorQuadraticMig;


cout << It_Erro << " · derv_error: " << derv_error << endl;
//

//ficher_analisis << Iteracions_Compensacio << ", " << modaFondo;
//for(int i=0; i<256; i++) ficher_analisis << " " << F_RELATIVAS[i];
//ficher_analisis << "]" << endl;
//ficher_analisis << endl;

///-----------
	// Calcul del minim
	if( ( derv_error == 0) && !min_found )
	{
		minim_error_compensacio = Mija_error;
        	memcpy( vector_mostrar_minim_error, vector_mostrar_ant, 480*431*4);
        	//Init_It_Error();
	}
	else {
		It_Erro=0;
	}

	// Bloqueig
	if( !min_found && It_Erro>8) 
	{
		min_found=true;
		Init_It_Imagens();
	}

	// Reset
	if( reset_comp )
	{
		reset_comp=false;
		Init_Textura();
		//Init_Textura_Groc();
		Init_Erro_Minim();
		Init_Erro_Mija();
		Init_It_Error();
		Init_It_Comp();
		min_found = false;
	}


	if ( !min_found ) mostrar_tex.SetPixels( vector_mostrar_ant );
	if (  min_found )
	{
		mostrar_tex.SetPixels( vector_mostrar_minim_error );
		cout << "Bloqueado" << endl;
	}

	return min_found;

}

/**********************************************************************
Compensación con 4 cámaras
***********************************************************************/
int camara::TransformarImagen_Y_CompensarDisplay ( void )
{
	long int error_total=0;
	int idxEnviarTextura=0;

	for(int cnt=0; cnt<CNTA; cnt=cnt+2 )
	{
		long int posIM  = tabla_calibracion[cnt];
		long int posTEX = tabla_calibracion[cnt+1];

		int roig = ImagenDisplayVector[posTEX  ];
		int vert = ImagenDisplayVector[posTEX+1];
		int blau = ImagenDisplayVector[posTEX+2];
		//int alfa = ImagenDisplayVector[posTEX+3];
				
		int error = (int)(floor(0.9*( 48-quadrat[posIM])));

		int col_global= roig + vert + blau + error;

		error_total = error_total + abs( 48-quadrat[posIM]);

		ImagenDisplayVector[posTEX]   = col_global;
		ImagenDisplayVector[posTEX+1] = col_global-255;
		ImagenDisplayVector[posTEX+2] = col_global-510;
		ImagenDisplayVector[posTEX+3] = 255;

		if (col_global-510<0)   ImagenDisplayVector[posTEX+2] =0;
		if (col_global-255<0)   ImagenDisplayVector[posTEX+1] =0;
		if (col_global<0)       ImagenDisplayVector[posTEX]   =0;
		if (col_global-510>255) ImagenDisplayVector[posTEX+2] =255;
		if (col_global-255>255) ImagenDisplayVector[posTEX+1] =255;
		if (col_global>255)     ImagenDisplayVector[posTEX]   =255;

		texturaComprimida[idxEnviarTextura+0] = ImagenDisplayVector[posTEX+0];
		texturaComprimida[idxEnviarTextura+1] = ImagenDisplayVector[posTEX+1];
		texturaComprimida[idxEnviarTextura+2] = ImagenDisplayVector[posTEX+2];

		idxEnviarTextura+=3;
	}

    return false;
}


/*
*
* @     Comprimir interseccion:
* @ A partir de la tabla de intersección, se puede comprimir la zona de la imagen
* @ la cual se intersecta con la otra cámara 
* 
* @ Aquí se comprime dicha zona de imagen y se almacena en "imageInterseccioComprimida"  
* 
*/
void camara::compresionImagenInterseccion(void)
{
	int idxEnviarTextura=0;
	//cout << "Comprimiendo... " << tamInterseccio << endl;

	for(int cnt=0; cnt < tamInterseccio ; cnt=cnt+2 ) 
	{
		int posTEX  = (int)tablaInterseccionIndividual[cnt+0];
		int posIM   = (int)tablaInterseccionIndividual[cnt+1];
		imageInterseccioComprimida[idxEnviarTextura] = quadrat[posIM];

		idxEnviarTextura++;
	}

}



/*
Detectar circunferencia
*/
long int camara::detectarCircunferencia(Mat& nubePuntos, long int totalPuntos)
{
	vector<Vec3f> circles;
	Mat center = Mat(Size(1, 3), CV_64FC1, Scalar(0,0,1));

	Mat src_gray = Mat (Size(WIDTH_FRAME,HEIGHT_FRAME), CV_8UC1, quadrat );
//	HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 2, 1000, 60, 10 , 1700, 2000 );

	for( size_t i=0; i<circles.size(); i++ )
	{

		center.at<double>(0,0) = (double)circles[i][0];
		center.at<double>(1,0) = (double)circles[i][1];
		center.at<double>(2,0) = 1;
		
		double radio = circles[i][2];

		Mat centro = dHiInv*center;

		double x = (centro.at<double>(0,0)/centro.at<double>(0,2));
		double y = (centro.at<double>(0,1)/centro.at<double>(0,2));

		//Claculo radio
		center.at<double>(0,0) = (double)circles[i][0];
		center.at<double>(1,0) = (double)circles[i][1] + radio;
		center.at<double>(2,0) = 1;

		centro = dHiInv*center;

		double x1 = (centro.at<double>(0,0)/centro.at<double>(0,2));
		double y1 = (centro.at<double>(0,1)/centro.at<double>(0,2));

		double radioTex = sqrt( pow(x-x1,2) + pow(y-y1,2) );

		nubePuntos.at<int>(totalPuntos,0)=x;
		nubePuntos.at<int>(totalPuntos,1)=y;
		nubePuntos.at<int>(totalPuntos,2)=radioTex;

		cout << "X: " << x << ", Y: " << y << ", " << radioTex << endl;
		totalPuntos++;

	}

	return totalPuntos;

}

/************************************************************************
Colocar una imagen
*************************************************************************/
void camara::fusionarImagen (unsigned char* fusionada)
{
	cout << "Fusionando... " << idCam << endl;
	for(int cnt=0; cnt<CNTFusion; cnt=cnt+2 )
	{
		long int posIM  =   tabla_calibracionFusion[cnt];
		long int posImFus = tabla_calibracionFusion[cnt+1];

		fusionada[posImFus] = quadrat[posIM];
		//fusionada[posImFus] = 63*idCam;
	}

}


/* 
	Obtener la Ecuación de la Recta

*/
void camara::obtenerEcuacionDeLaRecta(Point2f puntoA , Point2f puntoB, Mat& recta, int idRecta )
{
	double m;

	if(puntoA.x-puntoB.x!=0)
		m =  (puntoA.y-puntoB.y)/(puntoA.x-puntoB.x);
	else m=99999;

	double b =  puntoA.y - m*puntoA.x;

	recta.at<double>(idRecta,0)=m;
	recta.at<double>(idRecta,1)=b;

	rectas = recta;

}



/* 
	Obtener la Ecuación de la Recta

*/
void camara::obtenerEcuacionDeLaRectaFusionada(Point2f puntoA , Point2f puntoB, Mat& recta, int idRecta )
{
	double m;

	if(puntoA.x-puntoB.x!=0)
		m =  (puntoA.y-puntoB.y)/(puntoA.x-puntoB.x);
	else m=99999;

	double b =  puntoA.y - m*puntoA.x;

	recta.at<double>(idRecta,0)=m;
	recta.at<double>(idRecta,1)=b;

	rectasFusionada = recta;

}



/* 
 * Matriu de transforacio P
*/
Point2f camara::Px(double tx, double ty)
{
	Point2f transformado;

	Mat punt = Mat(3, 1, CV_64FC1);

	punt.at<double>(0,0)= tx; //cnt_j; // Y
	punt.at<double>(1,0)= ty; //cnt_i; // X
	punt.at<double>(2,0)= 1.0;

	//Mat punt_T = dHi*punt;
	Mat punt_T = dHiPlanoSuperior*punt;

	int x = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
	int y = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

	transformado.x=x;
	transformado.y=y;

	
	return transformado;
}



/* 
 * Matriu de transforacio inversa P
*/
Point2f camara::PinvX( double imx, double imy)
{

	Point2f transformado;

	Mat punt = Mat(3, 1, CV_64FC1);

	punt.at<double>(0,0)= imx; //cnt_j; // Y
	punt.at<double>(1,0)= imy; //cnt_i; // X
	punt.at<double>(2,0)= 1.0;

	//Mat punt_T = dHiInv*punt;
	Mat punt_T = dHiPlanoSuperior.inv()*punt;  // <<<<<<<<--------------------

	int tx = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
	int ty = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));
 

	transformado.x=tx;
	transformado.y=ty;

	
	return transformado;
	
}



/**************************************************************************
Proves 
***************************************************************************/
void camara::prova()
{
	bool unaCaptura=false;
	CE_Capturar(unaCaptura);

	unsigned char* imagenTest   = (unsigned char*) malloc (sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME);
	memset ( imagenTest, 0, sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME );

	int const numRectas = rectas.rows;
	double limiteEnY1=0,limiteEnY2=0;
	cout << "rectas = " << rectas << endl;
	//cout << "numRectas=" << numRectas << endl;

	for(int x=0; x<WIDTH_FRAME; x++)
	{

		if(idCam==1) //idCam==0)//if(numRectas==1) 
		{
			limiteEnY1 = rectas.at<double>(0,0)*x + rectas.at<double>(0,1);
			for(int j=0; j<HEIGHT_FRAME; j++)
			{
				if(j<limiteEnY1 )
				 	int pd=1;//quadrat[x*1944 + j]=255;
				else quadrat[j*WIDTH_FRAME + x]=127;
			}
		}
		if(idCam==2) //if(numRectas==1) 
		{
			limiteEnY1 = rectas.at<double>(0,0)*x + rectas.at<double>(0,1);
			for(int j=0; j<HEIGHT_FRAME; j++)
			{
				if( j>limiteEnY1 )
					int pd=1;//quadrat[x*1944 + j]=255;
				else quadrat[j*WIDTH_FRAME + x]=127;
			}
		}
/*		if(numRectas==2)
		{
			limiteEnY1 = rectas.at<double>(0,0)*x + rectas.at<double>(0,1);
			limiteEnY2 = rectas.at<double>(1,0)*x + rectas.at<double>(1,1);
			for(int j=0; j<1944; j++)
			{
				if(idCam==1||idCam==3)
				{
					if(j>limiteEnY1 && j>limiteEnY2 )
						int pd=1; //quadrat[x*1944 + j]=255;
					else quadrat[j*1944 + x]=127;
				}
				if(idCam==2)
				{
					if(j>limiteEnY1 && j<limiteEnY2 )
						int pd=1; //quadrat[x*1944 + j]=255;
					else quadrat[j*1944 + x]=127;
				}
				if(idCam==4)
				{
					if(j<limiteEnY1 && j<limiteEnY2 )
						int pd=1; //quadrat[x*1944 + j]=255;
					else quadrat[j*1944 + x]=127;
				}
			}
		}
*/

	}

	guardarTest("./Test", quadrat);

}

/**************************************************************************
Guarda una image 
***************************************************************************/
void camara::guardarTest(const char* nombre, unsigned char* im, int colores, int Wi, int Hi)
{
	
	char rutaIm[100];
	strcpy(rutaIm, nombre);
	string identificador = int2string(idCam);
	strcat( rutaIm, identificador.c_str() );
	strcat(rutaIm, ".bmp");
	Mat dig(Size(Wi, Hi), colores, im);   //WIDTH_FRAME, HEIGHT_FRAME
	imwrite( rutaIm, dig);

}

/**************************************************************************
Guarda una image 
***************************************************************************/
void camara::guardarTest(const char* nombre, unsigned char* im)
{

	char rutaIm[100];
	strcpy(rutaIm, nombre);
	string identificador1 = int2string(idSV);
	string identificador2 = int2string(idCam);

	strcat( rutaIm, identificador1.c_str() );
	strcat( rutaIm, identificador2.c_str() );

	strcat(rutaIm, ".bmp");
	Mat dig(Size(WIDTH_FRAME, HEIGHT_FRAME), CV_8UC1, im);
	imwrite( rutaIm, dig);

}


/**************************************************************************
Guarda una image 
***************************************************************************/
void camara::guardarTest(const char* nombre)
{

	char rutaIm[100];
	strcpy(rutaIm, nombre);
	string identificador = int2string(idCam);
	strcat( rutaIm, identificador.c_str() );
	strcat( rutaIm, ".bmp");
	Mat dig(Size(WIDTH_FRAME, HEIGHT_FRAME), CV_8UC1, quadrat);
	imwrite( rutaIm, dig);

}


/*************************************************************************
Convierte el Indice de Texel a Indice de imagen
**************************************************************************/
int camara::getPixelImage (int idxTexel)
{
	int idxImage=0;

	int Ancho_Pant = (int)floor(ImagenDisplayTextura->GetWidth());

	int texelY = (int)floor( (double)idxTexel/(4.0*floor(Ancho_Pant)));
	int texelX = (int)floor(((double)idxTexel/4.0 - floor(Ancho_Pant)*(double)texelY));

	Mat punt = Mat(3, 1, CV_64FC1);
	punt.at<double>(0,0) = (double)texelX;
	punt.at<double>(1,0) = (double)texelY;
	punt.at<double>(2,0) = 1.0;

	Mat punt_T = dHi*punt;

	int y = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
	int x = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

	idxImage = x*(int)floor(WIDTH_FRAME) + y;

	return idxImage;
}


/**************************************************************************
Multiplica GxP 
***************************************************************************/
Point2f camara::GxPx(double Imx, double Imy )
{
	Point2f transformado;

	Mat punt = Mat(3, 1, CV_64FC1);

	punt.at<double>(0,0)= Imx;
	punt.at<double>(1,0)= Imy;
	punt.at<double>(2,0)= 1.0;

	Mat punt_T = MatGxP*punt;

	int Fx = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
	int Fy = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

	transformado.x=Fx;
	transformado.y=Fy;

	
	return transformado;

}



/**************************************************************************
Multiplica Inv_GxPx 
***************************************************************************/
Point2f camara::Inv_GxPx(double FusX, double FusY )
{
	Point2f transformado;

	Mat punt = Mat(3, 1, CV_64FC1);

	punt.at<double>(0,0)= FusX;
	punt.at<double>(1,0)= FusY;
	punt.at<double>(2,0)= 1.0;

	Mat punt_T = MatGxPInv*punt;   //  <<<<<<<<< ----------------------


	int ImX = (int)floor(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
	int ImY = (int)floor(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

	transformado.x=ImX;
	transformado.y=ImY;

	
	return transformado;

}


/**************************************************************************
Introduce GxP 
***************************************************************************/
void camara::setMatGxP(Mat& G)
{
	//MatGxP = G*dHiInv;
	MatGxP = G*dHiPlanoSuperior.inv();  //   <<<<<<<<<<<<<<<<<------------------------------
	MatGxPInv = MatGxP.inv(); 
	cout << "Calculando GxP..." << idCam << endl;
}


/************************************************************************
Calculo tabla de Fusion
*************************************************************************/
int camara::calculo_Tabla_GXP ( void )
{

	int AnchoOrigen = WIDTH_FRAME;
	int AltoOrigen  = HEIGHT_FRAME;

	int AnchoFusion = WIDTH_FRAME*2;
	int AltoFusion  = HEIGHT_FRAME*2;

	//Mat punt = Mat(Size(1, 3), CV_64FC1, Scalar(0,0,1));
	cout << "Creando tabla... " << idCam << endl;
	

	//int const numRectas = rectas.rows;
	int const numRectas = rectasFusionada.rows;


	double limiteEnY1=0,limiteEnY2=0;
	int cnt = 0;
	long int posIM, posFus;
	Point2i puntFusionada;

	double m1, m2, b1, b2;
	if(numRectas>0)
	{
		m1 = rectas.at<double>(0,0);
		b1 = rectas.at<double>(0,1);

		if(numRectas==2)
		{
			m2 = rectas.at<double>(1,0);
			b2 = rectas.at<double>(1,1);
		}
	}

/*	if(numRectas>0)
	{
		m1 = rectasFusionada.at<double>(0,0);
		b1 = rectasFusionada.at<double>(0,1);

		if(numRectas==2)
		{
			m2 = rectasFusionada.at<double>(1,0);
			b2 = rectasFusionada.at<double>(1,1);
		}
	}
*/

	for(int ImX=0; ImX<AnchoOrigen; ImX++ )
	{
		for(int ImY=0; ImY<AltoOrigen; ImY++ )
		{
			puntFusionada = GxPx((double)ImX,(double)ImY);

			if(puntFusionada.x>=0 && puntFusionada.x<1944*2 && puntFusionada.y>=0 && puntFusionada.y<1944*2)
			{

				if(0) //numRectas==0)
				{
					posFus = (puntFusionada.y*(1944*2) + puntFusionada.x);
					if(evaluado[posFus]==0 )
					{
						posIM  = ImY*1944 + ImX;

						tabla_calibracionFusion[cnt]   = posIM;
						tabla_calibracionFusion[cnt+1] = posFus;
						evaluado[posFus]=255;
						cnt=cnt+2;
					}
				} 
				
				if(0) //numRectas==1)
				{
					//limiteEnY1 = m1*ImX + b1;
					limiteEnY1 = m1*puntFusionada.x + b1;

					posFus = (puntFusionada.y*(1944*2) + puntFusionada.x);
					if(!evaluado[posFus] && puntFusionada.y>limiteEnY1) //ImY>limiteEnY1 )
					{
						posIM  = ImY*1944 + ImX; 

						tabla_calibracionFusion[cnt]   = posIM;
						tabla_calibracionFusion[cnt+1] = posFus;
						evaluado[posFus]=true;
						cnt=cnt+2;
					}
				}		
			
				if(0) //numRectas==2)
				{
					limiteEnY1 = m1 * ImX + b1;
					limiteEnY2 = m2 * ImX + b2;

					//limiteEnY1 = m1 * puntFusionada.x + b1;
					//limiteEnY2 = m2 * puntFusionada.x + b2;

					posFus = (puntFusionada.y*(1944*2) + puntFusionada.x);

					//if(!evaluado[posFus] && puntFusionada.y>limiteEnY1 && puntFusionada.y>limiteEnY2 ) //ImY>limiteEnY1 && ImY>limiteEnY2 )
					if( ImY>limiteEnY1 && ImY>limiteEnY2 )
					{
						posIM  = ImY*1944 + ImX; 

						tabla_calibracionFusion[cnt]   =  posIM;
						tabla_calibracionFusion[cnt+1] =  puntFusionada.y*(1944*2)  + puntFusionada.x;
						evaluado[posFus]=true;
						cnt=cnt+2;
					}
				}

				if(1)
				{
					limiteEnY1 = m1 * ImX + b1;
					limiteEnY2 = m2 * ImX + b2;


					posFus = (puntFusionada.y*(1944*2) + puntFusionada.x);


					if(idCam==1||idCam==3)
					{
						if(ImY>limiteEnY1 && ImY>limiteEnY2 )
						{
							posIM  = ImY*1944 + ImX; 

							tabla_calibracionFusion[cnt]   =  posIM;
							tabla_calibracionFusion[cnt+1] =  puntFusionada.y*(1944*2)  + puntFusionada.x;
							evaluado[posFus]=true;
							cnt=cnt+2;
						}
							
					}
					if(idCam==2)
					{
						if(ImY>limiteEnY1 && ImY<limiteEnY2 )
						{
							posIM  = ImY*1944 + ImX; 

							tabla_calibracionFusion[cnt]   =  posIM;
							tabla_calibracionFusion[cnt+1] =  puntFusionada.y*(1944*2)  + puntFusionada.x;
							evaluado[posFus]=true;
							cnt=cnt+2;
						}
							
					}
					if(idCam==4)
					{
						if(ImY<limiteEnY1 && ImY<limiteEnY2 )
						{
							posIM  = ImY*1944 + ImX; 

							tabla_calibracionFusion[cnt]   =  posIM;
							tabla_calibracionFusion[cnt+1] =  puntFusionada.y*(1944*2)  + puntFusionada.x;
							evaluado[posFus]=true;
							cnt=cnt+2;
						}
							
					}


				}

			}
		
		}
	}

	CNTFusion = cnt;
	cout << cnt << endl;
}

/**********************************************************************
Segmentar
**********************************************************************/
void camara::segmentar(int umbralColor)
{

	memset(segmentada, 255, sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME );
	for( int cnt=0; cnt<WIDTH_FRAME * HEIGHT_FRAME; cnt++ )
		if(quadrat[cnt]>umbralColor)
			segmentada[cnt]=0;
	
}


/**********************************************************************
Detectar bordes
**********************************************************************/
long int camara::cannyUmbral( Mat& nubePuntos, long int totalPuntos )
{
	bool show = false;

	int Ancho_Pant = (int)floor(ImagenDisplayTextura->GetWidth());
	int Alto_Pant  = ImagenDisplayTextura->GetHeight();

	Mat src, dst, detected_edges;

	int edgeThresh = 2;
	int lowThreshold=0;
	int const max_lowThreshold = 20; //100
	int ratio = 3;
	int kernel_size = 3;

	//int anchoPant = (int)floor(WIDTH_PANT);

	segmentar(10);

	Mat src_gray = Mat (Size(WIDTH_FRAME,HEIGHT_FRAME), CV_8UC1, segmentada );
	Mat punt = Mat(Size(1, 3), CV_64FC1, Scalar(0,0,1));

	//if (!view.empty())

	// Reduce noise with a kernel 3x3
	blur( src_gray, detected_edges, Size(3,3) );

	// Canny detector
	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );


	float xmax=0, ymax=0, xmin=1000, ymin=1000;
	float mediaX=0, mediaY=0;

	for (int j=0; j<1944 ; j++)
		for (int i=0; i<1944 ; i++)
			if((int)detected_edges.at<uchar>(i,j)==255)
			{	
				punt.at<double>(0,0)=  (double)j; //j; //-i
				punt.at<double>(0,1)=  (double)i; //i; //j
				punt.at<double>(0,2)=  1.0;

				Mat punt_T = dHiInv*punt;

				//int y = (int)(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
				//int x = (int)(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));
				int x = (int)(punt_T.at<double>(0,0)/punt_T.at<double>(0,2));
				int y = (int)(punt_T.at<double>(0,1)/punt_T.at<double>(0,2));

				//cout << x << ", " << y << endl;

				if(x>=0 && x<Ancho_Pant && y>=0 && y<Alto_Pant)
				{
					nubePuntos.at<int>(totalPuntos,0)=x;
					nubePuntos.at<int>(totalPuntos,1)=y;

					long int posTEX = (y*floor(Ancho_Pant) + x);

					ImagenDisplayVector[posTEX*4]  = 255;
					ImagenDisplayVector[posTEX*4+1]=   0;
					ImagenDisplayVector[posTEX*4+2]=   0;
					ImagenDisplayVector[posTEX*4+3]= 255;
					
					totalPuntos++;
				}
			}
	return totalPuntos;

}



/*
*
* Conexión con el Servidor MASTER RASPBERRY 
* Se crea un hilo para conectar con el MASTER 
*
*/
void* clienteRASPBERRY (void* CAMARA)
{
	camara *CAM = (camara*) CAMARA;
	
	struct hostent *host;
        struct sockaddr_in server_addr;

	int sock;
	int sin_size;
	host = gethostbyname(IP_Master);          

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(3000);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);   
	bzero(&(server_addr.sin_zero),8);

	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("Socket");

	printf("Buscando Display: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
	do
	{	
		usleep(50000);
	}while(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1); // || !con_conexion
	
	printf("Conectado a Display: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));

	
	while(1)
	{
		CAM->comunicacionRaspberries( sock );
		CAM->esperaEnviar();
	}
	close(sock);
}



void camara::esperaEnviar(void)
{
	do{
		usleep(10000);
	} while(!enviaInfoAlMaster);

	enviaInfoAlMaster=false;
}


bool camara::comunicacionRaspberries (int sock)
{

	enviarCabecera(sock);


	if(ordenes1 == enviaTabla) {
		enviarTablaCalibracion(sock);
	}
	if(ordenes1 == enviaTextura) {
		//cout << "Enviando... " << endl;
		enviarTextura(sock);
	}

	if(ordenes2 == enviaTablaInterseccion) {
		enviarTablaInterseccion(sock);
	}

	if(ordenes2 == enviaInterseccion) {
		enviarInterseccio(sock);
	}

	//usleep(300000);

}


/*
 * Crea Cabecera
 *	// id		:  X
 *	// Estat	:  C·T·C
 *	// Tam Textura	:  XXX·XXX
 *	// Tam Intersc	:  XXX·XXX
 * Envia Cabecera
*/
bool camara::enviarCabecera (int sock)
{
	int pos = 0;
	// Inicialisa vector
	char cabecera[19]="";
	memset(cabecera, '\0', sizeof(cabecera));

	// Llena contenido //
	// id
	cabecera[pos] = (char)itoa(idCam)[0]; //idCamara;
	pos++;
	cabecera[pos] = (char)itoa(idSV)[0]; //id Sistema Visio;
	pos++;

	cabecera[pos] = (char)itoa(0)[0]; //esMaster
	pos++;

	// Estats de cada camara, Calibracio
	cabecera[pos] = (char)itoa(estadoCalibracion)[0]; // Estat Calibracio

	pos++;
	cabecera[pos] = (char)itoa(1)[0]; // Estat Taules
	pos++;
	cabecera[pos] = (char)itoa(1)[0]; // Estat
	pos++;


	// Tamany Textura
	string tamanoTabla = NOM_000X ( CNTA );
	for(int it=0; it< tamanoTabla.size(); it++) {
		cabecera[pos] = tamanoTabla[it];
		pos++;
	}

	// Tamany Inerterseccio
	string tamanoInterseccio = NOM_000X ( tamInterseccio ); //<--------------------
	for(int it=0; it< tamanoInterseccio.size(); it++) {
		cabecera[pos] = tamanoInterseccio[it];
		pos++;
	}

	//cout << "Cabecera: " << cabecera << endl;
	// Envía Cabecera
	if( send(sock, &cabecera, 19, 0 ) == -1 )
		cout << "Fallo enviar textura" << endl;


	// Inicialisa vector
	char acciones[9]="";
	memset(acciones, '\0', sizeof(acciones));
	// Recibe Acciones
	if(recv(sock,&acciones,9,0)==-1)
		cout << "Error recibiendo" << endl;

	ordenes1 = (int)acciones[0];
	ordenes2 = (int)acciones[1];
	//cout << "Accion1: " << (int)acciones[0] << " Accion2: " << (int)acciones[1] << endl;

}


void camara::solicitarDibuixarPatro (void)
{
	// enum {espera_3, quieroCalibrar, yaHeCalibrado};
	estadoCalibracion = quieroCalibrar;
	enviaAlMaster();
}




/*
* Textura
* Envía la Textura ya compensada
* La textura depende solo de su cámara correspondiente
* así que esta puede ser calculada localmente en esta Raspberry
* 
* Habrán dos buffers con la textura:
*    · Uno local (el calculado por el SLAVE) Este es virtual, no se visualiza
*    · Otro Remoto (el enviado al MASTER) Este es el que se visualiza
*/
bool camara::enviarTablaCalibracion(int sock)
{

	// Enviar Tabla
	//char* tablaAenviar = (char*) malloc ( sizeof(int) * CNTA );
	memset( tablaAenviar, '\0' , sizeof(int)* 900*480*2);		
	memcpy( tablaAenviar, tabla_calibracion, sizeof(int) * CNTA );

	int tam = sizeof(int) * CNTA;
	int enviados=0;

	enviados = send(sock, (const char*)tablaAenviar, tam, 0);

	if( enviados ==-1 )
		cout << "Error al enviar textura" << endl;

	//usleep(1000000);
	//cout << "Tabla Enviada " << enviados << " de " << tam << endl;
		
	//free(tablaAenviar);
	return true;


}


bool camara::enviarTextura(int sock)
{
	int tam = (int)(CNTA*3/2);

	int enviados=0;
	enviados = send(sock, (const char*)texturaComprimida, tam, 0);

	if( enviados ==-1 )
		cout << "Error al enviar textura" << endl;

	//cout << "En: " << enviados << " de " << tam << endl;
	//cout << "Enviar textura: " << idCam << endl;
}


/*
*
* @ Intersección Comprimida Para Textura
* @ El envío se lleva al DISPLAY
* @ Envia la imagen de interseccion de la camara
* @ para que la Raspberry Master calcule la compensación
* @ Para la parte intersectada se precisan las imágenes de las
* @ cámaras que intersecten, así que ésta no puede calcularse localmente
*
*/
bool camara::enviarTablaInterseccion(int sock)
{
	// Enviar Tabla
	//char* tablaAenviar = (char*) malloc ( sizeof(int) * tamInterseccio );
	memset( tablaAenviar, '\0' , sizeof(int)* 900*480*2);	
	memcpy(tablaAenviar, tablaInterseccionIndividual, sizeof(int) * tamInterseccio);

	int tam = sizeof(int) * tamInterseccio;

	int enviados = send(sock, (const char*)tablaAenviar, tam, 0);
	if( enviados ==-1 )
		cout << "Error al enviar textura" << endl;

	//usleep(1000000);
	//cout << "Envia Enviada:   " << enviados << endl;
	
	//free(tablaAenviar);
	return true;
}


bool camara::enviarInterseccio(int sock)
{
	int tam = (int)(tamInterseccio/2);

	int enviados = send(sock, (const char*) imageInterseccioComprimida , tam, 0);

	if( enviados ==-1 )
		cout << "Error al enviar imagen" << endl;

	//cout << "Enviar Intersección: " << idCam << endl;

}




/*
* 
* @ Comunicar les RPI d'un mateix sistema de visio
* @ Crear comunicacio TCP/IP en la Raspberry Pi que tinga el Sistema de Visio Master 
* 
*
*
* 
*/
void* clienteSistemaVision (void* CAMARA)
{
	camara *CAM = (camara*) CAMARA;
	
	struct hostent *host;
        struct sockaddr_in server_addr;

	int sock;
	int sin_size;
	host = gethostbyname(IP_SV_Ppal); // <---- Sistema Vision Principal

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(2000);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);   
	bzero(&(server_addr.sin_zero),8);

	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("Socket");

	printf("Buscando Sistema Vision Principal: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
	do
	{	
		usleep(50000);
	}while(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1); // || !con_conexion
	
	printf("-> Conectado a Sistema Vision Principal: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));

	
	while(1)
	{
		// ENVÍO MATRIZ de CALIBRACIÓN
		// Si la conexio es en una camara remota (l'ha d'enviar)
		if( CAM->GetId() == idCam_Ppal )
		{
			// Envia la tabla cuando se haya calculado, una vez
			if( CAM->isCalibrada() && !CAM->isMatrizCalibracionEnviada() && !CAM->isMatrizCalibracionRecibida() ) //!CAM->get_dHi().empty() ) 
			{
				int m=3, n=3;
				CAM->cabeceraSincronizacion(sock, 1);

				cout << endl <<"Enviando Matriz" << CAM->GetId() << "... " << endl << *CAM->get_dHi() << endl;
				CAM->enviarMatrizCalibracion( sock, CAM->get_dHi(), m, n );
			}

		}

		// RECIBO MATRIZ de CALIBRACIÓN
		// Si la conexio es en la camara local (l'ha de rebre)
		if( CAM->GetId() != idCam_Ppal ) 
		{
			// Recibe la tabla cuando se haya calculado, una vez <<<<< -----
			if( CAM->preparadaComunicar() && !CAM->isMatrizCalibracionEnviada() && !CAM->isMatrizCalibracionRecibida() ) //!CAM->get_dHi().empty() ) 
			{
				int m=3, n=3;
				CAM->cabeceraSincronizacion(sock, 1);

				CAM->recibirMatrizCalibracion (sock, CAM->get_dHi(), m, n);
				cout << endl <<"Recibiendo Matriz... " << endl << *CAM->get_dHi() << endl;
			}
		}


		// ENVÍO TABLA de INTERSECCIÓN
		// Si el Sistema de Visión es Secundario (Este hilo solo lo ejecuta el secundario)
		// enviará la tabla de compresion de la imagen de INTERSECCIÓN de la cámara local
		if( CAM->GetId() == idCam_Ppal ) 
		{
			if( CAM->isTablaInterseccionParcial_Calculada() && !CAM->isTablaInterseccionParcial_Enviada() )  // && !tablaInterseccionParcial_Recibida
			{
				CAM->cabeceraSincronizacion(sock, 2);
				CAM->enviarTablaInterseccionParcial(sock);
			}
		}

		// ENVÍO INTERSECCIÓN
		// Cuando se haya enviado la tabla de INTERSECCIÓN, se enviará la INTERSECCIÓN 
		if( CAM->isTablaInterseccionParcial_Enviada() )
		{
			// Espera la orden para enviar la INTERSECCION al SV Principal
			CAM->esperarEnviarPrincipal();
			CAM->cabeceraSincronizacion(sock, 3);
			CAM->enviarInterseccionParcial(sock);
		}


		// Si no se ha llegado al paso de INTERSECCION se hace espera cada bucle
		if( !CAM->isTablaInterseccionParcial_Enviada() ) 
		{
			usleep(500000);
		}
		
	}
	close(sock);
}


void camara::cabeceraSincronizacion(int sock, int tipoDato)
{
	int tam = 15;
	char cabecera[tam]="";
	int enviados=0;
	int pos=0;

	cabecera[pos]=(int)idCam;
	pos++;
	cabecera[pos]=(int)idSV;
	pos++;
	cabecera[pos]=(int)tipoDato;
	pos++;

	// Tamany Interseccio
	string tamanoTabla = NOM_000X ( tamInterseccionParcial );
	for(int it=0; it< tamanoTabla.size(); it++) {
		cabecera[pos] = tamanoTabla[it];
		pos++;
	}

	enviados = send(sock, cabecera, tam, 0);
	if( enviados ==-1 )
		cout << "Error al enviar matriz" << endl;

}


/*
* @ Enviar y recibir la Matriz de Calibración
*
*/
void camara::recibirMatrizCalibracion (int sock, Mat* matriu, int m, int n)
{
	int rebut;
	int tam_a_rebre = 23*m*n;

	memset( matriuRebuda, '\0', sizeof(char) * 300 );

	int rep=0;
	do{
		rebut = recv(sock, &matriuRebuda[rep], tam_a_rebre-rep, 0);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		//return false;	
	}

	array2matriu( matriu, matriuRebuda, m, n );
	//cout << dHi << endl;

	// Cuando recibe la Matriz, esta calibra y no la vuelve a recibir
	matrizCalibracionRecibida(true);
}


void camara::enviarMatrizCalibracion (int sock, Mat *matriu, int m, int n)
{

	char matriuEnviar[300]="";
	Mat matriz = *matriu;
	mat2array ( matriuEnviar, matriz, m, n);
	//cout << matriuEnviar << endl;
	int tam = 23*m*n;

	int enviados=0;
	enviados = send(sock, matriuEnviar, tam, 0);

	if( enviados ==-1 )
		cout << "Error al enviar matriz" << endl;

	//cout << "Enviados: " << enviados << " de " << tam << endl;

	// Cuando envia la Matriz, no la vuelve a enviar
	matrizCalibracionEnviada(true);
}


/*
* @ Intersección Parcial Para Fusión de Imágenes
*
* @ Envía la Intersección Completa de cada imagen (PARCIAL)
* @ El Sistema de Visión Secundario envía la intersección
* @ El Sistema de Visión Principal recibe la intersección
* 
*/ 
void camara::enviarTablaInterseccionParcial(int sock)
{
	// Enviar Tabla
	memset( tablasXenviar , '\0' , sizeof(long int)*  500000);
	memcpy( tablasXenviar , tablaInterseccionParcial, sizeof(long int) * tamInterseccionParcial);

	int enviados = send(sock, (const char*) tablasXenviar, tamInterseccionParcial*sizeof(long int), 0);
	if( enviados ==-1 )
		cout << "Error al enviar textura" << endl;

	cout << "Tamaño TABLA -------------> " << tamInterseccionParcial << endl;


	tablaInterseccionParcial_Enviada=true;
	
}


void camara::enviarInterseccionParcial(int sock)
{

	int enviados = send(sock, (const char*) imageInterseccioParcial , tamInterseccionParcial, 0);

	if( enviados ==-1 )
		cout << "Error al enviar imagen" << endl;
	cout << "Tamaño INTERSECCION enviada ---> " << tamInterseccionParcial << endl;

}


/*
* @ Recibir (Si es Principal)
*/  
void camara::recibirTablaInterseccionParcial(int sock)
{

	int tam_a_rebre = tamInterseccionParcial*sizeof(long int); 	
	memset( tablasXenviar    , '\0', sizeof(long int) * 500000 );

	// Tabla Calibración
	int rebut;
	int rep=0;
	do{
		rebut = read(sock, &tablasXenviar[rep], tam_a_rebre-rep);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		//return -1;	
	}

	memcpy(tablaInterseccionParcial, tablasXenviar, tam_a_rebre);
	cout << "-> Tabla Intersección Parcial recibida " << rep << " de " << tam_a_rebre << endl;

	// Cálculo del rango de la ventana (FYmin FYmax)
	minimIdx = 10000000;
	maximIdx = 0;
	for( int idx=0; idx < tamInterseccionParcial; idx++) {
		long int posFus = tablaInterseccionParcial[idx];
		if( posFus < minimIdx ) {
			minimIdx = posFus;
		}
		if( posFus > maximIdx ) {
			maximIdx = posFus;
		}
	}
 
	FYmin = (int)floor(minimIdx/2592.0);
	FYmax = (int)floor(maximIdx/2592.0);

	//cout << "·" << idxMin << "-> minim: " << minimIdx << " | ·" << idxMax << "-> maxim: " << maximIdx << endl;
	cout << "FYmin: " << FYmin << ", FYmax: " << FYmax << ", minimIdx: " << minimIdx << ", maximIdx: " << maximIdx <<  endl;
	SV->fusionarTablasDeFusion();
	tablaInterseccionParcial_Recibida=true;
}


void camara::recibirInterseccionParcial(int sock)
{

	//memset( imageInterseccioParcial, '\0', sizeof(char) * 900*480 );
	int rebut;
	int tam_a_rebre = tamInterseccionParcial; 

	int rep=0;
	do{
		rebut = recv(sock, &imageInterseccioParcial[rep], tam_a_rebre-rep, 0);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		//return -1;	
	}
	//cout << "Tamaño INTERSECCION recibida ---> " << tamInterseccionParcial << endl;

}

/*
* @
*/
void camara::esperarEnviarPrincipal (void)
{
	do{
		usleep(10000);
	} while(!enviaInfoAlPrincipal);

	enviaInfoAlPrincipal=false;
}


/*************************************************************************************************************
	Calcular los errores de proyección
**************************************************************************************************************/
double camara::computeReprojectionErrorsH(const vector<Point2f>& imagePoints,
	const vector<Point2f>& displayPoints, const Mat& dHiActual, float* corregir_x, float* corregir_y)
{
	double totalErr = 0, err;
	double dx[44], dy[44];
	double dxmax=0, dymax=0, dxmin=10000, dymin=10000;
	
	int tamany = (int)imagePoints.size();
	vector<Point2f> displayPointsEst(tamany);

	//perspectiveTransform(to be transformed, output, dHiActual);
	perspectiveTransform(imagePoints, displayPointsEst, dHiActual);

	char ruta[100]="./analisis/errores_data";
	string ID = int2string(idCam);
	strcat( ruta, ID.c_str() );
	strcat( ruta,".xml");
	

	FileStorage fs(ruta, FileStorage::WRITE);
	for (int kk=0; kk < tamany; kk++) {
		dx[kk] = displayPointsEst[kk].x - displayPoints[kk].x;
		dy[kk] = displayPointsEst[kk].y - displayPoints[kk].y;
		//err = sqrt((dx*dx)+(dy*dy));
		fs << "errorx" << dx[kk];
		fs << "errory" << dy[kk];
		*corregir_x += (float)dx[kk];
		*corregir_y += (float)dy[kk];
	}

	for(int cnt=0; cnt<21; cnt++) if( dx[cnt]>dxmax ) dxmax = dx[cnt];
	for(int cnt=0; cnt<21; cnt++) if( dy[cnt]>dymax ) dymax = dy[cnt];

	for(int cnt=0; cnt<21; cnt++) if( dx[cnt]<dxmin ) dxmin = dx[cnt];
	for(int cnt=0; cnt<21; cnt++) if( dy[cnt]<dymin ) dymin = dy[cnt];

	*corregir_x = *corregir_x/28;
	fs << "corregit_x" << *corregir_x;

	*corregir_y = *corregir_y/28;
	fs << "corregit_y" << *corregir_y;

	fs << "dxmax" << dxmax;
	fs << "dymax" << dymax;
	fs << "dxmin" << dxmin;
	fs << "dymin" << dymin;

	fs.release();

	err = norm(displayPoints, displayPointsEst, NORM_L2);
	totalErr = err*err;
	return std::sqrt(totalErr / tamany);
}


/********************************************************************************
Guarda la Homografia Inversa en un archivo
*********************************************************************************/
void camara::saveInverseHomographyParams(vector<Point2f> displayPoints, vector<Point2f> imagePoints, Mat dHi, double displayPointsErr)
{
	char rutaGravar[150]="";
	if(!MODO_CALIBRACION) strcpy ( rutaGravar, ruta_homography );
	if( MODO_CALIBRACION) strcpy ( rutaGravar, ruta_homographyPlanoPlaca );

	cout << "Guardando datos de calibracion en... "<< rutaGravar << endl;
	FileStorage fs(rutaGravar, FileStorage::WRITE);
 
       if (!displayPoints.empty())
       {
             int tamany = (int)displayPoints.size();
             Mat displayPtMat(2, tamany, CV_32FC1);
             for (size_t i = 0; i < tamany; i++)
             {
                    displayPtMat.at<float>(0, i) = displayPoints[i].x;
                    displayPtMat.at<float>(1, i) = displayPoints[i].y;
             }
             fs << "displayPoints" << displayPtMat;
       }
 
       if (!imagePoints.empty())
       {
             int tamany = (int)imagePoints.size();
             Mat imagePtMat(2, tamany, CV_32FC1);
             for (size_t i = 0; i < tamany; i++)
             {
                    imagePtMat.at<float>(0, i) = imagePoints[i].x;
                    imagePtMat.at<float>(1, i) = imagePoints[i].y;
             }
             fs << "image_points" << imagePtMat;
       }
 
       fs << "dHi" << dHi;
       fs << "displayPoints_reprojection_error" << displayPointsErr;
 
}





/********************************************************************************
Cargar tabla de calibración
*********************************************************************************/
bool camara::loadTablaCalibracion (void)
{

	FILE * pFile;
	long lSize;
		
	size_t result;

	if((pFile = fopen ( ruta_tablaCalibracion , "rb" ))!=NULL) //dir_rep
	{
		cout << "Cargando Tabla Calibracion de Display..." << ruta_tablaCalibracion << endl;

		if (pFile==NULL) {fputs ("File error",stderr); }

		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		if (tabla_calibracion == NULL) {fputs ("Memory error!!!! ",stderr); }

		result = fread (tabla_calibracion, sizeof(int), lSize, pFile);
		//if (result != lSize) {fputs ("Reading error!!!! ",stderr); }
	
	}
	else
	{
		cout << "No hay archivo" << endl;
		return false;
	}
	free (pFile);

	CNTA = lSize/sizeof(int);
	cout << "Tabla Calibración Cantidad: " << CNTA << "Bytes" << endl;
	return true;

}


/********************************************************************************
Guardar tabla de calibración
*********************************************************************************/
bool camara::saveTablaCalibracion (void)
{
	cout << "Guardando Tabla Calibracion..." << ruta_tablaCalibracion << endl;

	FILE *f = fopen(ruta_tablaCalibracion,"w");

	if (f == 0) 
		printf("*No he pogut obrir %s\n", ruta_tablaCalibracion);
	else
	{
		fwrite( tabla_calibracion, sizeof(int), CNTA, f);   //fwrite(image_reb,1,tam_reb,f);
		fclose(f);
	}

	
}

/********************************************************************************
Guarda la Homografia Inversa en un archivo
*********************************************************************************/
void camara::savePatron (Mat Mpatron)
{

	//cout << "Guardando datos de patrón en... "<< ruta_patron << endl;

	FileStorage fs(ruta_patron, FileStorage::WRITE);
	fs << "Mpatron" << Mpatron;

 
}


/********************************************************************************
Carga la Homografia Inversa
*********************************************************************************/
void camara::loadHomographyInverseParams(void)
{

	if(!existe(ruta_homography))
		cout << "No existe archivo: " << ruta_homography << endl;
	else{
		//cout << "Cargando datos de calibracion... " << ruta_homography << endl;
		FileStorage fs(ruta_homography, FileStorage::READ); 
		fs["dHi"] >> dHi;
		fs["displayPoints_reprojection_error"] >> erroCalibracio;
	}

}


/********************************************************************************
Carga la Homografia Inversa
*********************************************************************************/
void camara::loadHomographyNivelPlaca(void)
{

	if(!existe(ruta_homographyPlanoPlaca))
		cout << "No existe archivo: " << ruta_homographyPlanoPlaca << endl;

	else {
		//cout << "Cargando datos de calibracion... " << ruta_homography << endl;
		FileStorage fs(ruta_homographyPlanoPlaca, FileStorage::READ); 
		fs["dHi"] >> dHiPlanoSuperior;
		//fs["displayPoints_reprojection_error"] >> erroCalibracio;
	}
}


/********************************************************************************
Guarda el patrón
*********************************************************************************/
void camara::savePatron (void)
{
	//cout << "Guardando datos de patrón en... "<< ruta_patron << endl;

	FileStorage fs(ruta_patron, FileStorage::WRITE);
	fs << "OFFSET_X" << OFFSET_X;
	fs << "OFFSET_Y" << OFFSET_Y;
	fs << "escala"   << escala;
}


/********************************************************************************
Carga el patrón
*********************************************************************************/
void camara::loadPatron(void)
{
	if(!existe(ruta_patron))
		cout << "No existe archivo: " << ruta_patron << endl;

	else {
		//cout << "Cargando datos de Patron... " << ruta_patron << endl;
		FileStorage fs(ruta_patron, FileStorage::READ);
		fs["OFFSET_X"] >> OFFSET_X;
		fs["OFFSET_Y"] >> OFFSET_Y;
		fs["escala"]   >> escala;
	}

}



void camara::enviarImage (void)
{
	//cliente->cabecera( TAM, temperatura, SV->GetId(), SV->Get_It_Imagens() );
	//cliente->ENVIAR_IMAGE( (char*)circul, TAM );

	cliente->cabecera(WIDTH_FRAME*1944, 0.0, idSV, 0);  //2966132
	cliente->ENVIAR_IMAGE( (char*)quadrat, WIDTH_FRAME*1944 );

}


/************************************************
 Conexión con el Servidor
************************************************/
void* Conexion_Servidor (void* CAMARA )
{
	unsigned char* circul   = (unsigned char*) malloc (sizeof(char) *2592*2592);
	camara *CAM = (camara*) CAMARA;
	sistema_visio* SV = CAM->get_SV();

	// Configurar TCP/IP
	struct hostent *host;
        struct sockaddr_in server_addr;

	int sock;
	int sin_size;
	host = gethostbyname(IP_Servidor);          
 
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons( SV->get_Port_IP() );
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);   
	bzero(&(server_addr.sin_zero),8);

	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("Socket");

	printf("Buscando servidor: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));

	do
	{	
		usleep(50000);
	}while(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1); // || !con_conexion
	
	printf("Conectado a: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));


	while(1)
	{	
		//pthread_mutex_lock(&mutexEnviament);
		CAM->mensaje(circul, sock);
		//pthread_mutex_unlock(&mutexEnviament);
	}
	free(circul);
	close(sock);
}


void camara::mensaje ( unsigned char* circul, int sock )
{
	int estat = SV->Get_Estat_Enviament();
	if(estat==FES_HO)
	{
		bool estado_conexion=false;
		struct timespec t0 = Inicia_Temporisador();
		SV->Set_Estat_Enviament(OCUPAT);

		//cout << "Cantitat_Camares: " << Cantitat_Camares << endl;
		
		if(!tablaParcial_Enviada && Cantitat_Camares>1) {
			enviarTablaImagenParcial(sock);
		}

		if( Cantitat_Camares == 1 ) {
			int TAM = CIRCUL (circul, (unsigned char*)(SV->Get_Vect_Im()));
			this->Configuracio_Remota( sock, TAM, temperatura, SV->GetId(), SV->Get_It_Imagens() );  //2966132
			estado_conexion = ENVIAR_IMAGE ( (char*)circul, sock, TAM );
		}

		if( Cantitat_Camares > 1 ) {
			this->Configuracio_Remota(sock, 2592*2592, temperatura, SV->GetId(), SV->Get_It_Imagens());  //2966132
			estado_conexion = enviarImageParcial(sock);
		}
		cout << "Temps Enviament d'Image: "<< Medix_temps(t0) << endl; //

		SV->Set_Estat_Enviament(ACABAT);
		SV->Set_Resultat_Enviament(estado_conexion);
		
	}
	else {
		usleep(50000);
	}
	
}


/*
* @
* @ Enviar la Taula de Compressio i l'Image Parcial al Servidor
* @
* @ 
* @ 
* @ 
*/
bool camara::enviarTablaImagenParcial(int sock)
{
	// Tamany Taula
	int arrrrrr = 1000;
	char cabecera[7]="";
	string tamanoTabla = NOM_000X ( tamFusionadaParcial ); //tamFusionadaParcial
	for(int it=0; it< tamanoTabla.size(); it++) {
		cabecera[it] = tamanoTabla[it];
	}

	// Envía Capçalera
	if( send(sock, &cabecera, 7, 0 ) == -1 )
		cout << "Fallo enviar textura" << endl;


	// Enviar Tabla
	memset( tablaAenviarImageParcial, '\0' , sizeof(int)* 3000*3000);		
	memcpy( tablaAenviarImageParcial, tablaFusionadaParcial, sizeof(int) * tamFusionadaParcial );

	long int tam = sizeof(int) * tamFusionadaParcial;
	int enviados=0;

	enviados = send(sock, (const char*)tablaAenviarImageParcial, tam, 0);
	cout << "TABLA a ENVIAR AL SERVIDOR: " << enviados << " de " << tamFusionadaParcial << endl;

	if( enviados ==-1 )
		cout << "Error al enviar textura" << endl;

	tablaParcial_Enviada = true;

	return true;
}


/*
* @
* @
* @
*/
bool camara::enviarImageParcial(int sock)
{
	int tam = (int)floor(tamFusionadaParcial/2.0);
	cout << "Enviar image Parcial: " << tam << endl;
	int enviados=0;
	enviados = send(sock, (const char*)imageParcial, tam, 0);

	if( enviados ==-1 )
		cout << "Error al enviar textura" << endl;
	return true;
}


/************************************************************************************************************
Configuracio a partir de la conexio remota
*************************************************************************************************************/
int camara::Configuracio_Remota( int sock, int tamanyo, float temperatura, int Id, int NUM_IMAGE )
{
	int bytes_recieved , si = 1, i, ENVIAT, repetir =0,j=0, BYTES=1;

	int ni = tamanyo;
	char FI[5]="0", CONFIRMACIO[100]="";
	int WImage, HImage;       
           
	char INFO[70] = "";

	if (config==1) {
		WImage = HEIGHT_FRAME;	HImage = HEIGHT_FRAME;
	}
	if (config==4) {
		WImage = WIDTH_FRAME;	HImage = WIDTH_FRAME;
	}

	string tempt   = float2string(temperatura);
	string ample1  = int2string(WImage);	//int2string(WIDTH_FRAME);
	string alt1    = int2string(HImage);	//int2string(HEIGHT_FRAME);
	string BY1     = int2string(BYTES);

	string NOM1   = NOM_000X (NUM_IMAGE);
	string I      = NOM_000X (ni);

	string Id_Camara = "RASPxx_ ";
	Id_Camara[4] = (char)idSV;
	Id_Camara[5] = (char)idCam;

	cout << "----> idSV: " << idSV << " -> idCam: " << idCam << endl;

	string INFO6 = Id_Camara + NOM1 + " " + ample1 + " " + alt1 + " " + BY1
		     + " " + "0972" + " " + "1296" + " " + "0972" + " " + I 
             + " " + int2string((int)fin_captura) + " " + int2string((int)capturando) + " " + "0" + " " + tempt  +'\0';  // Falta compensando 

	int kt = INFO6.size(); 
	for(int k = 0; k < kt ; k++) INFO[k]=INFO6[k];
	INFO[kt]  = '\0';
	INFO6[kt] = '\0';
	cout << INFO << endl;

	// Enviar INFO del DISPOSITIU i MENSAGE
	if(send(sock,&INFO,71,0)==-1) goto FINISH; 
	fin_captura = false;

	// Rebre Configuracio
	if(recv(sock,&CONFIRMACIO,99,0)==-1) goto FINISH;
//	if(!bloquejat)
	{
		SV->set_dia		( (int ) CONFIRMACIO[0] );	
		SV->set_condicion	( (char) CONFIRMACIO[1] );	
		SV->set_probeta		( (int ) CONFIRMACIO[2] );
		SV->set_NUM_CAMS	( (int ) CONFIRMACIO[3] );
		SV->set_numMuestras	( (int ) CONFIRMACIO[5] );

		// CONFIRMACIO[6] MODO =  8 bits: 
		inici_remot      = (bool)((CONFIRMACIO[6] & 128)>>7);
		G_Local          = (bool)((CONFIRMACIO[6] &  64)>>6);
//		enviar           = (bool)((CONFIRMACIO[6] &  32)>>5);
		apagar_RPI       = (bool)((CONFIRMACIO[6] &  16)>>4);
		bool iAbortar    = (bool)((CONFIRMACIO[6] &   8)>>3);
		bool compensa	 = (bool)((CONFIRMACIO[6] &   4)>>2);

		if(iAbortar)	abortar	   = true;
		if(compensa)	recompensa = true;

		char S_umbral[7];
		for(int correr=0; correr<6; correr++ ) S_umbral[correr] = CONFIRMACIO[correr+9];
		umbral = atof(S_umbral);

		char VS[7];		
		for(int correr=0; correr<7; correr++ ) VS[correr] = CONFIRMACIO[correr+15];

		for(int correr=0; CONFIRMACIO[correr+22]!='\0'; correr++)
		{
//			nombre[correr] = CONFIRMACIO[correr+22];
		}


	}

	inicialisarCaptura();
	sincronisacio();

	return 1;

	FINISH:
        	return -1; 

}


/************************************************
Inicialisar Captura
************************************************/
void camara::inicialisarCaptura (void)
{
	if( inici_remot && !iniciat )
	{
		iniciat=true;
		inici_remot=false;

//		codicImage=0;
		capturando=true;
		cout << "Inicio captura" << endl;

		/*for (int iSV=0; iSV < num_camaras; iSV++){
			CAM[iSV].Set_Reset_Comp(true);
			CAM[iSV].found_min (false);
		}*/

	}
}


/*********************************************************
Sincronisacio
**********************************************************/
void camara::sincronisacio(void)
{	/*
	bool compensadas=true;
	for (int iD=0; iD < num_camaras; iD++) {
		compensadas = compensadas && CAM[iD].getEstatCompensada();
	}
	compensada = compensadas; */

	int codicImage  = SV->Get_It_Imagens();
	int numMuestras = SV->get_numMuestras();

	if( capturando && !compensando )
	{
		cout << "Capturando" << endl;
//		codicImage++;
//		cout << codicImage << endl;
	}

	if( capturando && codicImage>(numMuestras) || abortar)
	{
		finCapturaCamara();
//		codicImage=0;
	}

}


void camara::finCapturaCamara (void)
{
	fin_captura = true;
	capturando  = false;
	iniciat     = false;
}




