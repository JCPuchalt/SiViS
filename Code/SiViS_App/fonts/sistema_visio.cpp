#pragma once

#include "../llibreries/llibreries.hpp"
#include "../llibreries/sistema_visio.hpp"

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



/***************************************************************************************
Constructuor de Clase: "sistema_visio". Inicio de clase. Crea, inicializa... variables
****************************************************************************************/
sistema_visio::sistema_visio(int id, int conf, int num_cam, int configPort[], Display* disp, int cicles, int IP_Port)
{
	con_conexion=false; conectat=false;   proyecte_nou=false;    bloquejat=false;
	G_Local=true;       enviar=false;     capturando=false;      inici_local=false;
	inici_remot=false;  iniciat=false;    proyecto_creado=false; abortar=false;
	fin_captura=false;  apagar_RPI=false; 

	foto=false;
	compensada=true;
	monoCamara = false;

	//fusionada	= (unsigned char*) malloc (sizeof(char)      * WIDTH_FRAME*HEIGHT_FRAME*4);

	numMuestras=30;
	estat_enviament = 8; //LLIURE;
	Port_IP = IP_Port;

	idSV=id;
	config=conf;
	display=disp;
	num_camaras = num_cam;
	CICLES=cicles;

	strcpy(ruta_tablaCalibracionFusion    	, "./configuracio/tablaCalibracionFusion");
	crear_ruta_tablaCalibracionFusion();

	strcpy(ruta_tablaInterseccion    	, "./configuracio/tablaInterseccion");
	crear_ruta_tablaInterseccion();

	strcpy(ruta_idCAMARA    		, "./configuracio/idCAMARA");
	crear_ruta_idCAMARA();

	buffer = new bufferCircular();

	ImagenDisplayVector  = display->pImagenDisplayVector();
	ImagenDisplayTextura = display->pImagenDisplayTextura();

	codicImage=0;
	memcpy(numPort, configPort, 4*sizeof(int));

	imagen   = (unsigned char*) malloc (sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME * num_cam);
	evaluado = (unsigned char*) malloc (sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME * num_cam);
	memset ( evaluado, 0, sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME * num_cam );


	tabla_calibracionFusionInversa = (long int*) malloc (sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_cam * 2);
	memset ( tabla_calibracionFusionInversa, 0, sizeof(long int) * WIDTH_FRAME * HEIGHT_FRAME * num_cam * 2);


	idCAMARA = (unsigned char*) malloc (sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME * num_cam * 2);
	memset ( idCAMARA, 0, sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME * num_cam * 2);


	tabla_Interseccion = (int*) malloc ( sizeof(int) * WIDTH_PANT * HEIGHT_PANT * num_cam );
	memset ( tabla_Interseccion, 0, sizeof(int) * WIDTH_PANT * HEIGHT_PANT * num_cam );

	NUEVA_TABLA = (int*) malloc ( sizeof(int) * (int)floor(WIDTH_FRAME * HEIGHT_FRAME*0.3) );
	memset(NUEVA_TABLA, 0, sizeof(int) * (int)floor(WIDTH_FRAME * HEIGHT_FRAME*0.3) );


	detectadaInterseccion = (unsigned char*) malloc ( sizeof(char) * WIDTH_PANT * HEIGHT_PANT * num_cam * 4);
	memset ( detectadaInterseccion, 0, sizeof(char) * WIDTH_PANT * HEIGHT_PANT * num_cam * 4);


	//detectada = (unsigned char*) malloc ( sizeof(char) * WIDTH_PANT * HEIGHT_PANT * num_cam * 4);
	//memset ( detectada, 0, sizeof(char) * WIDTH_PANT * HEIGHT_PANT * num_cam * 4);


	// Definir ports del Multiplexor
			//pin  7,11,12
	int portMUX[4][3]={   {0, 0, 1},   //PORT CAM1
			      {1, 0, 1},   //PORT CAM2
			      {0, 1, 0},   //PORT CAM3
			      {1, 1, 0} }; //PORT CAM4


	// Crear Camares
	//CAM = new camara[num_camaras](1);
	CAM = reinterpret_cast<camara *>(new char[num_camaras * sizeof(camara)]);
	for (int i = 0; i < num_camaras; i++) {
    		new(&CAM[i]) camara(i+1, num_camaras, this);
	}


	// Si es Sistema de Vision Principal Se creará un hilo de conexión tipo servidor para
	// recibir las calibraciones
	if( SV_Principal ) {
		pthread_create(&conexion_id, NULL, crear_Servidor_SistemaVison_Master, this );
	}

	
	bool _AUTONOMO=false;


	// Configurar Camares
	for (int i=0; i<num_camaras; i++)
	{	
		int nPort = numPort[i];

		//CAM[i].SetId(i+1);
		CAM[i].setDisplay(display);
		CAM[i].pEvaluado(evaluado);
		CAM[i].pDetectarInterseccion(detectadaInterseccion);
		CAM[i].setCaracteristicas(num_cam, idSV, nPort, config);
		CAM[i].setPort ( portMUX[nPort-1][0], portMUX[nPort-1][1], portMUX[nPort-1][2]);
		CAM[i].crear_ruta_homografia();
		CAM[i].crear_ruta_patron();
		CAM[i].crear_ruta_tablaCalibracion();
		CAM[i].crear_ruta_homografiaPlanoPlaca();
		CAM[i].crear_ruta_tablaInterseccion();
		CAM[i].loadPatron();
	}


	// Calibraciones (hay dos tipos: Local y Remota)

	// Calibraciones Locales
	for (int i=0; i<num_camaras; i++)
	{
		if(1) //!_AUTONOMO
		{
			if ( CAM[i].GetId() == idCam_Ppal ||  withMUX)
			{
				cout << "calibracio local..." << CAM[i].GetId() << endl;
				if( CAM[i].CE_Calibrar() )
				{
					cout << "Calibrada cámara local: " << i+1 << endl << endl;
					CAM[i].enviaAlMaster();
					//-----parla=true;
				}
			}
		}
	}

	// Calibraciones Remotas.
	for ( int i=0; i<num_camaras; i++ )
	{
		if ( !withMUX ) // SV_Secundario &&  //CAM[i].GetId() != idCam_Ppal && !withMUX)
		{
			cout << "calibracio remota cámara: " << CAM[i].GetId() << endl;
			//CAM[i].solicitarDibuixarPatro();
			CAM[i].preparadaComunicar(true);
			// ---------------------------
			CAM[i].loadHomographyInverseParams();
			CAM[i].loadHomographyNivelPlaca();
			// ---------------------------
			while( !CAM[i].isMatrizCalibracionRecibida() && !CAM[i].isMatrizCalibracionEnviada() )
			{
				CAM[i].enviaAlMaster();
				display->dibuixar();
				usleep(100000);
			}
			CAM[i].CE_CalibrarRemoto();
		}
	}
	cout << "Calibraciones remotas realizadas" << endl;
	display->colorDisplay(0,0,0,255);
	display->dibuixar();

	if( Cantitat_Camares>1 ) {
		if(1) //!_AUTONOMO)
		{
			detectarInterseccionesX2();
			//detectarInterseccionesX4();

			// ---------------*****************----------------------------------------
			cout << "NO GUSTAR" << endl;
			memset ( detectadaInterseccion, 0, sizeof(char) * WIDTH_PANT * HEIGHT_PANT * 2 * 4);
			//if( (calibrada && recalibrar) || !existeArchivo )
			{
				for (int i=0; i<num_camaras; i++)
				{
					CAM[i].Calculo_Tabla_Calibracion();
					CAM[i].Init_Textura();
				}

				cout << "Creando tabla para los pixeles de interseccion" << endl;
				calculoPixelesIntersectados();
			}
		}


		/*
		// Comprobar si tiene que Calibrar
		int calibra = 0;
		for (int i=0; i<num_camaras; i++)
			calibra+=(int)(!CAM[i].ExisteTabla())+(int)CAM[i].isRecalibrada();

		*/
		// Calcula la tabla 
		//if (calibra)
			calibracioDeFussio();
		//else
		

	
		//if(!recalibrar) || _AUTONOMO
		{
			for (int i=0; i<num_camaras; i++)
			{
				CAM[i].loadTablaCalibracion();
				// Tabla interseccion por cada cámara
				CAM[i].load_TablaInterseccion();
			}
			loadTablaInterseccion();
			loadTablaFusionInversa();
		}
	}
	else { // Si solo hay una cámara por Sistema de visión
		for (int i=0; i<num_camaras; i++)
		{
			bool UNAimagen=true;
			CAM[i].Calculo_Tabla_Calibracion(UNAimagen);
		}
	}

	crearFils();

	cout << "Constructor Sistema Visión: Hecho" << endl;

}


void sistema_visio::crearFils (void)
{
	// Crear Una conexio al Servidor. Per a cada Sistema de Visio
	//fils* fil = new fils();
	//fil->StartInternalThread(this);

	// Crear un thread para guardar las imágenes
	pthread_create(&grabar_id, NULL, CE_Guardar, this );
}


