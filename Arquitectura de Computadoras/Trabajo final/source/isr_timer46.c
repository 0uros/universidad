#include "p33FJ256GP710.h"
#include "config.h"
//#define ValPR1	50000
#define ValPR4	39062
#define ValPR6	5000


extern int counterTog, contVel;
extern int num;

/*
counterTog = Variable usada en T4Interrupt
contVel = Variable usada en T6Interrupt
num = ???
*/


void __attribute__((interrupt, auto_psv)) _T4Interrupt( void )
{
	/* reset Timer 4 interrupt flag */
 	IFS1bits.T4IF = 0;
	//T4CONbits.TON = 0; 	//Deshabilito Timer
	counterTog ++;
    if (counterTog > 3)
            UpdateClock();  //Actualizar hora del dispositivo

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
	T4CONbits.TON = 1; 	//habilito Timer
    
    /*Se usa el timer para contar cada 1 segundo*/
    
}




/*---------------------------------------------------------------------
  Function Name: _T6Interrupt
  Description:   Timer6 Interrupt Handler

-----------------------------------------------------------------------*/
void __attribute__((interrupt, auto_psv)) _T6Interrupt( void )
{
	/* reset Timer 6 interrupt flag */
 	IFS2bits.T6IF = 0;          //actúa como contador de velocidad
	contVel++;

}

/*---------------------------------------------------------------------
  Function Name: Init_Timer6
  Description:   Initialize Timer6

-----------------------------------------------------------------------*/
void Init_Timer6( void )
{
	
	/* ensure Timer 6 is in reset state */
	T6CON = 0;
	T6CONbits.TCKPS = 1; //Prescaler 8

	/* reset Timer 6 interrupt flag */
 	IFS2bits.T6IF = 0;
 	
 	/* set Timer interrupt priority level */
	IPC11bits.T6IP = 5;

	/* enable Timer interrupt */
 	IEC2bits.T6IE = 1;
 	  	
	/* set Timer period register */
	PR6 = ValPR6;

    /*Timer para contar la distancia recorrida*/

}
