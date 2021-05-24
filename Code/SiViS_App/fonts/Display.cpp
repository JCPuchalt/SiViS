#include "../llibreries/llibreries.hpp"
#include "../llibreries/graphics.h"
#include "../llibreries/Display.hpp"




/**********************************************************************************************************************
Clase Display
**********************************************************************************************************************/
Display::Display(float in_ancho, float in_alto, bool esMaster)
{
	ancho  = in_ancho;
	alto   = in_alto ;
	Master = esMaster;
	num_conexions=0; token=0;
	totsCalibrats=false;
	minimFound=false;
	temps_k=0;
	refresca=false;
	pA=0.0;
	maximaProvabilitat=0.0;

	cout << "DISPLAY CREAT: " << this << endl;

	SUBdisplay = reinterpret_cast<subDisplay *>(new char[16 * sizeof(subDisplay)]);

	ImagenDisplayVector   = (unsigned char*) malloc (sizeof(char) * (int)floor(ancho) *(int)floor(alto) *4);
	InterseccioOptimisada = (unsigned char*) malloc (sizeof(char) * 400 *200 *3);

	//memcpy(InterseccioOptimisada, 0, sizeof(char) * 400 *200 *3);

	ImagenDisplayTextura.CreateRGBA ( ancho, alto );
	ImagenDisplayTextura.GenerateFrameBuffer();
	NUM_DISPLAYS++;
	cout << "NUM_DISPLAYS -> " << NUM_DISPLAYS << endl; 

}

void Display::conectar (void)
{
	if(Master)
	{
		pthread_create(&conexion_id, NULL, servidor, this );
		//pthread_create(&refresh_id , NULL, refreshDisplay, this );
	}
}

void* refreshDisplay(void* context)
{
	Display* display = (Display*)context;

	while(1)
	{
		display->refrescarDisplay();
		//usleep(50000);
	}
}

void Display::refrescarDisplay(void)
{
	while(!refresca) {
		usleep(100000);
	}
	refresca=false;
	dibuixar();	
}

void Display::colorDisplay (int R, int G, int B, int A)
{
	for(int cnt=0; cnt< (floor(ancho)*floor(alto)*4) ; cnt=cnt+4)  //
	{
		ImagenDisplayVector[cnt+0]= R;
		ImagenDisplayVector[cnt+1]= G;
		ImagenDisplayVector[cnt+2]= B;
		ImagenDisplayVector[cnt+3]= A;
	}
}


void Display::dibuixar(void)
{
	ImagenDisplayTextura.SetPixels(ImagenDisplayVector);

	BeginFrame();	
	DrawTextureRect ( &ImagenDisplayTextura,        -1.0f, -1.0f, 1.0f, 1.0f, NULL);
	EndFrame(); 
}


void Display::impr(void)
{
	std::cout << &ImagenDisplayVector << std::endl;
}


void Display::posarPunt( Point2f punt )
{

	int anchoPant = (int)floor(ImagenDisplayTextura.GetWidth());
	for( int cnti=((int)floor(punt.x)-5); cnti<((int)floor(punt.x)+5); cnti++ )
		for( int cntj=((int)floor(punt.y)-5); cntj<((int)floor(punt.y)+5); cntj++ )
		{
			long int posTEX  = cntj*anchoPant + cnti;

			ImagenDisplayVector[posTEX*4]  = 255;
			ImagenDisplayVector[posTEX*4+1]= 255;
			ImagenDisplayVector[posTEX*4+2]= 255;
			ImagenDisplayVector[posTEX*4+3]= 255;
		}


}

void Display::crearLlinia( double m, double b )
{

	int anchoPant = (int)floor(ImagenDisplayTextura.GetWidth());
	int altoPant  = (int)floor(ImagenDisplayTextura.GetHeight());

	for (int x=0; x<anchoPant; x++)
	{
		int y = (int)floor(m*(double)x+b);
		if(y<altoPant)
		{
			for( int cnti=(x-2); cnti<(x+2); cnti++ )
			{
				for( int cntj=(y-2); cntj<(y+2); cntj++ )
				{
			
					long int posTEX  = cntj*anchoPant + cnti;

					ImagenDisplayVector[posTEX*4]  =   0;
					ImagenDisplayVector[posTEX*4+1]= 255;
					ImagenDisplayVector[posTEX*4+2]= 255;
					ImagenDisplayVector[posTEX*4+3]= 255;
				}
			}
		}
	}

}


