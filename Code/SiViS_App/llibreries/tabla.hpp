#pragma once

#include "llibreries.hpp"



using namespace cv;
using namespace std;




/*
 * @ Classe Tabla
 *
 * @ 
 * @ 
 * @ 
*/
class tabla
{
	int* tablaPosciones;
	unsigned char* tablaImage;

	int tamanoReservado;
	int tamano;


public:
	tabla();

	getPosiciones
	getImagen
	int  size();
	int  posicio	( int i );
	int  color	( int i );
	void setPosicio	( int i, int pos );
	void setColor	( int i, int col );
	bool sendImage	();
	bool sendTabla	();
	bool recvImage	();
	bool recvTabla	();
	void reset();

};
