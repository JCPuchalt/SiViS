#pragma once

#include "llibreries.hpp"


#define FIFO_SIZE 20

using namespace cv;
using namespace std;


struct image_t
{
    int tamany;
    char direccion[150];
    unsigned char* image;
};

/*
 * @ Classe Buffer Circular
 *
 * @ Crea una cola de imágenes que se irán guardando en disco
 * @ 
 * @ 
*/
class bufferCircular
{

    image_t imageBuffer[FIFO_SIZE];

    int idxLast;
    int idxFI;
    int enCola;

public:
    bufferCircular();

    void addImage(unsigned char* Im, char dir[], int tamany);
    bool remImage(unsigned char* Im, char *direccion, int &tam );
    void reset(void);
    int checkAvail(void);

};
