#pragma once

#include "../llibreries/llibreries.hpp"


using namespace cv;
using namespace std;


/*
Clase clienteTCP_IP
*/
clienteTCP_IP::clienteTCP_IP( camara* CAMARA )
{
	//unsigned char* circul   = (unsigned char*) malloc (sizeof(char) *2592*2592);
	camara *CAM = (camara*) CAMARA;

	// Configurar TCP/IP
	struct hostent *host;
        struct sockaddr_in server_addr;

	int sin_size;
	host = gethostbyname("192.168.1.103");           //IP_Servidor
 
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(444);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);   
	bzero(&(server_addr.sin_zero),8);

	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("Socket");

	printf("Buscando servidor: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
	do
	{	
		usleep(50000);
	}while(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1); // || !con_conexion
	
	printf("Conectado a: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
}


/*
* @ Capçalera
*/
int clienteTCP_IP::cabecera( int TAM, float temperatura, int Id, int NUM_IMAGE )
{
	int bytes_recieved , si = 1, i, ENVIAT, repetir =0,j=0, BYTES=1;

	int ni = TAM;
	char FI[5]="0", CONFIRMACIO[100]="";         
           
	char INFO[70] = "";

	string tempt   = float2string(temperatura);
	string ample1  = int2string(WIDTH_FRAME);  //2592
	string alt1    = int2string(HEIGHT_FRAME); //2592
	string BY1     = int2string(BYTES);

	string NOM1   = NOM_000X (NUM_IMAGE);
	string I             = NOM_000X (ni);

	string Id_Camara;
	//if(CAM->GetId()==1) Id_Camara = "RASP01_ ";
	//if(CAM->GetId()==2) Id_Camara = "RASP02_ ";
	Id_Camara = "RASP01_ ";

	string INFO6 = Id_Camara + NOM1 + " " + ample1 + " " + alt1 + " " + BY1
		     + " " + "0972" + " " + "1296" + " " + "0972" + " " + I 
             + " " + int2string((int)0) + " " + int2string((int)0) + " " + tempt  +'\0';

	//cout << INFO6 << endl;
	int kt = INFO6.size(); 
	for(int k = 0; k < kt ; k++) INFO[k]=INFO6[k];
	INFO[kt]  = '\0';
	INFO6[kt] = '\0';

		
	// Enviar INFO del DISPOSITIU i MENSAGE
	if(send(sock,&INFO,71,0)==-1) return -1; 
	
	// Rebre Configuracio
	if(recv(sock,&CONFIRMACIO,99,0)==-1) return -1;

	return 1;
}


/*
* Enviar image al Servidor
*/
int clienteTCP_IP::ENVIAR_IMAGE (char* image, int TAM )  //const uint8_t* image
{
	int enviat;
	char FI[5]="0";
	int ni=TAM;

	// Enviar l'image al servidor
	if( (enviat = send(sock, (unsigned char*)image, ni, 0)) ==-1 ) return -1;

	// Rebre una ultima instruccio del Servidor	
	if(recv(sock,FI,sizeof(FI),0)==-1) return -1; 

	return 1;
}