/*********************************************************
Proceso
**********************************************************/
void sistema_visio::proceso(void)
{

	display->dibuixar();
	
	foto=false;
	if(ch == 'f') {
		foto=true;
		ch='a';
		cout << "fottttttttttttttttttttttttooooooooooooooooo" << endl;
	}

//	inicialisarCaptura();
//	sincronisacio();
	
	if(Cantitat_Camares==1) {
		// Compensacio Rapida de les camares locals ()
		for (int iC=0; iC < num_camaras; iC++)
		{
			if ( CAM[iC].GetId() == idCam_Ppal ||  withMUX)
			{
				// Compensación rápida
				if( !CAM[iC].found_min() ) {
					CAM[iC].CE_Compensar_Rapido();
					//CAM[iC].saveTexturaCompensada();
					//saveTexturaInterseccion();
					codicImage=0;
				}
			}
		}
	}

	struct timespec t0 = Inicia_Temporisador();
	for (int iC=0; iC < num_camaras; iC++)
	{
		// Procesa si la Cámara es Local o si Se utiliza MUX
		if ( CAM[iC].GetId() == idCam_Ppal ||  withMUX)
		{
			// Captura
			bool unaCaptura = false;
			CAM[iC].CE_Capturar (unaCaptura);
			// FIN Captura


			if( Cantitat_Camares>1 ) { // Si el Sistema de Visio te mes d'una Camara

				// Compensaciones
				// Compensaciones del Display
				if ( withMUX ) {  
					if( !CAM[iC].found_min() ) {
						CAM[iC].CE_Compensar_Rapido();
						codicImage=0;
					}
				}
				if ( !withMUX ) {
					CAM[iC].CE_Compensar();
				}

				// Comprimir Interseccion de la Textura para enviar al Master
				CAM[iC].compresionImagenInterseccion();
				// Orden para enviar Textura e Intersección al Master
				CAM[iC].enviaAlMaster();
				// FIN compensaciones


				// Fusiona la imagen de 1 cámara
				CAM[iC].fussioParcial( imagen, foto );
				// Comprime la imagen intersectada de 1 cámara
				CAM[iC].fussioInterseccioParcial(imagen);

				struct timespec t1 = Inicia_Temporisador();

				if(SV_Principal) {
					// Procesa la Intersección si es SV Principal
					procesarInterseccionParcial (foto);
					// Añade, comprime y envía a la Fusionada la Intersección
					CAM[iC].enviarInterseccioReconstruida( imagen, anticTamanyTabla );
				}

				cout << "Temps deteccio: "<< Medix_temps(t1) << endl;
				// Orden para enviar los resultados de fusión
				if(SV_Secundario) {
					CAM[iC].enviaAl_SV_Principal();
				}
				// FIN fusiones

			}
			else { // Si el Sistema de Visio te nomes una Camara
				// Compensaciones del Display
				if ( withMUX ) {  
					if( !CAM[iC].found_min() ) {
						CAM[iC].CE_Compensar_Rapido();
						codicImage=0;
					}
				}
				imagen = CAM[iC].Get_Vect_Im();
			}


			//cout << CAM[iC].Calcular_Luz_Media() << endl;
			CAM[iC].CE_Error_Compensacion();
			//CAM[iC].CE_Compensar_Rapido();

		}
	}
	int estat = Get_Estat_Enviament();
	//CAM[0].enviarImage();
	//cout << "estat -> " << estat << endl;
	//cout << "Temps bucle: "<< Medix_temps(t0) << endl;

	// -------------
	int numCamLocals = 0, estadoCapturando=0;
	for (int iC=0; iC < num_camaras; iC++)
	{
		if ( CAM[iC].GetId() == idCam_Ppal ||  withMUX)
		{
			estadoCapturando += CAM[iC].getCapturando();
			numCamLocals++;
		}
	}
	//cout << "numCamLocals: " << numCamLocals << ", estadoCapturando: " << estadoCapturando << endl;
	if ( estadoCapturando == numCamLocals ) {
		capturando=true;
		codicImage++;
	} else {
		capturando=false;
		codicImage=0;
	}


//	compensacionInterseccion();

	/*
	* Funciones de fusion
	*/
	//fussio_imagens();
	// Fusiona en una imagen única
//	fussioInversa_imagens();
	// Realiza la fusión de la imagen de manera parcial en dos imágenes diferentes
//	fussioInversa_imagensIndividual();


	// Genera ruta de imagen a grabar para modo autonomo
	int totalBytes = 2592*2592;
	char direccionDeGrabacion[150]="";
	//generadorDeRuta( direccionDeGrabacion );

	// Añadir al buffer para guardar en Memoria
	//buffer->addImage( imagen, direccionDeGrabacion, totalBytes );
	//Sum_It_Imagens();

}


/*
* @ Fussiona en una les dos taules de Fussio de les dos camares
*
*/
void sistema_visio::fusionarTablasDeFusion (void)
{

	if(!monoCamara) {

		int  maxim_1		= CAM[0].get_maximIdx();
		int  minim_1		= CAM[0].get_minimIdx();		
		int  tam_1		= CAM[0].getTamInterseccionParcial();
		long int* tabla_1 	= CAM[0].get_tablaInterseccionParcial();

		int  maxim_2		= CAM[1].get_maximIdx();
		int  minim_2		= CAM[1].get_minimIdx();		
		int  tam_2		= CAM[1].getTamInterseccionParcial();
		long int* tabla_2 	= CAM[1].get_tablaInterseccionParcial();

		int minim = min( minim_1, minim_2 );
		int maxim = max( maxim_1, maxim_2 );

		cout << "Calculando TABLA tam_1: " << tam_1 << endl;
		// Evaluar taula 1
		for(int cnt=0; cnt<tam_1; cnt++) {
			int posFus = tabla_1[cnt] - minim;
			NUEVA_TABLA[posFus] = posFus + minim;
		}

		cout << "Calculando TABLA tam_2: " << tam_2 << endl;
		// Evaluar taula 2
		for(int cnt=0; cnt<tam_2; cnt++) {
			int posFus = tabla_2[cnt] - minim;
			NUEVA_TABLA[posFus] = posFus + minim;
		}

		// Agarrar senyes de la camara Master
		int tam_FUS; long int* tabla_FUS; int idCamara;
		for( int iCam=0; iCam<num_camaras ; iCam++ ) {
			if ( CAM[iCam].GetId() == idCam_Ppal) {
				tam_FUS	  = CAM[iCam].get_tamFusionadaParcial();	// getTamInterseccionParcial();
				tabla_FUS = CAM[iCam].get_tablaFusionadaParcial();	// get_tablaInterseccionParcial();
				idCamara  = iCam;
			}
		}

		// Unir a la Tabla Imagen Parcial, y se tiene una Tabla Parcial-Intersección
		cout << "Calculando fusión ... DE TABLAS ..." << endl; 
		int possibleTamany = maxim - minim;
		int contador=0;		
		for(int cnt=0; cnt<possibleTamany; cnt++ ) {
			int posFus = NUEVA_TABLA[cnt];
			if(posFus!=0) {
				tabla_FUS[tam_FUS+contador+1] = posFus;
				contador += 2;
			}
		}
		
		// Canviar els tamanys de 
		CAM[idCamara].set_tamFusionadaParcial(tam_FUS + contador);
		anticTamanyTabla = tam_FUS; //cnt + tam_i
 		cout << "TAMANY ANTIC: " << tam_FUS << "" << ", NOVA TAULA ----> " << contador << ", total: " << tam_FUS + contador << endl;

	}
}


void  sistema_visio::finCapturaSV (void)
{
	for(int iCam=0; iCam<num_camaras ; iCam++) {
		CAM[iCam].finCapturaCamara();
	}
}


void sistema_visio::procesarInterseccionParcial(bool foto)
{
	if(SV_Principal) {
		for(int iCam=0; iCam<num_camaras ; iCam++) {
			CAM[iCam].procesarInterseccionParcial(foto); 
		}
	}

	detectarCElegans (foto);
	reconstruirInterseccio(foto);
}


void sistema_visio::reconstruirInterseccio ( bool foto )
{
	vector <objetos>* objectes1;
	vector <objetos>* objectes2;

	objectes1 = CAM[0].getObjectes();
	objectes2 = CAM[1].getObjectes();

	// Quan se modifica esta Matriu se modifica tambe el buffer "imagen"
	interseccioReconstruida = Mat (Size(2592, 2592 ), CV_8UC1, imagen);

	int llimit1 = CAM[0].getLlimit(); // 77;
	int llimit2 = CAM[1].getLlimit(); // 60;
	
	// Image de Dalt (CAM 2) ( primer Copiar )
	copiarImage ( interseccioReconstruida, *CAM[1].get_interseccioGrisos(),  llimit2);  // (+) Dalt

	// Image de Baix (CAM 1) (primer borrar objeces, despuix copiar image, per ultim pergar objectes de CAM 2 en CAM 1 )
	// Borrar Objectes
	for(int element=0; element<relacions.size(); element++ ) {
		int i1 = relacions[element].element1;
		int i2 = relacions[element].element2;
		if(i1>=0 && i2>=0) {
			objectes1->at(i1).eliminarObjeto ( *CAM[0].get_interseccioGrisos(), -llimit1 );
		}
	}


	// Copiar image CAM 1
	copiarImage ( interseccioReconstruida, *CAM[0].get_interseccioGrisos(), -llimit1); // (-) Baix

	// Pegar objectes de CAM2 a image reconstruida
	for(int element=0; element<relacions.size(); element++ ) {
		int i1 = relacions[element].element1;
		int i2 = relacions[element].element2;
		if(i1>=0 && i2>=0) {
			objectes2->at(i2).copiarObjeto   ( interseccioReconstruida, *CAM[1].get_interseccioGrisos() );
		}
	}

	//if(foto) imwrite( "./InterseccioBorrada1.bmp", *CAM[0].get_interseccioGrisos() );
	if(foto) imwrite( "./InterseccioRecontruida.bmp", interseccioReconstruida ); 

}



/*
* @ Copia una matriu a una atra fins a un llimit en Y
*/
void sistema_visio::copiarImage ( Mat& desti, Mat& orige, int llimit )
{
	int limite = abs(llimit);

	if(llimit<0) {  // Baix
		for(int idx=0; idx<orige.size().width; idx++) {
			for ( int idy=0; idy<orige.size().height; idy++ ) {
				if( idy > limite ) {
					int desfase = CAM[0].get_FYmin();
					desti.at<uchar>(idy+desfase, idx) = orige.at<uchar>(idy, idx);
				}
			}
		}
	}
	if(llimit>0) {  // Dalt
		for(int idx=0; idx<orige.size().width; idx++) {
			for ( int idy=0; idy<orige.size().height; idy++ ) {
				if( idy <= limite ) {
					int desfase = CAM[1].get_FYmin();
					desti.at<uchar>(idy+desfase, idx) = orige.at<uchar>(idy, idx);  //idy+variacioY, idx
				}
			}
		}
	}
}


