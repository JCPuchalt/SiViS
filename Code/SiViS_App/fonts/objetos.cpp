#pragma once

#include "../llibreries/llibreries.hpp"
#include "../llibreries/objetos.hpp"


using namespace cv;
using namespace std;


/*
Clase Objectes
*/
objetos::objetos()
{

}

objetos::objetos( Point2f Centroide)
{
	CENTROIDE = Centroide;
}

objetos::objetos( Point2f Centroide, vector<Point> Contours, Rect BoundRect, float Radius, int ydesfase)
{
	AREA=0;
	yDiferencia 	= ydesfase;
	
	BOUND_RECT	= BoundRect;
	RADIUS		= Radius;
	CENTROIDE	= Centroide;

	int xBox = BOUND_RECT.br().x - BOUND_RECT.tl().x;
	int yBox = BOUND_RECT.br().y - BOUND_RECT.tl().y;

	//PIXELS.reserve(1000*1000);		//xBox*yBox
	//CONTOURS.reserve(1000*1000);		//xBox*yBox
	//CONTOURS_POLY.reserve(1000*1000);	//xBox*yBox

	CONTOURS	= Contours;
	//CONTOURS_POLY	= Contours_Poly;

}


// Deteccio de pixels en l'interior dels contorns per mig de NIVELL
void objetos::addPixels	( Mat &imageGrisos, int level )
{
	for (int ix=BOUND_RECT.tl().x ; ix<BOUND_RECT.br().x; ix++ ) {
		for (int iy=BOUND_RECT.tl().y ; iy<BOUND_RECT.br().y; iy++ ) {
			if( imageGrisos.at<uchar>(iy, ix)<level ) {
				Point pixel (ix, iy);
				PIXELS.push_back(pixel);
				AREA++;
			}
		}
	}
}


// Deteccio de pixels en l'interior dels contorns per mig de provabilitat GAUSSIANA
void objetos::addPixels	( Mat &imageGrisos )
{

	int fi[256]={0};
	float minim=1000.0, maxim=0.0, mija=0.0;
	int N=0;

	// Calculs estadistics: Calcul Minim, Maxim, freqüencies relatives, mija i numero d'elements.
	for (int ix=BOUND_RECT.tl().x-10 ; ix<BOUND_RECT.br().x+10; ix++ ) {
		for (int iy=BOUND_RECT.tl().y-10 ; iy<BOUND_RECT.br().y+10; iy++ ) {
			if(ix>=0 && iy>=0 && ix<imageGrisos.size().width && iy<imageGrisos.size().height ) {
				int color = imageGrisos.at<uchar>(iy, ix);
				fi[color]++;
				mija += (float)color;
				if(color < minim ) minim = color;
				if(color > maxim ) maxim = color;
				N++;
			}
		}
	}
	mija = mija/(float)N;


	// Calcul de la Mijana
	int posMijana = (int)round(N/2);
	int Fi = 0, mijana=0;
	for (int xi=0; xi<256; xi++) {
		Fi += fi[xi];
		if( Fi >= posMijana ) {
			mijana = xi;
			break;
		}
	}


	// Calcul de la Moda
	int modaFondo=0, modaCE=0;
	for (int xi=0; xi<256; xi++) {
		if( fi[xi]>modaCE && xi<mijana) {
			modaCE = xi;
		}
		if( fi[xi]>modaFondo && xi>mijana) {
			modaFondo = xi;
		}
	}	

	// Filtrage
	for (int ix=BOUND_RECT.tl().x ; ix<BOUND_RECT.br().x; ix++ ) {
		for (int iy=BOUND_RECT.tl().y ; iy<BOUND_RECT.br().y; iy++ ) {
			int color = imageGrisos.at<uchar>(iy, ix);
			float pCE    = gaussian_pdf (color, modaCE   , mijana - minim );
			float pFondo = gaussian_pdf (color, modaFondo, maxim - mijana );
			if( pCE > pFondo ) {
				Point pixel (ix, iy);
				PIXELS.push_back(pixel);
				AREA++;
			}
		}
	}
}


/*
* @ Copia l'objecte
*/
void objetos::copiarObjeto ( Mat &imageGrisosDest, Mat &imageGrisosOrig  )
{
	int llimit1 = 77;
	int llimit2 = 60;
	int variacioY = llimit1-llimit2;

	for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
		Point pixel = PIXELS.at(idxPixel);
		int iy = pixel.y, ix = pixel.x;
		//imageGrisosDest.at<uchar>(iy+variacioY, ix) = imageGrisosOrig.at<uchar>(iy, ix);
		imageGrisosDest.at<uchar>(iy+yDiferencia, ix) = imageGrisosOrig.at<uchar>(iy, ix);
	}
}


/*
* @ Copia part de l'objecte
*/
void objetos::copiarObjeto ( Mat &imageGrisosDest, Mat &imageGrisosOrig, int llimit  )
{
	int limite = abs(llimit);

	if(llimit>0) { 	// (+) Dalt
		for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
			Point pixel = PIXELS.at(idxPixel);
			int iy = pixel.y, ix = pixel.x;
			if( iy <= llimit ) {
				imageGrisosDest.at<uchar>(iy, ix) = imageGrisosOrig.at<uchar>(iy, ix);
			}
		}
	}
	else { 		// (-) Baix
		for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
			Point pixel = PIXELS.at(idxPixel);
			int iy = pixel.y, ix = pixel.x;
			if( iy > llimit ) {
				imageGrisosDest.at<uchar>(iy, ix) = imageGrisosOrig.at<uchar>(iy, ix);
			}
		}
	}
}


void objetos::eliminarObjeto ( Mat &imageGrisos )
{
	for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
		Point pixel = PIXELS.at(idxPixel);
		int iy = pixel.y, ix = pixel.x;
		//cout << "[" << ix << "," << iy << "]" << " - " << pixel <<  endl; 
		imageGrisos.at<uchar>(iy, ix) = 48;
	}
}

void objetos::eliminarObjeto ( Mat &imageGrisos, int llimit )
{
	int limite = abs(llimit);

	if(llimit>0) {	// (+) Dalt
		for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
			Point pixel = PIXELS.at(idxPixel);
			int iy = pixel.y, ix = pixel.x;
			if( iy <= limite ) {
				imageGrisos.at<uchar>(iy, ix) = 48;
			}
		}
	} else {	// (-) baix
		for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
			Point pixel = PIXELS.at(idxPixel);
			int iy = pixel.y, ix = pixel.x;
			if( iy > limite ) {
				imageGrisos.at<uchar>(iy, ix) = 48;
			}
		}
	}
}

bool objetos::talla (int llinia)
{
	//cout << "Objecte ->" << endl;
	for(int idxPixel=0; idxPixel<PIXELS.size(); idxPixel++) {
		Point pixel = PIXELS.at(idxPixel);
		int iy = pixel.y;
		if( iy == llinia ) {
			//cout << llinia << " · " << pixel << endl;
			return true;
		}
	}
	return false;
}