void Display::posarQuadrat (int OrigeEnx, int OrigeEny, int AmpleQuadrat, int AltQuadrat )
{

	int anchoPant = (int)floor(ImagenDisplayTextura.GetWidth());

	for( int cnti=OrigeEnx; cnti<(OrigeEnx+AmpleQuadrat); cnti++ )
		for( int cntj=OrigeEny; cntj<(OrigeEny+AltQuadrat); cntj++ )
		{
			long int posTEX  = cntj*anchoPant + cnti;

			ImagenDisplayVector[posTEX*4]  = 255;
			ImagenDisplayVector[posTEX*4+1]= 100;
			ImagenDisplayVector[posTEX*4+2]=   0;
			ImagenDisplayVector[posTEX*4+3]= 255;
		}
}


void Display::posarCircul (int centro_x, int centro_y, int radi )
{

	int rad1 = (radi)*(radi);
	int anchoPant = (int)floor(ImagenDisplayTextura.GetWidth());

	for( int cnti=centro_x-radi; cnti<(centro_x+radi); cnti++ )
	{
		for( int cntj=centro_y-radi; cntj<(centro_y+radi); cntj++ )
		{
			int norma = pow((cnti-centro_x),2) + pow((cntj-centro_y),2);

			if(norma<=rad1)
			{			
				long int posTEX  = cntj*anchoPant + cnti;

				ImagenDisplayVector[posTEX*4]  = 255;
				ImagenDisplayVector[posTEX*4+1]= 255;
				ImagenDisplayVector[posTEX*4+2]= 255;
				ImagenDisplayVector[posTEX*4+3]= 255;
			}
		}
	}

}


void Display::borrarQuadrat (int OrigeEnx, int OrigeEny, int AmpleQuadrat, int AltQuadrat )
{

	int anchoPant = (int)floor(ImagenDisplayTextura.GetWidth());

	for( int cnti=OrigeEnx; cnti<(OrigeEnx+AmpleQuadrat); cnti++ )
		for( int cntj=OrigeEny; cntj<(OrigeEny+AltQuadrat); cntj++ )
		{
			long int posTEX  = cntj*anchoPant + cnti;

			ImagenDisplayVector[posTEX*4]  =   0;
			ImagenDisplayVector[posTEX*4+1]=   0;
			ImagenDisplayVector[posTEX*4+2]=   0;
			ImagenDisplayVector[posTEX*4+3]= 255;
		}
}


void Display::dibuixarPatroCalibracio (float OFFSET_X, float OFFSET_Y, float escala)
{
	float pantalla_x, pantalla_y;

	int fonfo=0;
	//ILLUMINACIO (display->pImagenDisplayTextura(), 0,0,0, NULL);
	for(int ite=0; ite<90; ite++) //42
	{
		if(ite%2) //UNO SÍ UNO NO
		{
			pantalla_x = puntos_pantalla[ite][0]*escala +  OFFSET_X;
			pantalla_y = puntos_pantalla[ite][1]*escala +  OFFSET_Y;

			int radi = 10;
			//cout << "X: " << pantalla_x << ", Y: " << pantalla_y << " R: " << radi << endl;
			posarCircul ( (int)round(pantalla_x), (int)round(pantalla_y), (int)radi );
		}
	}	

}