void sistema_visio::detectarCElegans(bool foto)
{
	vector <objetos> CElegans;

	// Cerca de la mitad del área de interés (y=60)
	vector <objetos>* objectes1;
	vector <objetos>* objectes2;

	objectes1 = CAM[0].getObjectes();
	objectes2 = CAM[1].getObjectes();
	
	int Obj1N = objectes1->size();
	int Obj2N = objectes2->size();


	// ¿Netejar?!!!!!! Si se neteja se pert el concepte TEMPORAL
	relacions.clear();


	if( Obj1N>=0 && Obj2N>=0 ) {

		for (int iO1=0; iO1<(int)objectes1->size(); iO1++  ) {

			float area1 = objectes1->at(iO1).area();

			for (int iO2=0; iO2<(int)objectes2->size(); iO2++  ) {

				struct relaciones relacio;

				// Provabilitat per tamany
				float area2 = objectes2->at(iO2).area();
				float mijaArea= 0.0, desviacioArea=0.1;
					//float numerador = min(area2, area1);
					float denominador = max(area2, area1);
				float varArea = (float)(area2-area1)/(float)denominador;
				float pTamany = gaussian_pdf ( varArea, mijaArea, desviacioArea );

				// Provabilitat per distancia
				Point2f distancia = objectes1->at(iO1).centroide(true) - objectes2->at(iO2).centroide(true);
				float modulo = sqrt (pow(distancia.x,2) + pow(distancia.y,2));

				float mijaDistancia=0.0, desviacioDistancia=6.0;
				float pDistancia = gaussian_pdf ( modulo, mijaDistancia, desviacioDistancia );

				// Provabilitat Total 
				float pTotal = (pTamany+pDistancia)/2.0;

				// Introduir valors en la relacio
				relacio.element1  = iO1;
				relacio.element2  = iO2;
				relacio.distancia = modulo;
				relacio.pTotal    = pTotal;
				relacio.pDistancia= pDistancia;
				relacio.pTamany   = pTamany;
				relacio.area1     = area1;
				relacio.area2     = area2;

				relacions.push_back(relacio);
			}

		}
/*
		// Matriu en totes les combinacions de distancies
		float distancies[Obj1N][Obj2N]={};
	
		// Emparellar els objectes (Els desparellats no son CElegans )
		for (int iO1=0; iO1<(int)objectes1->size(); iO1++  ) {
			for (int iO2=0; iO2<(int)objectes2->size(); iO2++  ) {

				Point2f distancia = objectes1->at(iO1).centroide(true) - objectes2->at(iO2).centroide(true);
				float modulo = sqrt (pow(distancia.x,2) + pow(distancia.y,2));
				distancies[iO1][iO2] = modulo;

			}

		}

		// Buscar mes proxim
		for (int iO1=0; iO1<(int)objectes1->size(); iO1++ ) {

			struct relaciones relacio = {-1.0, -1, -1};
			relacions.push_back(relacio);
			float minim = 100000.0;
			float area1 = objectes1->at(iO1).area();
			//float pTamany = gaussian_pdf ( float x, float mhu, float sigma );

			for (int iO2=0; iO2<(int)objectes2->size(); iO2++ ) {

				//float area2 = objectes2->at(iO2).area();

				if( distancies[iO1][iO2]<minim  ) {
					
					minim = distancies[iO1][iO2];

					relacions.at(iO1).element1  = iO1;
					relacions.at(iO1).element2  = iO2;
					relacions.at(iO1).distancia = minim;
				}
			}
		}
*/

		// Eliminar les relacions duplicades
		for(int element1=0; element1<relacions.size(); element1++ ) {
			// Si algun element de 1 no ha trobat cap element 2
			if( relacions[element1].element1==-1 || relacions[element1].element2==-1 ) {
				relacions.erase(relacions.begin()+element1);
			}
			else {
				int idElement2 = relacions[element1].element2;
				for(int element2=0; element2<relacions.size(); element2++ ) {
					// Si els elements 1 i 2 son iguals (i no son el mateix) El BORRA
					if ( relacions[element2].element2 == idElement2 && element1!=element2 ) {
						// Borra el mes gran
						//if( relacions[element1].distancia >  relacions[element2].distancia) {
						if( relacions[element1].pTotal <  relacions[element2].pTotal) {
							relacions.erase(relacions.begin()+element1);
							element1--;
							break;
						}
						// Borra el mes gran
						//else if( relacions[element1].distancia <= relacions[element2].distancia ) {
						else if( relacions[element1].pTotal >= relacions[element2].pTotal ) {
							relacions.erase(relacions.begin()+element2);
							element2--;
						}
					}
				}
			}

		}

		for(int element=0; element<relacions.size(); element++ ) {

			float pDistancia = relacions[element].pDistancia;
			float modulo	 = relacions[element].distancia;
			float pTamany	 = relacions[element].pTamany;
			float area1	 = relacions[element].area1;
			float area2	 = relacions[element].area2;
			float pTotal	 = relacions[element].pTotal;
			
			//cout << "pDistancia: " << pDistancia << " - modulo: " << modulo <<  ", pTamany: " << pTamany << " - area1: " << area1 << ", area2: " << area2 << endl;
			//cout << "pTotal: " << pTotal << endl;
		}
		if(foto) {
			// Dibuixar les relacions
			RNG rng(12345);
			//Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
			Mat drawing ( Size(2592,160), CV_8UC3, Scalar(0,0,0) );
			for(int element=0; element<relacions.size(); element++ ) {

				int i1 = relacions[element].element1;
				int i2 = relacions[element].element2;

				if(i1>=0 && i2>=0) {

					char str1[10]="";
					sprintf(str1,"%i",i1);

					Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
					circle		( drawing, objectes1->at(i1).centroide()	 , (int)objectes1->at(i1).radius()	, color, 1, 8, 0 );
					putText		( drawing, str1, objectes1->at(i1).centroide(), 1, FONT_HERSHEY_PLAIN, color, 1, 8, false );

					char str2[10]="";
					sprintf(str2,"%i",i2);

					circle		( drawing, objectes2->at(i2).centroide()	 , (int)objectes2->at(i2).radius()	, color, 1, 8, 0 );
					putText		( drawing, str2, objectes2->at(i2).centroide(), 1, FONT_HERSHEY_PLAIN, color, 1, 8, false );
				}
			}
			imwrite( "./CElegans.bmp", drawing);
		}
	}

}	

void sistema_visio::generadorDeRuta( char direccionDeGrabacion[] )
{

	//char generandoDireccion[150]="./Raspberry/dia_";
char generandoDireccion[150]="/media/usb0/home/pi/Pictures/bateria/dia_";

	string numCicles = int2string(CICLES);
	strcat( generandoDireccion, numCicles.c_str() );
	strcat( generandoDireccion, "/cond_A/placa_1");
	
	strcat( generandoDireccion, "/RASP01_");
	string sequencia = NOM_000X (codicImage);
	strcat( generandoDireccion, sequencia.c_str() );

	strcat( generandoDireccion, ".bmp");

	strcpy( direccionDeGrabacion, generandoDireccion);

	if(!existe(direccionDeGrabacion))
	{
		crear_carpeta (direccionDeGrabacion);
		cout << "Se ha generado carpeta" << endl;
	}


}



void sistema_visio::inicialisarPinsGPIO(void)
{
	pinMode( bit_0, OUTPUT);
	pinMode( bit_1, OUTPUT);
	pinMode( bit_2, OUTPUT);
	pinMode( bit_3, OUTPUT);
	pinMode( bit_4, OUTPUT);
	pinMode( bit_5, OUTPUT);
	pinMode( bit_6, OUTPUT);
	pinMode( bit_7, OUTPUT);
	pinMode( bit_OK, OUTPUT);
	pinMode( bit_sync, OUTPUT);

	digitalWrite( bit_0, 0);
	digitalWrite( bit_1, 0);
	digitalWrite( bit_2, 0);
	digitalWrite( bit_3, 0);
	digitalWrite( bit_4, 0);
	digitalWrite( bit_5, 0);
	digitalWrite( bit_6, 0);
	digitalWrite( bit_7, 0);
	digitalWrite( bit_sync, 0);
	digitalWrite( bit_OK, 0);
}


void sistema_visio::recibeDisplay(void)
{
	unsigned char* byte;
	int length=1;
	int channel=0;

}



/************************************************
Inicialisar Captura
************************************************/
void sistema_visio::inicialisarCaptura (void)
{
	if( inici_remot && !iniciat )
	{
		iniciat=true;
		inici_remot=false;

		codicImage=0;
		capturando=true;
		cout << "Inicio captura" << endl;

		/*for (int iSV=0; iSV < num_camaras; iSV++){
			CAM[iSV].Set_Reset_Comp(true);
			CAM[iSV].found_min (false);
		}*/

	}
}



bool sistema_visio::getEstatCompensada ( void )
{
	bool compensadas=true;
	for (int iD=0; iD < num_camaras; iD++) {
		compensadas = compensadas && CAM[iD].getEstatCompensada();
		//cout << "compensadas: " << compensadas;
	}
	return compensadas;
}


bool sistema_visio::isMinimFound ( void )
{
	minimFound=true;
	for (int iD=0; iD < num_camaras; iD++) {
		minimFound = minimFound && CAM[iD].found_min();
	}

	return minimFound;

}

void sistema_visio::recompensar (void)
{
	for (int iD=0; iD < num_camaras; iD++) {
		cout << "Recompensa" << endl;
		CAM[iD].Set_Reset_Comp(true);
		CAM[iD].found_min (false);
	}

}


/*********************************************************
Sincronisacio
**********************************************************/
void sistema_visio::sincronisacio(void)
{	/*
	bool compensadas=true;
	for (int iD=0; iD < num_camaras; iD++) {
		compensadas = compensadas && CAM[iD].getEstatCompensada();
	}
	compensada = compensadas; */

	if( capturando && !compensando )
	{
		cout << "Capturando" << endl;
		codicImage++;
		cout << codicImage << endl;
	}

	if( capturando && codicImage>(numMuestras) || abortar)
	{
		fin_captura=true;
		capturando=false;
		iniciat=false;
		codicImage=0;
	}

	if( (capturando && codicImage>(numMuestras) ) || abortar)
	{
		capturando  = false;
	}
}



