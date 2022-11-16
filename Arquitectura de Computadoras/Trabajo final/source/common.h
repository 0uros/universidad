/* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Richard Fischer   07/14/05  general variable linkage information
* Priyabrata Sinha  01/27/06  Ported to non-prototype devices
* Sebastián Wahler  04/06/19  Simplificado
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* 1. This file contains definitions commonly used in this project.
*
**********************************************************************/
#define MAX_V 50
#define MAX 20

#define FCY 40000000
#define BAUDRATE 9600
#define BRGVAL ((FCY / BAUDRATE) / 16) -1




#define TRUE	1
#define FALSE   0
#define CORRECTO   1
#define INCORRECTO   0

#define HAY_VEHICULO 1      
#define DIST_CABLES 30      //distancia entre cables   
#define KMxH 36             //pasaje de unidades de cm/ms a km/h
#define RECIBIDO 1

#define DIR_PLACA 3
#define DIR_PC 2

#define SOF 0
#define QTY 1
#define DST 2
#define SRC 3
#define SEC 4
#define COM 5


#define VAL_SOF 0xFE
#define VAL_SEC 0x80

/* Variables usadas con el Timer */
extern volatile unsigned char hours;
extern volatile unsigned char minutes;
extern volatile unsigned char seconds;


/* variables usadas en la UART */
extern volatile unsigned char uart_update;
extern volatile unsigned char caracter;
extern int cantV, comTx;
extern int checksum,qty,sum,cant;
extern int estadoTrama;
extern char recive[MAX];
extern char transmit[MAX];



typedef struct{         //estructura para guardar los datos de la hora en la que pasó el vehiculo
    int hour;
    int min;
    int seg;
} tHora;
typedef struct{         //estructura que tiene la hora del vehiculo, cantidad de ejes y velocidad
    tHora hora;
    int cantEjes;
    int velocidad;
}t_vehiculo;