/*
*
* Hilo de Servidor de Master
* Si esta RASPBERRY es el Master crea un hilo servidor a la espera de conexiones
* La conexión es para recibir la textura del Display
* La Textura se dibujará en el display
* 
*/
void* servidor(void* context)
{

	Display* display = (Display*)context;

	//char IPMaster[20]="192.168.1.2";
	int host_port= 3000;
	void* dato =0;

	struct sockaddr_in my_addr;

	int hsock;
	int * p_int ;
	int err;

	socklen_t addr_size = 0;
	int* csock;
	sockaddr_in sadr;
	pthread_t thread_id=0;

	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n", errno);
	}
		    
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1; //
	
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ))
	{
		printf("Error setting options %d\n", errno);
		free(p_int);
	}
	free(p_int);

	my_addr.sin_port = htons(3000);
	my_addr.sin_addr.s_addr = inet_addr(IP_Master);
	my_addr.sin_family = AF_INET;
	bzero(&(my_addr.sin_zero),8);
	
	     
	if( bind( hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		//fprintf(stderr, "Error con el servidor RPI, PORT %d \n",errno);
		printf("Error con el servidor Display: (%s , %d) \n", inet_ntoa(my_addr.sin_addr),ntohs(my_addr.sin_port));
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",errno);
	}

	addr_size = sizeof(sockaddr_in);

	//struct Contexto contexto[4];
	int iter=0;
    
	while(true)
	{

		csock = (int*)malloc(sizeof(int));
		if(true) //num_conexions!=2)
		{
			if((*csock = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1)
			{
				printf("\033[1;32m-> Conexión establecida con Display %s \033[0m\n", inet_ntoa(sadr.sin_addr));
				pthread_create(&thread_id,0, SocketHandler, (display->crearSubDisplay(iter, csock)) );
				pthread_detach(thread_id);
				iter++;
			}
		}
		else
		{
			fprintf(stderr, "Error accepting %d\n", errno);
		}

	}


}

subDisplay* Display::crearSubDisplay ( int iD, int *sock )
{
	new(&SUBdisplay[iD]) subDisplay(this, sock, num_conexions);
	
	num_conexions++;
	cout << "num_conexions = " << num_conexions << endl << endl;

	return &SUBdisplay[iD];
}


/*
* @ Hilo de conexión
* @ Para cada RASPBERRY habrá una conexión
* @ Se gestionarán las tareas relacionadas entre cada cámara y el display
* @ Calibración, envío de tablas, envío de textura e intersecciones comprimidas
* 
*/
void* SocketHandler(void* context)
{
	subDisplay  *SubDisplay = (subDisplay*)  context;
	int 	    *csock1     =  SubDisplay -> getSock();
	Display     *display    =  SubDisplay -> getDisplay();


	bool patronCargado=false;
	int error=0;
	while(1)
	{
		// Cabecera
		SubDisplay->recibirCabecera(csock1);

		if( SubDisplay->getEstadoCalibracion() == quieroCalibrar &&    // Cuando alguien quiera calibrar
		    SubDisplay->getOrdeCalibracio() == display->getToken() )   // Cuando le toque
		{ 
			if (!patronCargado) {
				SubDisplay->crear_ruta_patron(); 
				patronCargado = SubDisplay->loadPatron();
			}
			display->colorDisplay(0,0,0,255);
			SubDisplay->dibuixarPatroCalibracio();
			//display->dibuixar();
			//cout << "Dibuixa " << endl;
		}

		if(SubDisplay->getOrdenes1() == recibeTabla) {
			SubDisplay->recibirTablaCalibracion (csock1);
			usleep(1000000);
		}

		if(SubDisplay->getOrdenes1() == recibeTextura) {
			//cout << "Recibiendo textura..." << SubDisplay->getIdCam() << endl;
			SubDisplay->recibirTextura (csock1);
			if( display->getTotsCalibrats() ) {
				SubDisplay->dibujarTextura();
				//display->refrescar();
				//display->dibuixar();
			}			
		}


		if(SubDisplay->getOrdenes2() == recibeTablaInterseccion) {
			SubDisplay->recibirTablaInterseccion (csock1);
			display->setTemps_k(0.0);
			usleep(1000000);
		}

		if(SubDisplay->getOrdenes2() == recibeInterseccion) {
			//cout << "Recibiendo intersección..." << SubDisplay->getIdCam() << endl;
			SubDisplay->recibirInterseccio (csock1);
			if( display->getTotsCalibrats() ) {
				display->dibuixarInterseccio( SubDisplay->getIdSV() );
				//display->dibuixar();
			}
		}

		// Si todos han calibrado
		if ( display->getNumConexions() <= display->getToken() ) {
			display->setTotsCalibrats(true);
		}
		else {
			display->setTotsCalibrats(false);
		}

		//usleep(10000);

	}

	close(*csock1);
	free(csock1);
}


/*
*
* @ Compensa y dibuja la textura de las dos imágenes
* 
*/
bool Display::dibuixarInterseccio (int idSV)
{
	//int idSV=2;
	int idC1=1; int idC2=2;

	int idx1 = buscar( idC1, idSV );
	int idx2 = buscar( idC2, idSV );

	unsigned char* imagen1 = SUBdisplay[idx1].get_imagenInterseccion();
	unsigned char* imagen2 = SUBdisplay[idx2].get_imagenInterseccion();

	int* tablaInterseccion1 = SUBdisplay[idx1].get_tablaInterseccion();
	int* tablaInterseccion2 = SUBdisplay[idx2].get_tablaInterseccion();

	int tam = SUBdisplay[idx1].get_TamTablaInterseccion();

	int idxIm=0;
	double media1=0.0, media2=0.0;
	double erroMig=0.0;

	for(int cnt=0; cnt < tam; cnt=cnt+2 )
	{
		long int posTEX   = tablaInterseccion1[cnt];
		//long int posIM1   = tablaInterseccion1[cnt+1];
		//long int posIM2   = tablaInterseccion2[cnt+1];
		
		float p1 = (float)imagen1[idxIm];
		float p2 = (float)imagen2[idxIm];

		media1 += (double)p1;
		media2 += (double)p2;
		//if(p1<48.0 && p1>=38.0) p1=p1*0.9;
		//if(p2<48.0 && p2>=38.0) p2=p2*0.9;
		
		//diferencial = (p1-p2);
		//if(p1<38.0) p1=48.0;
		//if(p2<38.0) p2=48.0;

		int error = (int)floor(0.9*( 48.0-( (p1+p2)/2.0) ));
		erroMig += error;

//		if(!minimFound) {
			int roig = ImagenDisplayVector[posTEX];
			int vert = ImagenDisplayVector[posTEX+1];
			int blau = ImagenDisplayVector[posTEX+2];
			//int alfa = ImagenDisplayVector[posTEX+3];

			int col_global= roig + vert + blau + error;

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

			mascara(posTEX);
//		}

		idxIm++;
	}
/*
	erroMig = erroMig/(double)idxIm;
	media1  = media1/(double)idxIm;
	media2  = media2/(double)idxIm;

	// provabilitat Error
	float mhu   = 0.0; 	// La mija
	float sigma = 7.0; 	// Desviacio tipica
	float pE    = gaussian_pdf (erroMig, mhu, sigma);
	cout << "erroMig: " << erroMig << ", provabilitat: " << pE << endl;

	// pA provabilitat Acumulada entre a i b
	// for(int x=a; x<b; x++) {}
	float mhu2   = 15.0; 	// La mija
	float sigma2 =  5.0;  	// Desviacio tipica	
	//float pT = gaussian_acumulada (0, mhu2, sigma2) - gaussian_acumulada (temps_k, mhu2, sigma2);
	//float pT = gaussian_pdf ( temps_k, mhu2, sigma2);
	//cout << "temps_k: " << temps_k << ", provabilitat Temporal: " << pA << endl;

	// provabilitat Mija 1
	float mhu3   = 48.0; 	// La mija
	float sigma3 =  5.0; 	// Desviacio tipica
	float pM1    = gaussian_pdf (media1, mhu3, sigma3);
	cout << "media1: " << media1 << ", provabilitat: " << pM1 << endl;

	// provabilitat Mija 1
	float mhu4   = 48.0; 	// La mija
	float sigma4 =  5.0; 	// Desviacio tipica
	float pM2    = gaussian_pdf (media2, mhu4, sigma4);
	cout << "media2: " << media2 << ", provabilitat: " << pM2 << endl;

	float pTotal = (pE + pM1 + pM2)/3.0;

	// La textura mes provable
	if ( maximaProvabilitat < pTotal ) {
		maximaProvabilitat = pTotal;
		copiarInterseccioOptimisada(idx1);
	} 

	// Si ha trobat alta provabilitat, ha trobat un optim
	if(maximaProvabilitat>0.142 && temps_k>30 && !minimFound) {
		minimFound=true;
		maximaProvabilitat = 0.0;
		pegarInterseccioOptimisada(idx1);
	}

	// Si no troba un optim, reseteja la busqueda
	if(!minimFound && temps_k>30) {
		temps_k=0.0;
		resetInterseccio( idx1, 0,0,0);
	}

	// Si hi ha un canvi alt en les condicions se reseteja
	if (pTotal<0.05 && minimFound) {
		minimFound=false;
		temps_k=0.0;
		resetInterseccio( idx1, 0,0,0);
		SUBdisplay[idx1].resetTextura(0,0,0);
		SUBdisplay[idx2].resetTextura(0,0,0);
		cout << "RESET" << endl;
	}
	if(minimFound) {
		cout << "Bloqueado <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl; 
	}
	cout << "maximaProvabilitat: " << maximaProvabilitat << ", provabilitat Total: " << pTotal << endl;
	//cout << "----->> Compensación CALCULADA--> media1: " << media1 << ", media2: " << media2 << endl;
	temps_k++; */
}


void Display::copiarInterseccioOptimisada (int idx)
{
	int* tablaInterseccion = SUBdisplay[idx].get_tablaInterseccion();
	int tam = SUBdisplay[idx].get_TamTablaInterseccion();

	int idxInter=0;
	for(int cnt=0; cnt < tam; cnt=cnt+2 )
	{
		long int posTEX   = tablaInterseccion[cnt];

		InterseccioOptimisada[idxInter+0] = ImagenDisplayVector[posTEX+0];
		InterseccioOptimisada[idxInter+1] = ImagenDisplayVector[posTEX+1];
		InterseccioOptimisada[idxInter+2] = ImagenDisplayVector[posTEX+2];
		//ImagenDisplayVector[posTEX+3];

		idxInter=idxInter+3;
	}
}


void Display::pegarInterseccioOptimisada(int idx)
{
	int* tablaInterseccion = SUBdisplay[idx].get_tablaInterseccion();
	int tam = SUBdisplay[idx].get_TamTablaInterseccion();

	int idxInter=0;
	for(int cnt=0; cnt < tam; cnt=cnt+2 )
	{
		long int posTEX   = tablaInterseccion[cnt];

		ImagenDisplayVector[posTEX+0] = InterseccioOptimisada[idxInter+0];
		ImagenDisplayVector[posTEX+1] = InterseccioOptimisada[idxInter+1];
		ImagenDisplayVector[posTEX+2] = InterseccioOptimisada[idxInter+2];
		//ImagenDisplayVector[posTEX+3];

		idxInter=idxInter+3;
	}

}


void Display::resetInterseccio(int idx, int roig, int vert, int blau)
{
	//int idx=0;
	int* tablaInterseccion = SUBdisplay[idx].get_tablaInterseccion();
	int tam = SUBdisplay[idx].get_TamTablaInterseccion();

	for(int cnt=0; cnt < tam; cnt=cnt+2 )
	{
		long int posTEX   = tablaInterseccion[cnt];

		ImagenDisplayVector[posTEX+0]=roig;
		ImagenDisplayVector[posTEX+1]=vert;
		ImagenDisplayVector[posTEX+2]=blau;
		//ImagenDisplayVector[posTEX+3];
	}
	//refresca=true;
	dibuixar();
}



void Display::mascara(int posTEX)
{
	// SON TEXELS (per lo que son mes grans)
	int mascara=3;
	float grau = mascara+1;

	int Roig=0,Vert=1,Blau=2;
	float sumaR=0.0;
	float sumaG=0.0;
	float sumaB=0.0;
	int N=0;

	int anchoPant = (int)floor(ImagenDisplayTextura.GetWidth());
	int altoPant  = (int)floor(ImagenDisplayTextura.GetHeight());

	for(int j=-mascara; j<mascara; j++ ) {
		for(int i=-mascara; i<mascara; i++ ) {

			//float k = grau - max(abs(j), abs(i));
			//float pes  = k/grau;
 
			sumaR += ImagenDisplayVector[posTEX+(j*anchoPant*4)+i*4+Roig];	//ImagenDisplayVector[posTEX+0]
			sumaG += ImagenDisplayVector[posTEX+(j*anchoPant*4)+i*4+Vert];	//ImagenDisplayVector[posTEX+1]
			sumaB += ImagenDisplayVector[posTEX+(j*anchoPant*4)+i*4+Blau];	//ImagenDisplayVector[posTEX+2]
												//ImagenDisplayVector[posTEX+3]
			N++;
		}
	}

	sumaR=sumaR/N;
	sumaG=sumaG/N;
	sumaB=sumaB/N;

	ImagenDisplayVector[posTEX]   = (int)floor(sumaR);
	ImagenDisplayVector[posTEX+1] = (int)floor(sumaG);
	ImagenDisplayVector[posTEX+2] = (int)floor(sumaB);
	//ImagenDisplayVector[posTEX+3] = 255;

}

int Display::buscar( int idC, int idSV )
{
	int idx = -1;

	for( int ind=0; ind<num_conexions ; ind++)
	{
		int idCam	= SUBdisplay[ind].getIdCam();
		int idSisVis	= SUBdisplay[ind].getIdSV();
		if( idCam==idC && idSV==idSisVis ) {
			//cout << "Indice: " << ind << ", idCam: " << idCam << ", idSisVis: " << idSisVis << endl; 
			return ind;
		}
	}

	return idx;

}


/*******************************************************************
	Dibuixa el patro en el display per a calcular la calibracio
********************************************************************/
void subDisplay::dibuixarPatroCalibracio (void)
{
	display->dibuixarPatroCalibracio(OFFSET_X, OFFSET_Y, escala);
}




/**********************************************************************************************************************
Clase subDisplay
**********************************************************************************************************************/
subDisplay::subDisplay( Display* disp, int* csock, int ordeCal )
{
	idCamara=0;
	tamTabla=0;	tamInterseccio=0;
	ordenes1 = 0;	ordenes2 = 0;
	estadoCalibracion = 0;
	camaraMaster=0; ordeCalibracio=0;
	ordeCalibracio = ordeCal;
	calibrada=false;
	reset=false;

	OFFSET_X=0.0; OFFSET_Y=0.0; escala=1.0;

	strcpy( ruta_patron, "./configuracio/p" );

	isTablaRecibida=false;
	isTablaInterseccionRecibida=false;

	display = disp;
	sock    = csock;
	ImagenDisplayVector = display->pImagenDisplayVector();
	ImagenDisplayTextura= display->pImagenDisplayTextura();

	cout << "DISPLAY: " << display << endl;

	tabla_calibracion  = (int*)		malloc ( sizeof(int)  * 900*480*2 );
	texturaComprimida  = (unsigned char*)	malloc ( sizeof(char) * 900*480   );
	tablaRecibida 	   = (char*) 		malloc ( sizeof(int)  * 900*480*2 );
	tablaInterseccion  = (int*)		malloc ( sizeof(int)  * 900*480*2 );
	imagenInterseccion = (unsigned char*)	malloc ( sizeof(char) * 900*480   );
	
	memset( tabla_calibracion  , '\0', sizeof(int ) * 900*480*2 );
	memset( texturaComprimida  , '\0', sizeof(char) * 900*480 );
	memset( tablaRecibida      , '\0', sizeof(int ) * 900*480*2 );
	memset( tablaInterseccion  , '\0', sizeof(int ) * 900*480*2 );
	memset( imagenInterseccion , '\0', sizeof(char) * 900*480 );

}

void subDisplay::dibuixar (void)
{
	display->dibuixar();
}


/*
 * Recibe Cabecera
 * 	Chequea estados
 *	Genera Acciones
 * Crea Cabecera
 *	id    :  X
 *	Estat :  C·T·C
 * Envia Cabecera
 *
*/
int subDisplay::recibirCabecera (int* csock1)
{
	int pos = 0;

	/**** Recibir Tabla de Calibración ***/
	char cabecera[19]="";
	memset (cabecera, '\0', sizeof(cabecera));
	string tamanoTabla="0000000";
	string tamanoInterseccio="0000000";

	// Cabecera
	if(recv(*csock1,&cabecera,19,0)==-1)
		cout << "Error recibiendo" << endl;

	char id[2] ="";
	id[0] = cabecera[pos];
	idCamara = (int)atoi((const char*)id);
	pos++;

	char id2[2] ="";
	id2[0] = cabecera[pos];
	idSistemaVisio = (int)atoi((const char*)id2);
	pos++;

	char camMast[2]="";
	camMast[0]= cabecera[pos];
	camaraMaster = (int)atoi((const char*)camMast);
	pos++;

	//if(!camaraMaster)
		estadoCalibracion = (int)cabecera[pos]-48;

	if( estadoCalibracion==yaHeCalibrado && !calibrada) {
		display->sumToken();
		calibrada=true;
	}

	//cout << "estadoCalibracion: " << estadoCalibracion << endl;
	pos++;
	int taul = (int)cabecera[pos];
	pos++;
	int xxxx = (int)cabecera[pos];
	pos++;

	// Tamany Textura
	for(int it=0; it<6; it++) {
		tamanoTabla[it] = cabecera[pos];
		pos++;
	}
	tamanoTabla[6]='\0';

	// Tamany Interseccio
	for(int it=0; it<6; it++) {
		tamanoInterseccio[it] = cabecera[pos];
		pos++;
	}
	tamanoInterseccio[6]='\0';


	tamTabla = (int)atoi(tamanoTabla.c_str());
	tamInterseccio = (int)atoi(tamanoInterseccio.c_str());
	//cout << "Recibida: " << cabecera << "  Tam1: " << tamTabla << "  Tam2: " << tamInterseccio << endl;


	// Inicialisa vector
	char acciones[9]="";
	memset(acciones, '\0', sizeof(acciones));
	//generaAccion();
	if (tamTabla && ordenes1==espera1){ //&& !camaraMaster
 		ordenes1 = recibeTabla;
	}
	if(isTablaRecibida ){ //&& !camaraMaster
		ordenes1 = recibeTextura;
	}
	acciones[0] = ordenes1;


	if (tamInterseccio && ordenes2==espera2){
		ordenes2 = recibeTablaInterseccion;
	}
	if(isTablaInterseccionRecibida) {
		ordenes2 = recibeInterseccion;
	}
	acciones[1] = ordenes2;

	// Envía Accion
	if( send(*csock1, &acciones, 9, 0 ) == -1 )
		cout << "Fallo enviar textura" << endl;

} 



/*
* Textura
* Recibe la Textura ya compensada
* La textura depende solo de su cámara correspondiente
* así que esta puede ser calculada localmente en esta Raspberry
* 
* Habrán dos buffers con la textura:
*    · Uno local (el calculado por el SLAVE) Este es virtual, no se visualiza
*    · Otro Remoto (el enviado al MASTER) Este es el que se visualiza
*/
int subDisplay::recibirTablaCalibracion (int* csock1)
{
	int error=0;
	int tam_a_rebre = tamTabla*sizeof(int);
	memset( tablaRecibida    , '\0', sizeof(int) * 900*480*2 );

	// Tabla Calibración
	int rebut;
	int rep=0;
	do{
		rebut = read(*csock1, &tablaRecibida[rep], tam_a_rebre-rep);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		return -1;	
	}

	memcpy(tabla_calibracion, tablaRecibida, tam_a_rebre);
	cout << "Tabla recibida " << rep << " de " << tam_a_rebre << endl;

	isTablaRecibida=true;

	return 1;

}


int subDisplay::recibirTextura (int* csock1)
{
	memset( texturaComprimida, '\0', sizeof(char) * 900*480 );
	int rebut;
	int tam_a_rebre = (int)(tamTabla/2*3); 	// <-----------

	int rep=0;
	do{
		rebut = recv(*csock1, &texturaComprimida[rep], tam_a_rebre-rep, 0);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		return -1;	
	}

	//cout << "rebuts = " << (int)texturaComprimida[0] << endl;
	//cout << "rebuts = " << rep << " de " << tam_a_rebre << endl;

	return 1;
}


/*
*
* Intersección
* Recibe la imagen de interseccion de la camara
* para que la Raspberry Master calcule la compensación
* Para la parte intersectada se precisan las imágenes de las
* cámaras que intersecten, así que ésta no puede calcularse localmente
*/
int subDisplay::recibirTablaInterseccion (int* csock1)
{	
	int error=0;
	int tam_a_rebre = tamInterseccio*sizeof(int);
	memset( tablaRecibida    , '\0', sizeof(char) * 900*480*2 );

	// Tabla Calibración
	int rebut;
	int rep=0;
	do{
		rebut = read(*csock1, &tablaRecibida[rep], tam_a_rebre-rep);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		return -1;	
	}

	memcpy(tablaInterseccion, tablaRecibida, tam_a_rebre);
	//cout << "Tabla Inteeeeeeeeeeeeeerseccion recibida" << tam_a_rebre << endl;

	isTablaInterseccionRecibida = true;

	return 1;

}


bool subDisplay::recibirInterseccio(int* csock1)
{

	memset( imagenInterseccion, '\0', sizeof(char) * 900*480 );
	int rebut;
	int tam_a_rebre = (int)(tamInterseccio/2); 	// <-----------

	//cout << "Recibiiiiiiiiiiiiiiiiiiiir " << tam_a_rebre << endl;

	int rep=0;
	do{
		rebut = recv(*csock1, &imagenInterseccion[rep], tam_a_rebre-rep, 0);
		rep = rep + rebut;
	}while(rebut>=0 && rep<tam_a_rebre);

	if(rebut==-1) {
		cout << "Error/Desconexión" << endl;
		return false;	
	}

	//cout << "Interseeeeeeeeeeeeeeeeeeccio = " << rep << endl;

	return true;

}


/*
*
*
*
*/
bool subDisplay::dibujarTextura ( void )
{
	if(!reset) {
		int idx=0;

		for(int cnt=0; cnt<tamTabla; cnt=cnt+2 )
		{
			//long int posIM  = tabla_calibracion[cnt];
			long int posTEX = tabla_calibracion[cnt+1];

			ImagenDisplayVector[posTEX  ] = (int)texturaComprimida[idx  ];
			ImagenDisplayVector[posTEX+1] = (int)texturaComprimida[idx+1];
			ImagenDisplayVector[posTEX+2] = (int)texturaComprimida[idx+2];

			idx += 3;
		}
	}
	else {
		for(int cnt=0; cnt<tamTabla; cnt=cnt+2 ) {
			long int posTEX = tabla_calibracion[cnt+1];

			ImagenDisplayVector[posTEX  ] = 0;
			ImagenDisplayVector[posTEX+1] = 0;
			ImagenDisplayVector[posTEX+2] = 0;
		}
	}

}

void subDisplay::resetTextura( int roig, int vert, int blau)
{
	reset=true;

	for(int cnt=0; cnt<tamTabla; cnt=cnt+2 )
	{
		//long int posIM  = tabla_calibracion[cnt];
		long int posTEX = tabla_calibracion[cnt+1];

		ImagenDisplayVector[posTEX  ] = roig;
		ImagenDisplayVector[posTEX+1] = vert;
		ImagenDisplayVector[posTEX+2] = blau;
	}
	usleep(5000000);
	reset=false;
}


/*
*
*
*
*/
bool subDisplay::loadPatron (void)
{
	if(!existe(ruta_patron))
		cout << "No existe archivo: " << ruta_patron << endl;
	else
	{
		cout << "Cargando datos de Patron... " << ruta_patron << endl;
		FileStorage fs(ruta_patron, FileStorage::READ);
		fs["OFFSET_X"] >> OFFSET_X;
		fs["OFFSET_Y"] >> OFFSET_Y;
		fs["escala"]   >> escala;
		
		return true;
	}
	
	return false;
}


/*
*
* Crear la ruta del patron
*
*/
void subDisplay::crear_ruta_patron (void)
{
	string conf = int2string(4); //num_camaras
	strcat( ruta_patron, conf.c_str() );

	string sistVis = int2string( idSistemaVisio );  //idSV
	strcat( ruta_patron, sistVis.c_str() );

	string sancer = int2string( idCamara );
	strcat( ruta_patron, sancer.c_str() );

	strcat( ruta_patron,".xml");

	cout  << ruta_patron << endl;
}