/************************************************

************************************************/
void sistema_visio::fussio_imagens(void)
{
	if(num_camaras==1)
		imagen= CAM[0].Get_Vect_Im();

	//if(num_camaras==2)

	if(num_camaras==4)
	{
		//unsigned char* imageReconstruida;
		for(int iSV=0; iSV < num_camaras; iSV++)
		{
			CAM[iSV].fusionarImagen(imagen);
		}

		//imagen = imageReconstruida;
	}
}



void sistema_visio::fussioInversa_imagens(void)
{
	//unsigned char* quadrat;

	//cout << "Fusionando Inversa TOTAL... " << endl;
	int iam = 4;
	//cout << "Número fils: " << iam << endl;
	//cout << "NUMTHRDS: " << NUMTHRDS << endl;
	int in;
	#pragma omp parallel num_threads(4)
	{
		iam = omp_get_thread_num();

		#pragma omp for
		{
			for(int cnt=0; cnt<CNTFusionInversa; cnt=cnt+2 )
			{
				imagen[tabla_calibracionFusionInversa[cnt+1]] = CAM[(int)idCAMARA[cnt]].Get_Vect_Im()[tabla_calibracionFusionInversa[cnt]];
			}
		}	
	}
	cout << endl;


	//char rutaIm[100]="./fusionadaTotal.bmp";
	//Mat dig(Size(2592, 2592), CV_8UC1, imagen);
	//imwrite( rutaIm, dig); 
	
}



/*****************************************************************************
Compensacion puntos Intersección
******************************************************************************/
void sistema_visio::compensacionInterseccion(void)
{

	for(int cnt=0; cnt<cntInterseccion; cnt=cnt+3 )
	{
		int posTEX  = tabla_Interseccion[cnt+0];
		int posIM1  = tabla_Interseccion[cnt+1];
		int posIM2  = tabla_Interseccion[cnt+2];

		int roig = ImagenDisplayVector[posTEX];
		int vert = ImagenDisplayVector[posTEX+1];
		int blau = ImagenDisplayVector[posTEX+2];
		//int alfa = ImagenDisplayVector[posTEX+3];
				
		//int error = (int)(floor(0.9*( 48-quadrat[posIM])));
		int error = (int)floor(0.9*( 48-( (   CAM[0].Get_Vect_Im()[posIM1] + CAM[1].Get_Vect_Im()[posIM2])/2) ));

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


	}

}



/*********************************************************************
 *@ Calcula los pixeles de interseccion y crea una tabla de asignacion
 *@ en función del TEXEL (es decir pierde información de la imagen)
**********************************************************************/
void sistema_visio::calculoPixelesIntersectados (void)
{
	int cnt=0, cntCam=0;
	int tamTextura = (int)floor(WIDTH_PANT)*HEIGHT_PANT*4;

	for(int idxTexel=0; idxTexel<tamTextura; idxTexel=idxTexel+4)
	{
		if((int)detectadaInterseccion[idxTexel]==1)
		{
			tabla_Interseccion[cnt+0] = idxTexel;

			tabla_Interseccion[cnt+1] = CAM[0].getPixelImage(idxTexel);
			CAM[0].crear_TablaInterseccion( idxTexel, CAM[0].getPixelImage(idxTexel), cntCam);		

			tabla_Interseccion[cnt+2] = CAM[1].getPixelImage(idxTexel);
			CAM[1].crear_TablaInterseccion( idxTexel, CAM[1].getPixelImage(idxTexel), cntCam);

			cnt=cnt+3;
			cntCam=cntCam+2;
		}
	}
	cout << "Número de intersecciones: " << cnt  << " Compressio: " << cntCam << endl;

	// Tabla Intersección del sistema de visión
	cntInterseccion=cnt;
	saveTablaInterseccion();

	// Tabla Interseccion individualizada a cada cámara
	CAM[0].save_TablaInterseccion();

	// Tabla Interseccion individualizada a cada cámara
	CAM[1].save_TablaInterseccion();

}




/************************************************
Calibración 
************************************************/
void sistema_visio::calibracioDeFussio(void)
{

	bool unaCaptura = false;
	// Captura las cuatro imágenes
	for(int iSV=0; iSV < num_camaras; iSV++)
		CAM[iSV].CE_Capturar(unaCaptura);		


	if(1) //num_camaras==4)
	{

		cout << "calibracioDeFussio" << endl;
		//Conseguir Puntos de borde de la imagen en coordenadas de dsiplay (texels)
		long int totalPuntos=0;
		Mat nubePuntos(20, 3, DataType<int>::type);

		Mat G;
		int Cx=0, Cy=0, R=0;

		//char a;
		//scanf("%c", &a);
			 
//		for(int iSV=0; iSV < num_camaras; iSV++)
//		{
			//cout << "cannyUmbral" << endl;
			//totalPuntos = CAM[iSV].cannyUmbral(nubePuntos,totalPuntos);
//			totalPuntos = CAM[iSV].detectarCircunferencia(nubePuntos,totalPuntos);
//		}
		
		//Consegir el radio y el centro de la imagen del patron
//		cout << "calcularCentroYRadioCirculo" << endl;
		//cout << endl;
		//cout << nubePuntos << ", " << totalPuntos << endl;
//		calcularCentroYRadioCirculo( nubePuntos, &Cx, &Cy, &R, totalPuntos );

		//Cx=642; Cy=240; R=170;
		//Cx=606; Cy=242.25; R=200;
		//Cx=235; Cy=242.25; 
		R=200;
		//Calcular el escalado y la translación para la nueva imagen
		//Calcular con ellas la matriz de transformación G
		calcularTransformacion ( G, Cx, Cy, R );
		//detectarInterseccionesEnFusionadaX4CAM();
//		detectarInterseccionesEnFusionadaX2CAM();


		//Calular la matriz resultado de G*Pi
		//Calcular la tabla asociada a cada G*Pi			
//		for(int iSV=0; iSV < num_camaras; iSV++)
//		{
//			CAM[iSV].calculo_Tabla_GXP();
//		}

		//calculo_Tabla_GXP_InversaX4CAM();
//		calculo_Tabla_GXP_InversaX2CAM();
	
		for(int iSV=0; iSV < num_camaras; iSV++)
		{
			if ( CAM[iSV].GetId() == idCam_Ppal ||  withMUX) {
				CAM[iSV].calculo_Tabla_Fusionada_Parcial (detectadaInterseccion, centroImagen);
			}
		}
		//saveTablaFusionInversa();

	}

}




/* ****************************************************************************
 detectar intersecciones
******************************************************************************/
void sistema_visio::detectarInterseccionesX2 ( void )
{
	int l=1944;
	cout << "detectarIntersecciones de dos cámaras" << endl;
	/***************************
		CAM 0 
		limite_A, limite_B y limite_C
	****************************/
	// Intersección A
	Point2f texel_12 = CAM[0].PinvX(0,0);
	Point2f texel_21 = CAM[1].PinvX(0,1944);
	Point2f limite_A = (texel_12+texel_21)/2;

	display->posarPunt(texel_12);
	display->posarPunt(texel_21);
	display->posarPunt(limite_A);


	// Intersección B
	texel_12 = CAM[0].PinvX(2592,0);
	texel_21 = CAM[1].PinvX(2592,1944); 
	Point2f limite_B = (texel_12+texel_21)/2;

	display->posarPunt(texel_12);
	display->posarPunt(texel_21);
	display->posarPunt(limite_B);

	centroImagen = (limite_A+limite_B)/2;
	cout << "Centro de la imagen: " << centroImagen << endl;

	// Cálculo de la recta en Cámara 0
	Mat recta1 = Mat(2, 2, CV_64FC1);
	Point2f puntoLinea_A0  = CAM[0].Px(limite_A.x, limite_A.y);
	Point2f puntoLinea_B0  = CAM[0].Px(limite_B.x, limite_B.y);
	CAM[0].obtenerEcuacionDeLaRecta( puntoLinea_A0, puntoLinea_B0, recta1, 0 );	//En Fusionada
	CAM[0].obtenerEcuacionDeLaRecta( limite_A     , limite_B     , recta1, 1 );	//En textura


	// Cálculo de la recta en Cámara 1
	Mat recta2 = Mat(2, 2, CV_64FC1);
	Point2f puntoLinea_A1 = CAM[1].Px(limite_A.x, limite_A.y);
	Point2f puntoLinea_B1 = CAM[1].Px(limite_B.x, limite_B.y);
	CAM[1].obtenerEcuacionDeLaRecta( puntoLinea_A1, puntoLinea_B1, recta2, 0 );	//En Fusionada
	CAM[1].obtenerEcuacionDeLaRecta( limite_A     , limite_B     , recta2, 1 ); 	//En textura

//	display->crearLlinia( recta1.at<double>(1,0), recta1.at<double>(1,1) );

}


