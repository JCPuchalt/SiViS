#pragma once

#include "llibreries.hpp"


using namespace cv;
using namespace std;

class camara;

/*
 * @ Classe 
 *
 * @ Crea 
 * @ 
 * @ 
*/
class clienteTCP_IP
{
private:
	int sock;
	camara* CAM;
public:
	clienteTCP_IP( camara* CAMARA );
	~clienteTCP_IP() { close(sock); }	
	
	int cabecera		( int TAM, float temperatura, int Id, int NUM_IMAGE );
	int ENVIAR_IMAGE	( char* image, int TAM );
};
