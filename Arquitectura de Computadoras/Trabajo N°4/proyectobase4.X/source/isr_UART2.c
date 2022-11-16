/* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Ricardo López    11/6/10  UART2C module interrupt handler
* Marcelo Gómez    09/03/14 Portado a MPLAB.X
* Marcelo Gómez    20/05/15 Portado a MPLAB.X
* Sebastián Wahler 04/06/19 Simplificado
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
*
**********************************************************************/

#include "p33FJ256GP710.h"

//UART
#define FCY 40000000
#define BAUDRATE 19200
#define BRGVAL ((FCY / BAUDRATE) / 16) -1
#define max 8

/*---------------------------------------------------------------------
  Function Name: UART2Interrupt
  Description:   UART2 Interrupt Handler
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
unsigned int i , qty = 0;
int buffer[max];
unsigned int termino_rx, termino_tx = 0;  
//Interrupciones de la UART2
void __attribute__((interrupt, auto_psv)) _U2RXInterrupt( void )
{                  
    IFS1bits.U2RXIF = 0;     
    if(termino_rx == 0) /*termino_rx = falso*/
    {    
        if(qty != 0) 
        {                
            buffer[i]= U2RXREG;     /*Llegan los valores al arreglo*/
            i++;
        }             
        if(qty == 0) /*qty lleva la cantidad del arreglo*/
        {
            qty = U2RXREG;  
            buffer[i] = qty;
            i++;
        }
        if(i == qty)        
        {                 
            TMR4 = 0;     
            T4CONbits.TON = 1;        	      /*encender timer 4*/                          
            termino_rx = 1;   /*termino_rx = verdadero*/               
        }          
    }           
}

/*Rutina de TX.*/
void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void)
{	                 
    IFS1bits.U2TXIF = 0;
    if(termino_tx == 0)     /*termino_tx = falso*/
    {                     
        if(qty > 0)
        {
            U2TXREG = buffer[i];
            i++;
            qty--;       
        }
        if(qty == 0)
        {
            termino_tx = 1;               
            termino_rx = 1;
        }
    }        
}       
/* 
    * Despues de que termina interviene la rutina de RX
    *haria que lanzar RX ( IFS1bits.U2RXIF = 0;)  
 
 
 */

/*---------------------------------------------------------------------
  Function Name: InitUART2
  Description:   Inicializar UART2
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void InitUART2(void)
{
	// The HPC16 board has a DB9 connector wired to UART2, 
	// so we will be configuring this port only
	// configure U2MODE
	U2MODEbits.UARTEN = 1;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	U2MODEbits.RTSMD = 1;	// Bit11 Simplex Mode
    U2MODEbits.BRGH = 0; //low speed
    U2MODEbits.PDSEL = 0; // 8N
    U2MODEbits.STSEL = 0; // 1 bit de stop
    U2STAbits.PERR = 0;
    U2STAbits.URXISEL = 0; //El bit del flag de interrupcion es setiado cuando recibe un caracter

	// Load a value into Baud Rate Generator.  Example is for 19200.
	U2BRG = BRGVAL;	// 40Mhz osc, 19200 Baud

	IPC7 = 0x4400;	// Mid Range Interrupt Priority level, no urgent reason

	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC1bits.U2RXIE = 1;	// Enable Recieve Interrupts    

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
	U2STAbits.UTXEN = 1;	// Empieza a transmitir. Se dispara el Flag TXIF

	IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
	IEC1bits.U2TXIE = 1;	// Enable Transmit Interrupts
}