/* ****************************************************************************
 detectar intersecciones
******************************************************************************/
void sistema_visio::detectarInterseccionesX4 ( void )
{
	int l=1944;
	cout << "detectarIntersecciones" << endl;
	/***************************
		CAM 0 
		limite_A, limite_B y limite_C
	****************************/
	// Intersección A
	Point2f texel_12 = CAM[0].PinvX(1944,1944);
	Point2f texel_21 = CAM[2].PinvX(0,1944); //(1944,0)

	Point2f limite_A = (texel_12+texel_21)/2;
	Point2f puntoLinea_A = CAM[0].Px(limite_A.x, limite_A.y);
display->posarPunt(texel_12);
display->posarPunt(texel_21);
	display->posarPunt(limite_A);


	// Intersección B
	texel_12 = CAM[0].PinvX(0,0);
	texel_21 = CAM[1].PinvX(0,1944); //(1944,0)

	Point2f limite_B = (texel_12+texel_21)/2;
	Point2f puntoLinea_B = CAM[0].Px(limite_B.x, limite_B.y);
display->posarPunt(texel_12);
display->posarPunt(texel_21);
	display->posarPunt(limite_B);


	// Intersección C  - Centroide de la imagen
	Point2f texel_a = CAM[0].PinvX(1944,0); //(0,1944)
	Point2f texel_b = CAM[1].PinvX(1944,1944);
	Point2f texel_c = CAM[2].PinvX(0,0);
	Point2f texel_d = CAM[3].PinvX(0,1944); //(1944,0)

	Point2f limite_C = (texel_a+texel_b+texel_c+texel_d)/4;
	Point2f puntoLinea_C = CAM[0].Px( limite_C.x, limite_C.y );
	cout << "Centro de las cámaras = " << limite_C << endl;
display->posarPunt(texel_a);
display->posarPunt(texel_b);
display->posarPunt(texel_c);
display->posarPunt(texel_d);
	display->posarPunt(limite_C);

	// Linea AC y Línea BC
	Mat recta1 = Mat(2, 2, CV_64FC1);
	CAM[0].obtenerEcuacionDeLaRecta( puntoLinea_C, puntoLinea_A, recta1, 0 );
	CAM[0].obtenerEcuacionDeLaRecta( puntoLinea_C, puntoLinea_B, recta1, 1 );
	cout << endl;


	/***************************
		CAM 1
		limite_C y limite_D
	****************************/
	// Intersección D
	puntoLinea_C = CAM[1].Px( limite_C.x, limite_C.y );
	texel_12 = CAM[1].PinvX(1944,0); //(0,1944)
	texel_21 = CAM[3].PinvX(0,0);

	Point2f limite_D = (texel_12+texel_21)/2;
	Point2f puntoLinea_D = CAM[1].Px(limite_D.x, limite_D.y);
display->posarPunt(texel_12);
display->posarPunt(texel_21);
	display->posarPunt(limite_D);

	// Linea CD
	Mat recta2 = Mat(2, 2, CV_64FC1);
	CAM[1].obtenerEcuacionDeLaRecta( puntoLinea_D , puntoLinea_C, recta2, 0 );
	// Linea CB
	puntoLinea_B = CAM[1].Px(limite_B.x, limite_B.y);
	CAM[1].obtenerEcuacionDeLaRecta( puntoLinea_B , puntoLinea_C, recta2, 1 );
	cout << endl;


	/***************************
		CAM 2
		limite_C y limite_E
	****************************/
	// Intersección E
	puntoLinea_C = CAM[2].Px( limite_C.x, limite_C.y );

	texel_12 = CAM[2].PinvX(1944,0); //(0,1944)
	texel_21 = CAM[3].PinvX(1944,1944);

	Point2f limite_E = (texel_12+texel_21)/2;
	Point2f puntoLinea_E = CAM[2].Px(limite_E.x, limite_E.y);
display->posarPunt(texel_12);
display->posarPunt(texel_21);
	display->posarPunt(limite_E);

	// Linea CE
	Mat recta3 = Mat(2, 2, CV_64FC1);
	CAM[2].obtenerEcuacionDeLaRecta( puntoLinea_E , puntoLinea_C, recta3, 0 );
	// Linea CA
	puntoLinea_A = CAM[2].Px(limite_A.x, limite_A.y);
	CAM[2].obtenerEcuacionDeLaRecta( puntoLinea_A , puntoLinea_C, recta3, 1 );


	/***************************
		CAM 3
		limite_C y limite_E
	****************************/
	puntoLinea_C = CAM[3].Px( limite_C.x, limite_C.y );
	puntoLinea_E = CAM[3].Px( limite_E.x, limite_E.y);
	puntoLinea_D = CAM[3].Px(limite_D.x, limite_D.y);

	Mat recta4 = Mat(2, 2, CV_64FC1);	
	// Linea CE
	CAM[3].obtenerEcuacionDeLaRecta( puntoLinea_E , puntoLinea_C, recta4, 0 );
	// Linea CD
	CAM[3].obtenerEcuacionDeLaRecta( puntoLinea_D , puntoLinea_C, recta4, 1 );

}


/* 
detectar intersecciones

*/
void sistema_visio::detectarInterseccionesEnFusionadaX2CAM( void )
{
	cout << "detectarIntersecciones en fusionada 2 Cámaras" << endl;
	/****************************************
		CAM 0 
		limite_A, limite_B y limite_C
	*****************************************/
	// Intersección A
	Point2f texel_12 = CAM[0].GxPx(0,0);
	Point2f texel_21 = CAM[1].GxPx(0,1944);
	Point2f limite_A = (texel_12+texel_21)/2;


	// Intersección B
	texel_12 = CAM[0].GxPx(2592,0);
	texel_21 = CAM[1].GxPx(2592,1944);
	Point2f limite_B = (texel_12+texel_21)/2;

	// Linea AB
	Mat recta1 = Mat(1, 2, CV_64FC1);
	CAM[0].obtenerEcuacionDeLaRectaFusionada( limite_A, limite_B, recta1, 0 );

	// Linea AB
	Mat recta2 = Mat(1, 2, CV_64FC1);
	CAM[1].obtenerEcuacionDeLaRectaFusionada( limite_A, limite_B, recta2, 0 );

}



/* 
detectar intersecciones

*/
void sistema_visio::detectarInterseccionesEnFusionadaX4CAM( void )
{
	cout << "detectarIntersecciones en fusionada" << endl;
	/***************************
		CAM 0 
		limite_A, limite_B y limite_C
	****************************/
	// Intersección A
	Point2f texel_12 = CAM[0].GxPx(1944,1944);
	Point2f texel_21 = CAM[2].GxPx(0,1944);
	Point2f limite_A = (texel_12+texel_21)/2;


	// Intersección B
	texel_12 = CAM[0].GxPx(0,0);
	texel_21 = CAM[1].GxPx(0,1944);
	Point2f limite_B = (texel_12+texel_21)/2;


	// Intersección C  - Centroide de la imagen
	Point2f texel_a = CAM[0].GxPx(1944,0);
	Point2f texel_b = CAM[1].GxPx(1944,1944);
	Point2f texel_c = CAM[2].GxPx(0,0);
	Point2f texel_d = CAM[3].GxPx(0,1944);

	Point2f limite_C = (texel_a+texel_b+texel_c+texel_d)/4;
	cout << "Centro de las cámaras = " << limite_C << endl;


	// Linea AC y Línea BC
	Mat recta1 = Mat(2, 2, CV_64FC1);
	CAM[0].obtenerEcuacionDeLaRectaFusionada( limite_C, limite_A, recta1, 0 );
	CAM[0].obtenerEcuacionDeLaRectaFusionada( limite_C, limite_B, recta1, 1 );


	/***************************
		CAM 1
		limite_C y limite_D
	****************************/
	// Intersección D
	texel_12 = CAM[1].GxPx(1944,0);
	texel_21 = CAM[3].GxPx(0,0);
	Point2f limite_D = (texel_12+texel_21)/2;

	// Linea CD
	Mat recta2 = Mat(2, 2, CV_64FC1);
	CAM[1].obtenerEcuacionDeLaRectaFusionada( limite_D, limite_C, recta2, 0 );
	// Linea BC
	CAM[1].obtenerEcuacionDeLaRectaFusionada( limite_B, limite_C, recta2, 1 );  //-----



	/***************************
		CAM 2
		limite_C y limite_E
	****************************/
	// Intersección E
	texel_12 = CAM[2].GxPx(1944,0);
	texel_21 = CAM[3].GxPx(1944,1944);
	Point2f limite_E = (texel_12+texel_21)/2;

	// Linea CE
	Mat recta3 = Mat(2, 2, CV_64FC1);
	CAM[2].obtenerEcuacionDeLaRectaFusionada( limite_E, limite_C, recta3, 0 );
	CAM[2].obtenerEcuacionDeLaRectaFusionada( limite_A, limite_C, recta3, 1 );


	/***************************
		CAM 3
		limite_C y limite_E
	****************************/

	// Linea CE
	Mat recta4 = Mat(2, 2, CV_64FC1);
	CAM[3].obtenerEcuacionDeLaRectaFusionada( limite_E, limite_C, recta4, 0 );
	CAM[3].obtenerEcuacionDeLaRectaFusionada( limite_D, limite_C, recta4, 1 );


}




/************************************************************************
Calculo tabla de Fusion 2 CAM
*************************************************************************/
int sistema_visio::calculo_Tabla_GXP_InversaX2CAM (void)
{

	int AnchoOrigen = WIDTH_FRAME;
	int AltoOrigen  = HEIGHT_FRAME;

	int AnchoFusion = 2592;
	int AltoFusion  = 2592;

	cout << "Creando tabla Inversa TOTAL para 2 cámaras... " << endl;
	

	double limiteEnY1=0;
	long int cnt = 0;
	long int posIM, posFus;
	Point2f puntImage;

	double m1, b1;

	for(int id=0; id<num_camaras; id++)
	{

		m1=CAM[id].getM(0);
		b1=CAM[id].getB(0);


		for(int FusionadaX=0; FusionadaX<AnchoFusion; FusionadaX++ )
		{
			for(int FusionadaY=0; FusionadaY<AltoFusion; FusionadaY++ )
			{
				puntImage = CAM[id].Inv_GxPx((double)FusionadaX,(double)FusionadaY);

				if(puntImage.x>=0 && puntImage.x<AnchoOrigen && puntImage.y>=0 && puntImage.y<AltoOrigen)
				{

					posFus = FusionadaY*(AnchoFusion)  + FusionadaX;

					if( id==0 )
					{
						limiteEnY1 = m1 * puntImage.x + b1;

						if( puntImage.y>limiteEnY1 && !evaluado[posFus]) 		//!evaluado[posFus] && puntImage.y<limiteEnY1 )
						{
							posIM  = puntImage.y*AnchoOrigen + puntImage.x; 

							tabla_calibracionFusionInversa[cnt]   =  posIM;
							tabla_calibracionFusionInversa[cnt+1] =  posFus;
							idCAMARA[cnt] = id;
							evaluado[posFus]=1;
							cnt=cnt+2;

						}
							
					}
					if(id==1)
					{
						if( !evaluado[posFus]) 	 //puntImage.y<limiteEnY1 && 	//!evaluado[posFus] && puntImage.y>limiteEnY1 )
						{
							posIM  = puntImage.y*AnchoOrigen + puntImage.x; 

							tabla_calibracionFusionInversa[cnt]   =  posIM;
							tabla_calibracionFusionInversa[cnt+1] =  posFus;
							idCAMARA[cnt] = id;
							evaluado[posFus]=1;
							cnt=cnt+2;

						}
							
					}
								

				}
		
			}
		}
		cout << "pixelesConvert: " << cnt << endl;

	}

	CNTFusionInversa = cnt;

}



