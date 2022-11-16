/**********************************************************************
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Ricardo López     03/11/2011  1ra. versión
* Sebastián Wahler  08/05/2013  2da. versión
* Marcelo Gómez     09/03/2014  Portado a MPLAB.X
* Sebastián Wahler  04/06/2019  Simplificado
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
**********************************************************************/
#include "p33FJ256GP710.h"
#include "common.h"
#include "config.h"


void config( void )
{
	//Variables para clock del sistema
	hours, minutes, seconds = 0;
/*
	Inicialización de PORTS I/O
	 (RA0-RA7) como salidas - LEDs 
	TRISA = 0xFF00; 
    */
	/* Initializar UART2*/
	InitUART2();

	/* Inicializar Timers necesarios */
	Init_Timer4();

}
