/**********************************************************************
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Ricardo L�pez     03/11/2011  1ra. versi�n
* Sebasti�n Wahler  08/05/2013  2da. versi�n
* Marcelo G�mez     09/03/2014  Portado a MPLAB.X
* Sebasti�n Wahler  04/06/2019  Simplificado
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
**********************************************************************/
#include "p33FJ256GP710.h"
#include "common.h"
#include "config.h"
void config(void)
{
	//Variables para clock del sistema

	//Inicializaci�n de PORTS I/O
    TRISAbits.TRISA0 = 0;       //cables del portd para los cables
    TRISD = 0x20C0;             //
    PORTD = 0x0000;             //setea los cables
		
    InitCN();
	/* Initializar UART2*/
	InitUART2();
	/* Inicializar Timers necesarios */
//    Init_Timer1();
	Init_Timer4();
    Init_Timer6();
}