/************************************************************************
Calculo tabla de Fusion
*************************************************************************/
int sistema_visio::calculo_Tabla_GXP_InversaX4CAM ( void )
{

	int AnchoOrigen = WIDTH_FRAME;
	int AltoOrigen  = HEIGHT_FRAME;

	int AnchoFusion = WIDTH_FRAME*2;
	int AltoFusion  = HEIGHT_FRAME*2;

	cout << "Creando tabla Inversa... " << endl;
	

	double limiteEnY1=0,limiteEnY2=0;
	long long int cnt = 0;
	long int posIM, posFus;
	Point2f puntImage;

	double m1, m2, b1, b2;


	for(int id=0; id<num_camaras; id++)
	{

		m1=CAM[id].getM(0);
		b1=CAM[id].getB(0);

		m2=CAM[id].getM(1);
		b2=CAM[id].getB(1);

		for(int FusionadaX=0; FusionadaX<AnchoFusion; FusionadaX++ )
		{
			for(int FusionadaY=0; FusionadaY<AltoFusion; FusionadaY++ )
			{
				puntImage = CAM[id].Inv_GxPx((double)FusionadaX,(double)FusionadaY);

				if(puntImage.x>=0 && puntImage.x<1944 && puntImage.y>=0 && puntImage.y<1944)
				{

					limiteEnY1 = m1 * puntImage.x + b1;
					limiteEnY2 = m2 * puntImage.x + b2;

					posFus = FusionadaY*(1944*2)  + FusionadaX;

					if( id==0 || id==2 )
					{
						if(!evaluado[posFus] && puntImage.y>limiteEnY1 && puntImage.y>limiteEnY2 )
						{
							posIM  = puntImage.y*1944 + puntImage.x; 

							tabla_calibracionFusionInversa[cnt]   =  posIM;
							tabla_calibracionFusionInversa[cnt+1] =  posFus;
							idCAMARA[cnt] = id;
							evaluado[posFus]=1;
							cnt=cnt+2;

						}
							
					}
					if(id==1)
					{
						if(!evaluado[posFus] && puntImage.y>limiteEnY1 && puntImage.y<limiteEnY2 )
						{
							posIM  = puntImage.y*1944 + puntImage.x; 

							tabla_calibracionFusionInversa[cnt]   =  posIM;
							tabla_calibracionFusionInversa[cnt+1] =  posFus;
							idCAMARA[cnt] = id;
							evaluado[posFus]=1;
							cnt=cnt+2;

						}
							
					}
					if(id==3)
					{
						if(!evaluado[posFus] && puntImage.y<limiteEnY1 && puntImage.y<limiteEnY2 )
						{
							posIM  = puntImage.y*1944 + puntImage.x; 

							tabla_calibracionFusionInversa[cnt]   =  posIM;
							tabla_calibracionFusionInversa[cnt+1] =  posFus;
							idCAMARA[cnt] = id;
							evaluado[posFus]=1;
							cnt=cnt+2;

						}
							
					}				

				}
		
			}
		}
		cout << "pixelesConvert: " << cnt << endl;
	}

	CNTFusionInversa = cnt;
	cout << cnt << endl;
}


/**********************************************************************************************************************
Calcular transformación del display a la imagen compuesta
**********************************************************************************************************************/
void sistema_visio::calcularTransformacion ( Mat& G, int Cx, int Cy, int radioImTextura )
{
	cout << "calcularTransformacion" << endl;
	double anchoImagenCompuesta = 2592; //WIDTH_FRAME
	double altoImagenCompuesta  = 2592; //HEIGHT_FRAME

	double radioImCompuesta = altoImagenCompuesta/2.0;

	double S = (double)radioImTextura/radioImCompuesta;

	double dx =   2592/2*S + (double)centroImagen.x;
	double dy =   2592/2*S - (double)centroImagen.y;

	G = Mat::zeros( 3, 3, CV_64FC1);

	// Matriz G
	G.at<double>(0,0)=-1;				G.at<double>(0,2)= dx;
				G.at<double>(1,1)= 1;	G.at<double>(1,2)= dy;
							G.at<double>(2,2)= S;
	

	for(int id=0; id<num_camaras; id++)
		CAM[id].setMatGxP(G);
	

}


/**********************************************************************************************************************
Detectar circunferencia
**********************************************************************************************************************/
void sistema_visio::calcularCentroYRadioCirculo ( Mat& nubePuntos, int *Cx, int *Cy, int *R,  long int totalPuntos )
{

	double   mediaRadio=0;
	double   mediaX=0, mediaY=0, mediar=0;
	long int xSuma=0, ySuma=0, rSuma=0;


	if(totalPuntos>0)
	{
		for( int cnt=0; cnt<totalPuntos; cnt++ )
		{
			xSuma += nubePuntos.at<int>(cnt,0); //(*nubePuntos)[cnt][0];
			ySuma += nubePuntos.at<int>(cnt,1);  //(*nubePuntos)[cnt][1];
			rSuma += nubePuntos.at<int>(cnt,2);
		}

		mediaX = (double)(xSuma/totalPuntos);
		mediaY = (double)(ySuma/totalPuntos);
		mediar = (double)(rSuma/totalPuntos);

		/*
		for( int cnt=0; cnt<totalPuntos; cnt++ )
		{
			double x = nubePuntos.at<int>(cnt,0); //(*nubePuntos)[cnt][0];
			double y = nubePuntos.at<int>(cnt,1);  //(*nubePuntos)[cnt][1];

			mediaRadio += sqrt ( pow( mediaX-x, 2 ) + pow( mediaY-y, 2 ) );
		}*/

		mediaRadio = mediar;
		//mediaRadio = mediaRadio/totalPuntos;

				/**********************************
				Dibuixa
				***********************************/
				int anchoPant = (int)floor(WIDTH_PANT);
				for( int cnti=(mediaX-5); cnti<(mediaX+5); cnti++ )
					for( int cntj=(mediaY-5); cntj<(mediaY+5); cntj++ )
					{
						long int posTEX  = cntj*anchoPant + cnti;

						ImagenDisplayVector[posTEX*4]  =   0;
						ImagenDisplayVector[posTEX*4+1]=   0;
						ImagenDisplayVector[posTEX*4+2]= 255;
						ImagenDisplayVector[posTEX*4+3]= 255;
					}
				/*********************************************/

		*Cx=mediaX;
		*Cy=mediaY;
		*R =(int)round(mediaRadio);
		cout << "Cx: " << mediaX << ", Cy: " << mediaY << ", R: " << mediaRadio << endl;
	}
}


