#pragma once
#include "llibreries/llibreries.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

using namespace cv;
using namespace std;


CCamera* cam;

pthread_mutex_t mutexEnviament = PTHREAD_MUTEX_INITIALIZER;
bool esMaster;
bool withMUX;
char IP_Servidor[15]="";
char IP_Master[15]="";
char IP_SV_Ppal[15]="";
//	int  Port_IP;
int  id_Raspberry;
int  id_Sistema_visio;
int  Cantitat_Sistemes_Visio;
int  Cantitat_Camares;
int  idCam_Ppal;
int  WIDTH_FRAME;
int  HEIGHT_FRAME;
int  config;

bool SV_Principal =false;
bool SV_Secundario=false;

int NUM_DISPLAYS=0;
int ch;
bool recompensa=false;
bool compensando=false;

bool G_Local=false;
bool abortar=false;
bool apagar_RPI=false;
float umbral=80;

int Num_pendrives=0;
int Num_pen=0;
string directorio_pendrive;
char DIR_PEN[60]="";

int fname=0;
float temperatura=0;


char nom_proyecte[100]="";
char direccio[150]="/home/pi/GRAVAR/";
char sub_dir[150]="";
char NOM[15]="";

class objetos;


int main(int argc, const char *argv[])
{
	char modoCali;
	bool yes=false;

	// Crea el fil per a llegir pendrives
	pthread_t USB_thread_id;
	pthread_create ( &USB_thread_id, NULL, USB_thread, NULL );

	int cicles = contadorCiclos();
	cargarConfiguracio();

	// Inicialisar Digital Port
	wiringPiSetup();
	pinMode( pin_11,OUTPUT);
	pinMode( pin_12,OUTPUT);
	pinMode( pin_7 ,OUTPUT);

	//init graphics and the camera
	InitGraphics();

	// Enable Temperature Sensor
	int fd=wiringPiI2CSetup(0x18);

	// Crear Display
	Display display( WIDTH_PANT, HEIGHT_PANT, esMaster);
	display.conectar();

	// Crear Sistemas de Visión
	int numeroCamaras=Cantitat_Camares; // Por Sistema de Visión
	// int config=1; Una raspberry, int config=4; 4 raspberry
	sistema_visio* SV = reinterpret_cast<sistema_visio *>(new char[2 * sizeof(sistema_visio)]);

	if( config==4 ) {
		int Port_IP = 1999+id_Sistema_visio;
		int numPort_SV1[4]={2,4,0,0};  //int numPort_SV1[4]={4,2,0,0};
		int idSV = id_Sistema_visio;
		new(&SV[0]) sistema_visio( idSV, config, numeroCamaras, numPort_SV1, &display, cicles, Port_IP );
	}

	if( config==1 ) {
		int Port_IP = 2000;
		int numPort_SV1[4]={4,0,0,0};
		int idSV=2;
		new(&SV[0]) sistema_visio( idSV, config, numeroCamaras, numPort_SV1, &display, cicles, Port_IP );
		Port_IP++;
		idSV=1;
		int numPort_SV2[4]={2,0,0,0};
		new(&SV[1]) sistema_visio( idSV, config, numeroCamaras, numPort_SV2, &display, cicles, Port_IP );
	}


	long int i=0;
	int index;
	long int contadorDePrograma=0;

	while(1)
	{
		struct timespec t0 = Inicia_Temporisador();

		// llig temperatura
		if(esMaster || config==1 ) {
			temperatura = temp(fd);
			cout << "Temp: " << temperatura << "ºC" << endl;
		}

		bool found=true, compensada=true;
		for ( int iSV=0; iSV<Cantitat_Sistemes_Visio; iSV++) {
			compensada = compensada && SV[iSV].getEstatCompensada();
		}

		compensando = !compensada;
		if(compensando) cout << "Compensandoooooooooo " << endl;

		// ABORTAR LA CAPTURA
		if(abortar) {
			for (int id=0; id<Cantitat_Sistemes_Visio; id++) {
				SV[id].finCapturaSV();
			}
			abortar = false;
		}

		// Processa image, calcula interseccions, calcula
		for ( int iSV=0; iSV<Cantitat_Sistemes_Visio; iSV++) {
			SV[iSV].proceso();
			SV[iSV].Set_Estat_Enviament (FES_HO); // FES_HO
			cout << endl;
		}

		// Re-Compensa en el cas de que se li done l'orde
		if (recompensa) {
			StopCamera();
			cam = StartCamera( WIDTH_HD, HEIGHT_HD, 7,1,false );
			//cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
			for ( int iSV=0; iSV<Cantitat_Sistemes_Visio; iSV++) {
				SV[iSV].recompensar();
			}
			recompensa=false;
		}

			long int espera = (long int)floor((1.0-Medix_temps(t0))*1000000);
			if(espera>0 && espera<1000000 ) {
				usleep(espera);
			}

		cout << contadorDePrograma <<" · TEMPS DE BUCLE: " << Medix_temps(t0) << endl;
		cout << "_____________________________________________________________" << endl;

		i++;
		contadorDePrograma++;
		if(apagar_RPI)
		{
			sync();
			system("sudo halt");
			return 3;
		}
	}
}
