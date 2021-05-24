#pragma once

#include "../llibreries/llibreries.hpp"
#include "../llibreries/bufferCircular.hpp"


using namespace cv;
using namespace std;


/*****************************************************************************************************************
Clase Buffer Circular
******************************************************************************************************************/
bufferCircular::bufferCircular()
{
	idxLast= 0;
	idxFI  = 0;
	enCola = 0;
	for(int it=0; it<FIFO_SIZE; it++)
		imageBuffer[it].image = (unsigned char*) malloc ( sizeof(char*)*2592*2592 );
}


void bufferCircular::addImage(unsigned char* Im, char dir[], int tamany)
{
	strcpy(imageBuffer[idxLast].direccion, dir);
	imageBuffer[idxLast].tamany = tamany;
	memcpy( imageBuffer[idxLast].image, Im, tamany );

	//cout << "Añadida: " << imageBuffer[idxLast].direccion <<  ", Last: " << idxLast << endl;

	idxLast++;
	if(idxLast>=FIFO_SIZE)
		idxLast=0;
}


bool bufferCircular::remImage(unsigned char* Im, char *direccion, int &tam )
{
	checkAvail();
	if(enCola>0)
	{
		//cout << "En cola: " << enCola << endl;
		tam = imageBuffer[idxFI].tamany;
		memcpy( Im, imageBuffer[idxFI].image, imageBuffer[idxFI].tamany );
		strcpy( direccion, imageBuffer[idxFI].direccion );

		//cout << "Quitada: " << imageBuffer[idxFI].direccion <<  ", Frst: " << idxFI << endl;

		idxFI++;
		if(idxFI>=FIFO_SIZE)
			idxFI=0;

		//checkAvail();
		//cout << "En cola: " << enCola << endl;
		return true;
	}
	return false;
}


void bufferCircular::reset(void)
{
    idxLast=  0;
    idxFI  =  0;
}


int bufferCircular::checkAvail(void)
{
    if(idxFI>idxLast)
        enCola=FIFO_SIZE+idxLast-idxFI;
    else if (idxFI<=idxLast)
        enCola=idxLast-idxFI;

    if(enCola<0)
        enCola=0;

    return enCola;
}