/*
*
* Si el Sistema de Vision es Master (Es decir controla diversas cámaras)
* entonces crea un servidor TCP/IP para poder recibir las Matrices de Calibración
*
*/
void* crear_Servidor_SistemaVison_Master( void* context )
{
	sistema_visio* SV = (sistema_visio*) context;

	int host_port= 2000;
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

	my_addr.sin_port = htons(2000);
	my_addr.sin_addr.s_addr = inet_addr(IP_SV_Ppal);
	my_addr.sin_family = AF_INET;
	bzero(&(my_addr.sin_zero),8);
	
	    
	if( bind( hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		//fprintf(stderr, "Error con el servidor RPI, PORT %d \n", errno); // PORT %d \n", errno);
		printf("Error con Sistema Vision Principal: (%s , %d) \n", inet_ntoa(my_addr.sin_addr),ntohs(my_addr.sin_port));
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",errno);
	}

	addr_size = sizeof(sockaddr_in);

	int iter=0;
	struct argumentos argumento[8];
    
	while(true)
	{
		//printf("Esperant una conexio...\n");
		csock = (int*)malloc(sizeof(int));
		if(true) //num_conexions!=2)
		{
			if((*csock = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1)
			{
				argumento[iter].SV = SV;
				argumento[iter].scock = csock;
				cout << "\033[1;32mConexión establecida Sistema Vision Principal\033[0m" << endl;
				//printf("-> Conexión establecida Sistema Vision Principal %s \n", inet_ntoa(my_addr.sin_addr));
				pthread_create(&thread_id,0,gestionComunicacion_SistemaVision, (void*)&argumento[iter] );
				pthread_detach(thread_id);
			}
		}
		else
		{
			fprintf(stderr, "Error accepting %d\n", errno);
		}

	}
}


/*
// Cabecera
// Recibir Matriz

void sistema_visio::
	sistemaVisioMaster
	// Si el identificador de la cámara es 1, este sistema de Visión será el principal,
	// es decir será el máster de las cámaras. Se creará un hilo de conexión tipo servidor para
	// recibir las calibraciones
	if(idCam_Ppal==1 && !withMUX) {
		// Crear Servidor ¿IP?
		// Cabecera
		// Recibir Matriz
		CAM[i].recibirMatrizCalibracion();
		// Espera 1s
	}
*/
void* gestionComunicacion_SistemaVision ( void* context )
{
	struct argumentos* argumento = (struct argumentos*)context;

	int *csock1 = (int*)argumento->scock;
	sistema_visio* SV = argumento->SV;
	int sock = *csock1;
	int idCamReb, idSVReb, tipoDato=0;

	while(1)
	{

		SV->cabeceraSincronizacion(sock, &idCamReb, &idSVReb, &tipoDato);

		// Si la conexio es en una camara remota (l'ha de rebre)
		if(idCamReb != idCam_Ppal && tipoDato==1)  // <<<<<<<
		{
			SV->recibirMatrizCalibracion (sock, idCamReb, idSVReb);
		}

		// Si la conexio es en la camara local (l'ha d'enviar)
		if(idCamReb == idCam_Ppal && tipoDato==1)   // <<<<<<<
		{
			SV->enviarMatrizCalibracion (sock, idCamReb, idSVReb);
		}
	
		// Rebre Tabla Interseccio Parcial (de Camara Secundaria)
		if (idCamReb != idCam_Ppal && tipoDato==2) 
		{
			SV->recibirTablaInterseccionParcial(sock, idCamReb, idSVReb);
		}
		
		// Rebre Image Interseccio Parcial (de Camara Secundaria)
		if (idCamReb != idCam_Ppal && tipoDato==3)
		{
			SV->recibirInterseccionParcial(sock, idCamReb, idSVReb);
		}

	}
}


void sistema_visio::cabeceraSincronizacion(int sock, int* idCamReb, int* idSVReb, int* tipoDato)
{
	int tam = 15;
	char cabecera[tam]="";
	string tamanoInterseccio="0000000";
	int rebut=0;
	int pos=0;

	rebut = recv(sock, &cabecera, tam, 0);

	*idCamReb = (int)cabecera[pos];
	pos++;
	*idSVReb  = (int)cabecera[pos];
	pos++;
	*tipoDato = (int)cabecera[pos];
	pos++;

	// Tamany Interseccio
	for(int it=0; it<6; it++) {
		tamanoInterseccio[it] = cabecera[pos];
		pos++;
	}
	tamanoInterseccio[6]='\0';
	int tamInterseccionParcial = (int)atoi(tamanoInterseccio.c_str());
	

	int iD = buscar( *idCamReb, *idSVReb );
	CAM[iD].setTamInterseccionParcial(tamInterseccionParcial);
}

/*
* @ Recibir la MATRIZ de calibración de la cámara Remota
*
*/
void sistema_visio::recibirMatrizCalibracion(int sock, int idCamReb, int idSVReb)
{
	cout << "Entra en función" << endl;
	//Mat matriu;
	int m=3, n=3;

	int idC, IDSV;
	int iD = buscar( idCamReb, idSVReb );
	CAM[iD].recibirMatrizCalibracion (sock, CAM[iD].get_dHi(), m, n);
	cout << "Recibiendo Matriz " << idCamReb << "... " << endl << *CAM[iD].get_dHi() << endl << endl;

	// Cuando recibe la Matriz, esta calibra
	CAM[iD].matrizCalibracionRecibida(true);
}

void sistema_visio::enviarMatrizCalibracion(int sock, int idCamReb, int idSVReb)
{
	int m=3, n=3;

	int idC, IDSV;
	int iD = buscar( idCamReb, idSVReb );

	while ( !CAM[iD].isCalibrada() && CAM[iD].GetId() == idCam_Ppal  ) usleep(100000);

	CAM[iD].enviarMatrizCalibracion (sock, CAM[iD].get_dHi(), m, n);
	cout << "Enviando Matriz " << idCamReb << "... " << endl << *CAM[iD].get_dHi() << endl << endl;

	// Cuando envia la Matriz, no la vuelve a enviar
	CAM[iD].matrizCalibracionEnviada(true);
}


/*
* @ Recibir la Imagen INTERSECTADA
*
*/
void sistema_visio::recibirTablaInterseccionParcial (int sock, int idCamReb, int idSVReb)
{
	int idC, IDSV;
	int iD = buscar( idCamReb, idSVReb );
	CAM[iD].recibirTablaInterseccionParcial (sock);

}


void sistema_visio::recibirInterseccionParcial (int sock, int idCamReb, int idSVReb)
{
	int idC, IDSV;
	int iD = buscar( idCamReb, idSVReb );
	CAM[iD].recibirInterseccionParcial (sock);

}


/*
*
* Buscar idCam
*
*/
int sistema_visio::buscar( int idC, int IDSV )
{
	int idx = -1;

	for( int ind=0; ind<num_camaras ; ind++)
	{
		int idCam	= CAM[ind].GetId();
		int idSisVis	= idSV;
		
		if( idCam==idC && idSV==IDSV ) {
			//cout << "Indice: " << ind << ", idCam: " << idCam << ", idSisVis: " << idSisVis << endl; 
			return ind;
		}
	}

	return idx;

}




/************************************************
Crear la ruta de la tabla de calibracion FUSION
************************************************/
void sistema_visio::crear_ruta_tablaCalibracionFusion (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_tablaCalibracionFusion, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_tablaCalibracionFusion, sistVis.c_str() );

	strcat( ruta_tablaCalibracionFusion,".cnf");

	//cout  << ruta_tablaCalibracionFusion << endl;

}


/************************************************
Crear la ruta de la tabla de calibracion FUSION
************************************************/
void sistema_visio::crear_ruta_idCAMARA (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_idCAMARA, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_idCAMARA, sistVis.c_str() );

	strcat( ruta_idCAMARA,".cnf");

	//cout  << ruta_idCAMARA << endl;

}


/************************************************
Crear la ruta de la tabla de calibracion FUSION
************************************************/
void sistema_visio::crear_ruta_tablaInterseccion (void)
{
	string conf = int2string(config); //num_camaras
	strcat( ruta_tablaInterseccion, conf.c_str() );

	string sistVis = int2string(idSV);  //idSV
	strcat( ruta_tablaInterseccion, sistVis.c_str() );

	strcat( ruta_tablaInterseccion,".cnf");

	//cout  << ruta_tablaInterseccion << endl;

}



/********************************************************************************
Cargar tabla de calibración FUSION
*********************************************************************************/
bool sistema_visio::loadTablaFusionInversa(void)
{
	// cargar la tabla 
	FILE * pFile;
	long int lSize;
		
	size_t result;

	if((pFile = fopen ( ruta_tablaCalibracionFusion , "rb" ))!=NULL) //dir_rep
	{
		//cout << "Cargando Tabla Calibracion..." << ruta_tablaCalibracionFusion << endl;

		if (pFile==NULL) {fputs ("File error",stderr); }

		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		if (tabla_calibracionFusionInversa == NULL) {fputs ("Memory error",stderr); }

		result = fread (tabla_calibracionFusionInversa, sizeof(int), lSize, pFile);
		//if (result != lSize) {fputs ("Reading error",stderr); }
	
	}
	else
	{
		cout << "No hay archivo" << endl;
		return false;
	}
	free (pFile);

	//cout << "Tabla Inversa cargada: " << lSize << " Bytes" << endl;


	// Cargar la tabla idCAMARA
	memset ( idCAMARA, 0, sizeof(char) * WIDTH_FRAME * HEIGHT_FRAME * num_camaras * 2);
	FILE * pFile2;
	long int lSize2;
		
	size_t result2;

	if((pFile2 = fopen ( ruta_idCAMARA , "rb" ))!=NULL) //dir_rep
	{
		//cout << "Cargando Tabla Calibracion..." << ruta_idCAMARA << endl;

		if (pFile2==NULL) {fputs ("File error",stderr); }

		fseek (pFile2 , 0 , SEEK_END);
		lSize2 = ftell (pFile2);
		rewind (pFile2);

		if (idCAMARA == NULL) {fputs ("Memory error",stderr); }

		result2 = fread (idCAMARA, sizeof(char), lSize2, pFile2);
		//if (result2 != lSize2) {fputs ("Reading error",stderr); }
	
	}
	else
	{
		cout << "No hay archivo" << endl;
		return false;
	}
	free (pFile2);
	//cout << "Tabla Inversa cargada: " << lSize2 << " Bytes" << endl;
	CNTFusionInversa = (long int)lSize2;

	return true;

}


/********************************************************************************
Guardar tabla de calibración FUSION
*********************************************************************************/
bool sistema_visio::saveTablaFusionInversa (void)
{
	//cout << "Guardando Tabla Calibracion..." << ruta_tablaCalibracionFusion << endl;

	FILE *f = fopen(ruta_tablaCalibracionFusion,"w");

	if (f == 0) 
		printf("*No he pogut obrir %s\n", ruta_tablaCalibracionFusion);
	else
	{
		fwrite(tabla_calibracionFusionInversa,sizeof(int),CNTFusionInversa,f);   //fwrite(image_reb,1,tam_reb,f);
		fclose(f);
	}


	FILE *f2 = fopen( ruta_idCAMARA,"w");

	if (f2 == 0) 
		printf("*No he pogut obrir %s\n", ruta_idCAMARA );
	else
	{
		fwrite(idCAMARA, sizeof(char), CNTFusionInversa, f2);   //fwrite(image_reb,1,tam_reb,f);
		fclose(f2);
	}

	
}


/********************************************************************************
Cargar tabla de calibración FUSION
*********************************************************************************/
bool sistema_visio::loadTablaInterseccion(void)
{
	// cargar la tabla 
	FILE * pFile;
	long int lSize;
		
	size_t result;

	if((pFile = fopen ( ruta_tablaInterseccion , "rb" ))!=NULL) //dir_rep
	{
		//cout << "Cargando Tabla Calibracion..." << ruta_tablaInterseccion << endl;

		if (pFile==NULL) {fputs ("File error",stderr); }

		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		if (tabla_Interseccion == NULL) {fputs ("Memory error",stderr); }

		result = fread (tabla_Interseccion, sizeof(int), lSize, pFile);
		//if (result != lSize) {fputs ("Reading error",stderr); }
	
	}
	else
	{
		cout << "No hay archivo" << endl;
		return false;
	}
	free (pFile);

	//cout << "Tabla Intersecciones cargada: " << lSize << " Bytes" << endl;
	cntInterseccion = lSize/sizeof(int);

	return true;
}


