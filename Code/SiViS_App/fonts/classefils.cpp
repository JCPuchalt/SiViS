#pragma once

#include "../llibreries/llibreries.hpp"
#include "../llibreries/classefils.hpp"

using namespace std;

void fils::InternalThreadEntry( )
{
	unsigned char* circul   = (unsigned char*) malloc (sizeof(char) *2592*2592);

	// Configurar TCP/IP
	struct hostent *host;
        struct sockaddr_in server_addr;

	int sock;
	int sin_size;
	host = gethostbyname(IP_Servidor);        //  
 
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(444);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);   
	bzero(&(server_addr.sin_zero),8);

	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("Socket");

	printf("Buscando servidor: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
		int estat = SV->Get_Estat_Enviament();
		int id = SV->GetId();
		cout << "ID " << id << ", estat -> " << estat << endl;
	do
	{	
		usleep(50000);
	}while(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1); // || !con_conexion
	
	printf("Conectado a: (%s , %d) \n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
		estat = SV->Get_Estat_Enviament();
		id = SV->GetId();
		cout << "ID " << id << ", estat -> " << estat << endl;

	while(1)
	{	
		estat = SV->Get_Estat_Enviament();
		cout << "ID: " << SV->GetId() << ", Estat: " << estat << endl;
		if(estat==FES_HO)
		{
			struct timespec t0 = Inicia_Temporisador();
			SV->Set_Estat_Enviament(OCUPAT);
			int TAM = CIRCUL (circul, (unsigned char*)(SV->Get_Vect_Im()));
			
            		//SV->Configuracio_Remota(sock, TAM, temperatura, SV->GetId(), SV->Get_It_Imagens());  //2966132
			bool estado_conexion = true;
           		//bool estado_conexion = ENVIAR_IMAGE( (char*)circul, sock, TAM );

  			cout << "Temps Envio: "<< Medix_temps(t0) << endl;

			SV->Set_Estat_Enviament(ACABAT);
			SV->Set_Resultat_Enviament(estado_conexion);
		}
		else {
			usleep(50000);
		}
	}
	free(circul);
	close(sock);
}

