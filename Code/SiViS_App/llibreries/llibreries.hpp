#pragma once

//#include <stdio.h>
#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv_modules.hpp> 
#include <opencv2/stitching.hpp> 
#include <iomanip>
#include "TCP_IP.hpp"
#include "objetos.hpp"
#include "camara.hpp"
#include "camera.h"
#include "graphics.h"
#include "sistema_visio.hpp"
#include "CE_Funcions.hpp"
#include "bufferCircular.hpp"
#include "Display.hpp"
#include "classefils.hpp"
#include <sys/time.h>
#include <curses.h>
#include <ncurses.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <resolv.h>
#include <bitset>
#include <math.h> 
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>
//#include <thread> 
#include <limits>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <errno.h>
#include <time.h>
#include <omp.h>
//#include <SFML/Audio.hpp>
#include <libudev.h>
#include <locale.h>
#include <dirent.h>
#include <vector>
#include <wiringPiSPI.h>


using namespace cv;
using namespace std;

enum { MOSTR_TEX_COMPENSADA, MOSTR_TEX_IMAGE };
enum { LLIURE, OCUPAT, ACABAT, FES_HO };

#define NUMTHRDS 4

#define	pin_11	0
#define	pin_12	1
#define	pin_7	7

#define bit_0 2
#define bit_1 3
#define bit_2 12
#define bit_3 13
#define bit_4 14
#define bit_5 21
#define bit_6 22
#define bit_7 23
#define bit_sync 24
#define bit_OK 25 

#define texelRatio 0.1926/0.1790

#define WIDTH_PANT  800*texelRatio     //430.391061453
#define HEIGHT_PANT 480                  //480

#define WIDTH_HD  2592  // 3280
#define HEIGHT_HD 1944  // 2464

extern int WIDTH_FRAME;  // 3280
extern int HEIGHT_FRAME; // 2464

//#define WIDTH_FRAME  1944  // 3280
//#define HEIGHT_FRAME 1944  // 2464

extern pthread_mutex_t mutexEnviament; // = PTHREAD_MUTEX_INITIALIZER;
extern int  config;

extern char tmpbuff[WIDTH_HD*HEIGHT_HD*4];

	extern bool esMaster;
	extern bool withMUX;
	extern char IP_Servidor[15];
	extern char IP_Master[15];
	extern char IP_SV_Ppal[15];
	extern int id_Raspberry;
	extern int id_Sistema_visio;
	extern int Cantitat_Sistemes_Visio;
	extern int Cantitat_Camares;
	extern int idCam_Ppal;

	extern bool SV_Principal;
	extern bool SV_Secundario;

	extern bool MODO_CALIBRACION;
	extern bool MODO_ENFOQUE;
	extern bool MODO_FOTO;
	
	extern int NUM_DISPLAYS;
	extern int ch;

extern bool G_Local;
extern bool abortar;
extern bool apagar_RPI;
extern bool compensando;
extern bool recompensa;
extern stringstream ss;

extern float umbral;

extern char nombre[50];
extern char nom_proyecte[100];
extern char carpeta[50];
extern char direccio[150];
extern char sub_dir[150];
extern char NOM[15];

extern int fname;
extern CCamera* cam;
extern float temperatura;

extern ofstream ficher_analisis;
extern ofstream ficherEstudiBateria1;
extern ofstream ficherEstudiBateriaMatlab1;
extern ofstream ficherEstudiBateria2;
extern ofstream ficherEstudiBateriaMatlab2;

extern int Ur[60][2] ;
extern float puntos_pantalla[90][2]; 
extern ofstream ficher;

extern long int start_time;

extern int Num_pendrives;
extern int Num_pen;

extern char DIR_PEN[60];
extern string directorio_pendrive;

struct relaciones {
	int element1;
	int element2;

	int area1;
	int area2;

	float distancia;
	float pDistancia;

	float pTamany;
	float pTotal;
};

