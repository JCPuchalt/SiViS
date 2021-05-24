#pragma once

#include "llibreries.hpp"


using namespace cv;
using namespace std;


/*
 * @ Classe 
 *
 * @ Crea 
 * @ 
 * @ 
*/
class objetos
{
private:
	vector<Point> 	PIXELS;
	vector<Point> 	CONTOURS;
	Point2f 	CENTROIDE;
	vector<Point>	CONTOURS_POLY;
	Rect		BOUND_RECT;
	float		RADIUS;
	int AREA;
	int yDiferencia;
public:
	objetos();
	objetos( Point2f Centroide);
	objetos( Point2f Centroide, vector<Point> Contours, Rect BoundRect, float Radius, int ydesfase );
	~objetos() {}	
	
	void setCentroide	( Point2f valor ) 	{ CENTROIDE = valor; }
	void setArea		( int valor )		{ AREA	    = valor; }
	
	Point2f		centroide	( void )		{ return CENTROIDE;	}
	Point2f		centroide	( bool ABS )		{ return Point2f(CENTROIDE.y+yDiferencia, CENTROIDE.x);	}
	int		area		( void )		{ return AREA;		}
	vector<Point>	contours	( void )		{ return CONTOURS;	}
	vector<Point>	pixels		( void )		{ return PIXELS;	}
	Rect		boundRect	( void )		{ return BOUND_RECT;	}
	float		radius		( void )		{ return RADIUS;	}
	vector<Point>	contours_poly	( void )		{ return CONTOURS_POLY;	}

	void addPixel		( Point2f valor )		{ PIXELS.push_back(valor); }
	void addPixels		( Mat &imageGrisos, int level );
	void addPixels		( Mat &imageGrisos );

	void eliminarObjeto	( Mat &imageGrisos );
	void eliminarObjeto 	( Mat &imageGrisos, int llimit );
	void copiarObjeto 	( Mat &imageGrisosDest, Mat &imageGrisosOrig  );
	void copiarObjeto 	( Mat &imageGrisosDest, Mat &imageGrisosOrig, int llimit  );
	bool talla 		( int llinia );
};
