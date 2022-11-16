#include "p33FJ256GP710.h"
#include "config.h"

#define ValPR4	40000
#define ValPR6	0

volatile unsigned char hours;
volatile unsigned char minutes;
volatile unsigned char seconds;
extern int i, es_qty, counterTog;

/*---------------------------------------------------------------------
  Function Name: _T4Interrupt
  Description:   Timer4 Interrupt Handler

Comments: 39062
-----------------------------------------------------------------------*/
void __attribute__((interrupt, auto_psv)) _T4Interrupt( void )
{
	/* reset Timer 4 interrupt flag */
 	IFS1bits.T4IF = 0;       
    counterTog ++; //controlar un segundo                 
    if(counterTog == 4) /*250 ms * 4 = 1 segundo*/            
    {                
        i = 0;  /*i se reinicia para TX*/           
        counterTog = 0;                
        T4CONbits.TON = 0;
        IFS1bits.U2TXIF = 1;    /*Lanzar tx*/
     }       
}

/*---------------------------------------------------------------------
  Function Name: Init_Timer4
  Description:   Initialize Timer4
-----------------------------------------------------------------------*/
void Init_Timer4( void )
{
	/* ensure Timer 4 is in reset state */
	T4CON = 0;
	T4CONbits.TCKPS = 3; //Prescaler 256

	/* reset Timer 4 interrupt flag */
 	IFS1bits.T4IF = 0;
 	
 	/* set Timer interrupt priority level */
	IPC6bits.T4IP = 6;

	/* enable Timer interrupt */
 	IEC1bits.T4IE = 1;
 	  	
	/* set Timer period register */
	PR4 = ValPR4;
	T4CONbits.TON = 0; 	

}


/*---------------------------------------------------------------------
  Function Name: _T6Interrupt
  Description:   Timer6 Interrupt Handler

-----------------------------------------------------------------------*/

void __attribute__((interrupt, auto_psv)) _T6Interrupt( void )
{
	/* reset Timer 6 interrupt flag */
 	IFS2bits.T6IF = 0;
	T6CONbits.TON = 0;	//Deshabilito Timer6

}

/*---------------------------------------------------------------------
  Function Name: Init_Timer6
  Description:   Initialize Timer6

-----------------------------------------------------------------------*/

void Init_Timer6( void )
{
	/* ensure Timer 6 is in reset state */
	T6CON = 0;
	T6CONbits.TCKPS = 3;

	/* reset Timer 6 interrupt flag */
 	IFS2bits.T6IF = 0;
 	
 	/* set Timer interrupt priority level */
	IPC11bits.T6IP = 5;

	/* enable Timer interrupt */
 	IEC2bits.T6IE = 1;
 	  	
	/* set Timer period register */
	PR6 = ValPR6;
	T6CONbits.TON = 1; //habilito Timer 
} 	    