/********************************************************************************
Guardar tabla de Intersecion
*********************************************************************************/
bool sistema_visio::saveTablaInterseccion (void)
{
	//cout << "Guardando Tabla Intersección..." << ruta_tablaInterseccion << endl;

	FILE *f = fopen(ruta_tablaInterseccion,"w");

	if (f == 0) 
		printf("*No he pogut obrir %s\n", ruta_tablaInterseccion);
	else
	{
		fwrite( tabla_Interseccion, sizeof(int), cntInterseccion , f);
		fclose(f);
	}
	
}


/************************************************
Guarda la Imagen
************************************************/
void* CE_Guardar ( void* SistVis )
{
	sistema_visio *SV = (sistema_visio*) SistVis;
	bufferCircular *Buffer = SV->getBuffer();

	unsigned char* imageAgravar = (unsigned char*) malloc ( sizeof(char)* WIDTH_FRAME * HEIGHT_FRAME* 2);

	SV->Init_It_Imagens();		// Inicia el conteig del numero d'imagens a 0

	while(1)
	{
		do
		{
		    usleep(1000);
		}while(Buffer->checkAvail()<=0);

		cout << "En Cola: " << Buffer->checkAvail() << endl;

		struct timespec t1 = Inicia_Temporisador();
		char rutaGrabacion[150]="";
		int tamTotal;
		Buffer->remImage( imageAgravar, rutaGrabacion, tamTotal);
		cout << "Guardada: " << rutaGrabacion << ", Tam: " << tamTotal << endl;
		
		FILE *f = fopen(rutaGrabacion,"w");
		if (f == 0)
			printf("No he pogut obrir %s\n", rutaGrabacion);
		else
		{
			fwrite( imageAgravar,1,tamTotal,f);
			fclose(f);
		}
		//Mat dig(Size(2592, 2592), CV_8UC1, imageAgravar);
		//imwrite( rutaGrabacion, dig);
		cout << "Temps guardar: "<< Medix_temps(t1) << endl;
	}
	free(imageAgravar);

}




/************************************************
Interficie de Audio
************************************************/
void* Reproduir_Audio (void*)
{
/*	char ruta_audio[70];
	while(1)
	{
		//while(!parla) usleep(100000);
		while(1) usleep(100000);

		//parla=false;
		//if(calibrado1==true) strcpy(ruta_audio, "./recursos/cal1.wav" );
		//if(calibrado2==true) strcpy(ruta_audio, "./recursos/cal1.wav" );

		sf::SoundBuffer buffer;
		if(buffer.loadFromFile(ruta_audio))
		{
			sf::Sound sound;
			sound.setBuffer(buffer);
			sound.play();
			sleep(5);
		}	
	}*/
}


/************************************************
Detector de pendrive USB
************************************************/
//  Lee los archivos del directorio
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

// Consigue los hijos
static struct udev_device*
    get_child(struct udev* udev, struct udev_device* parent, const char* subsystem)
{
    struct udev_device* child = NULL;
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_parent(enumerate, parent);
    udev_enumerate_add_match_subsystem(enumerate, subsystem);
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices)
    {
        const char *path = udev_list_entry_get_name(entry);
        child = udev_device_new_from_syspath(udev, path);
        break;
    }

    udev_enumerate_unref(enumerate);
    return child;
}

static void enumerate_usb_mass_storage(struct udev* udev)
{
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_subsystem(enumerate, "scsi");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices)
    {
        const char* path = udev_list_entry_get_name(entry);
        struct udev_device* scsi = udev_device_new_from_syspath(udev, path);

        struct udev_device* block = get_child(udev, scsi, "block");
        struct udev_device* scsi_disk = get_child(udev, scsi, "scsi_disk");

        struct udev_device* usb
            = udev_device_get_parent_with_subsystem_devtype(scsi, "usb", "usb_device");

        if (block && scsi_disk && usb)
        {
            Num_pendrives = Num_pendrives+2;
            string dir = string("/media");
            vector<string> files = vector<string>();

            getdir(dir,files);

            for (unsigned int i = 0; i < files.size(); i++)
                if( files[i].compare( "SETTINGS")!=0 && files[i].compare( ".")!=0 && files[i].compare( "..")!=0 )
                    directorio_pendrive = "/media/" +  files[i] + "/" + '\0';

            strcpy(DIR_PEN, "");
            int kt = directorio_pendrive.size();
            for(int k = 0; k < kt ; k++) DIR_PEN[k]=directorio_pendrive[k];
            directorio_pendrive[kt]='\0';
            //DIR_PEN[kt]='\0';
            cout << DIR_PEN << endl;

        }

        if (block) {
            udev_device_unref(block);
        }

        if (scsi_disk) {
            udev_device_unref(scsi_disk);
        }
        udev_device_unref(scsi);
    }
    udev_enumerate_unref(enumerate);
}

void*  USB_thread ( void* )
{
    struct udev* udev = udev_new();
    enumerate_usb_mass_storage(udev);

    Num_pen = Num_pendrives/2;
    cout <<  "Pendrives conectados..." << Num_pen << endl;
    cout << directorio_pendrive << endl;

    struct udev_device *dev;
    struct udev_monitor *mon;
    int fd;

    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
    udev_monitor_enable_receiving(mon);
    fd = udev_monitor_get_fd(mon);

    while (1)
    {
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        // Check if our file descriptor has received data.
        if (ret > 0 && FD_ISSET(fd, &fds))
        {
            dev = udev_monitor_receive_device(mon);
            cout << endl;

            if (dev)
            {
                // Conexion de dispositivo pendrive
                if( strcmp(udev_device_get_action(dev),"add")==0 )
                {
                    Num_pendrives++;
                    cout << "Nuevo dispositivo." << endl;
                    string dir = string("/media");
                    vector<string> files = vector<string>();

                    //usleep(1000);
                    getdir(dir,files);

                    for (unsigned int i = 0; i < files.size(); i++)
                        if( files[i].compare( "SETTINGS")!=0 && files[i].compare( ".")!=0 && files[i].compare( "..")!=0 )
                            directorio_pendrive = "/media/" +  files[i] + "/" + '\0';

                    strcpy(DIR_PEN, "");
                    int kt = directorio_pendrive.size();
                    for(int k = 0; k < kt ; k++) DIR_PEN[k]=directorio_pendrive[k];
                    cout << DIR_PEN << endl;
                    files.clear();
                }

                // Desconexion de dispositivo pendrive
                if( strcmp(udev_device_get_action(dev),"remove")==0 )
                {
                    Num_pendrives--;
                    cout << "Dispositivo desconectado."  << endl;
                }
                udev_device_unref(dev);
            }
            else printf("No Device from receive_device(). An error occured.\n");

            Num_pen = Num_pendrives/2;
            cout <<  "Pendrives conectados..." << Num_pen << endl;
        }
        usleep(250*1000);
        fflush(stdout);
    }

    udev_unref(udev);
    return 0;
}



/*
* Cuenta las veces que ha arrancado la RASPBERRY PI
*/
int contadorCiclos( void )
{
	int CICLES;
	if(!existe("./analisis/cicles.xml")){
		FileStorage fs("./analisis/cicles.xml", FileStorage::WRITE);
		fs << "CICLES" << 0;
	}
	{
		FileStorage fs( "./analisis/cicles.xml", FileStorage::READ); 
		fs["CICLES"] >> CICLES;
		CICLES++;
	}
	{
		FileStorage fs("./analisis/cicles.xml", FileStorage::WRITE);
		fs << "CICLES" << CICLES;
	}

	return CICLES;
}


void cargarConfiguracio ( void )
{

	if(!existe("./configuracio.xml"))
	{
		cout << "NO EXISTE ARCHIVO DE CONFIGURACIÓN PRINCIPAL" << endl;/*
		FileStorage fs("./configuracio.xml", FileStorage::WRITE);
		fs << "esMaster"		<< Master;
		fs << "IP_Servidor"		<< IP_Servidor;
		fs << "IP_Master"		<< IP_Master;
		fs << "id_Raspberry"		<< id_Raspberry;
		fs << "Cantitat_Sistemes_Visio"	<< Cantitat_Sistemes_Visio;
		fs << "Cantitat_Camares"	<< Cantitat_Camares;
		fs << "idCam_Ppal"		<< idCam_Ppal;
		fs << "SistemaVisio_Ppal"	<< SistemaVisio_Ppal;
		fs << "IP_SV_Ppal"		<< IP_SV_Ppal;
		*/
	}

	bool SistemaVisio_Ppal=false;
	string IPSer, IPMast, IPPpal;
	FileStorage fs("./configuracio.xml", FileStorage::READ);
	fs ["esMaster"]			>> esMaster;
	fs ["withMUX"]			>> withMUX;
	fs ["IP_Servidor"]		>> IPSer;
	fs ["IP_Master"]		>> IPMast;
	fs ["id_Raspberry"]		>> id_Raspberry;
	fs ["id_Sistema_visio"]		>> id_Sistema_visio;
	fs ["Cantitat_Sistemes_Visio"]	>> Cantitat_Sistemes_Visio;
	fs ["Cantitat_Camares"]		>> Cantitat_Camares;
	fs ["idCam_Ppal"]		>> idCam_Ppal;
	fs ["SistemaVisio_Ppal"]	>> SistemaVisio_Ppal;
	fs ["IP_SV_Ppal"]		>> IPPpal;
	fs ["WIDTH_FRAME"]		>> WIDTH_FRAME;
	fs ["HEIGHT_FRAME"]		>> HEIGHT_FRAME;
	fs ["config"]			>> config;

	strcpy(IP_Servidor , IPSer.c_str());
	strcpy(IP_Master   , IPMast.c_str());
	strcpy(IP_SV_Ppal  , IPPpal.c_str());


	// Si el identificador de la cámara es 1, este sistema de Visión será el principal,
	// es decir será el que gestione las cámaras de este mismo Sistema de Visión.
	if( SistemaVisio_Ppal==true && !withMUX) {
		SV_Principal  = true;
	}
	else if ( SistemaVisio_Ppal==false && !withMUX ) {
		SV_Secundario = true;
	}

}




