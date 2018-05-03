/* DRIVER using SUSI for the MOVEBOX */
/* BALYO */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)

#define PORT	 	75
#define MAX_CLIENTS 	100

#define BUF_SIZE	1024

#include <ctype.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <inttypes.h>
#include <signal.h>


#include "SUSIDriver/Susi4.h"


#include "DriverLibrary.h"



int main(int argc, char *argv[]){
    /** Variables Ethernet **/
    SOCKET socketCreated;
    socketCreated = socket(AF_INET,SOCK_STREAM,0);

    SOCKADDR_IN sin;
     sin.sin_addr.s_addr = inet_addr("192.168.5.21");
     sin.sin_family = AF_INET;
     sin.sin_port = htons(276);
     size_t recsize = sizeof(sin);
    unsigned char  buffer[1024];
    int succeed=0;
    int nbitsent;
    int dataReceived, indiceEthernetToConnect;
    /** Variables Driver **/
    int i;
    int connected = 0;
    int choice;
    unsigned char grandeTrame[NumberOfBits];
    unsigned char frame4test[NumberOfBits];
    unsigned int paquet = 0;
    for(i = 0; i < NumberOfBits ;i++){ // Initialization trame
        grandeTrame[i] = 0;
    }
    /** Beginning **/
    connected = initializationDriver(&grandeTrame[0]);
    if(connected){
        dec2bin(&grandeTrame[0], CRC, NumberOfBits); // Ecriture du nombre des données dans la trame
    	dec2bin(&grandeTrame[0], SortiesN, 1); // Ecriture de la version
        crcCalcul(&grandeTrame[0]); // Calcul du CRC
        printf("Local frame:\n");
        printTrameBinary(&grandeTrame[0]); //Affichage de la trame pour connaitre son structure
        while(connect(socketCreated, (SOCKADDR*)&sin, (socklen_t)recsize)==SOCKET_ERROR){ // On avance pas jusqu'au ce que l'ethernet soit connecté
        }
        printf("*******************\nConexion Ethernet succeed\n*******************\n");
        dataReceived = recv(socketCreated, buffer, BUF_SIZE - 1, 0); // Reception de données via ethernet
        if (dataReceived<0){
            printf("Data don't received\n");
        }
        else{
            printf("Data received 0: %s \n",buffer);
            for(int i = 0; i < dataReceived; i++){
                if(buffer[i]=='1') buffer[i] = 1;
                else buffer[i] = 0;
            }
            buffer[dataReceived] = '\0';
        }
        do{
            choice = 5;
            writeGPIO(&grandeTrame[0], &paquet, choice,&buffer[0]);
            readGPIO(&grandeTrame[0]);
            printTrameHex(&grandeTrame[0]);
            if(choice == 5){
                unsigned char trame[dataReceived];
                for(i = 0; i < dataReceived ;i ++){
                    if(grandeTrame[SortiesN + i] == 1 ) trame[i] = '1';
                    else trame[i] = '0';
                    printf("%c", trame[i]);
                }
                nbitsent = send(socketCreated, trame, strlen(trame), 0);
                if(nbitsent < 0){
                    perror("\nError!!\n");
                }else{
                    printf("\nSucces\n");
                }
            }
            printf("Do you want to leave? [1|0]");
            scanf("%d", &choice);

        }while(choice != 1);
        leaveSUSI();
        close(socketCreated);
    }
    return 0;
}


