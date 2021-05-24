#pragma once

#include "llibreries.hpp"

//unsigned char* inv_quadrat = (unsigned char*) malloc(sizeof(char)*1944*1944);
void  find_white_pixels  	( unsigned char* image, int ample, int alt, ofstream& centroide,  ofstream& punt_image, float pantalla_x, float pantalla_y);
void  test_calibracion2  	( unsigned char* image, int ample, int alt, ofstream& arch_error, ofstream& punt_image, float pantalla_x, float pantalla_y, float* corregir_x, float* corregir_y, GfxTexture* tex_filtrada);
void  test_calibracion   	( unsigned char* image, int ample, int alt, ofstream& centroide, ofstream& punt_image, float pantalla_x, float pantalla_y, float* corregir_x, float* corregir_y, GfxTexture* tex_filtrada);


void  aprendre			( void* camera, float* param_gauss, int* N, int tam_comp, char* ruta1, int N_Experiencies );
void  GET_GAUSS			( float * P, char* rut );
void  FONDO_GUARDAT		( char* ruta3, float* param_gauss );
long  LLEGIR_IMAGE		( char* dir, unsigned char* buffer_k );
void  CALCUL_MIJA_DESVIACIO	( unsigned char* buffer_k, int N_Experiencies, int* N, float* Mig, float* param_gauss, int lSize, unsigned char* Min_Max, int k, int Min, int Max );
void  CALCUL_GAUSSIANA		( float* param_gauss, int* N, int tam_comp, char* ruta1 );
int   CALCUL_PROVABILITAT	( unsigned char* buffer_k, unsigned char* digital1, float* param_gauss, int tam_comp, int* N, float* P );
float gaussian_pdf 		( float x, float mhu, float sigma );
float gaussian_acumulada 	( float x, float mhu, float sigma );
int   COMPRESSIO		( unsigned char* comprimit, unsigned char* digital1, int canvis, int tam_comp); // , int* N, float* P );
void  DESCOMPRESSIO		( unsigned char* digital1, unsigned char* comprimit, int tam_comp, int tam_comp2 );
void  RECONSTRUIR_CIRCUL	( unsigned char* digital2, unsigned char* digital1, int ample, int alt);
void  MOSTRAR_IMAGE		( unsigned char* digital2, int ample, int alt, int colors);
int   CIRCUL			( unsigned char* circul, unsigned char* image);
int   QUADRAT			( unsigned char* quadrat, unsigned char* image);
int   ENVIAR_IMAGE		( char* image, int socket, int TAM );  //const uint8_t*
void* config_cam		( void* camera);
void  GUARDAR			( unsigned char* dada, int TAM);
void  GEN_NOM 			( int numero, char* nombre );
void  GEN_RUTA			( char* direccio_gen, char* nom_carpeta, int numero );
void* CAPTURA_IMAGEN		( void* dat );
bool  EXISTIX_PROYECTE		( void );
bool  CREAR_PROYECTE		( void );
float temp			( int fd );


string 	int2string	( int sancer );
const char* itoa	( int entero );
string 	float2string	( float sancer );
string 	NOM_000X	( int sequencia );
bool 	mat2array 	( char* matriuEnviar, Mat matriu, int m, int n);
bool 	double2string 	( string *array, double valor);
bool 	string2array 	( char *array , string strng);
bool	array2matriu	( Mat *matriuOut, char matriuRebuda[], int m, int n );
bool	array2double	( double* valor, char array[], int precisio);


void Hora_Actual	( char buf[] );

struct 	timespec Inicia_Temporisador(void);
double  Medix_temps 		(struct timespec t0 );
bool	crear_carpeta		( char generada[] );
void	generar_ruta_completa	( char *ruta_completa, char raiz[], char generada[] );
void	generar_carpeta		( char *generada1, char *generada2, char nombre[], int dia, char condicion, int probeta );
string	AC2S			( char i[] );
string	C2S			( char i );
bool	existe			( char nombre[] );
string	I2S			( int i );
string	F2S			( float i );

