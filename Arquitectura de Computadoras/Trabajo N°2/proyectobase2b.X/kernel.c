#include <p33FJ256GP710.h>
#include "kernel.h"

#define Valor_PR 300
#define Max_Quantum 2 //quantum
#define Cant_Val 8//cant. posiciones/estados de procesos a resguardar.
#define PILA_TIMER 18 //cant. posiciones de pila de timer y planificador.
#define Posicion_PC 0 //posición de cada arreglo que almacena PC.

unsigned int direB,direC,quantum;
unsigned int estadoA[Cant_Val],estadoB[Cant_Val],estadoC[Cant_Val]; //buffers por proceso
char estado; //flag de proceso.

void init(void)
{
    quantum = 0;
    estado = 'a'; 
    estadoB[0]= direB; 
    estadoC[0]= direC; 
}

void confReloj(void)
{
    /* ensure Timer 1 is in reset state */
	T1CONbits.TON = 0;
    
    
    T1CONbits.TCS = 0; // Reloj interno. Frecuencia de instrucción: 40MHz
	T1CONbits.TCKPS = 0; //Prescaler 1:8

	/* reset Timer 1 interrupt flag */
 	IFS0bits.T1IF = 0;

 	/* set Timer interrupt priority level */
	IPC0bits.T1IP = 5;

	/* enable Timer interrupt */
 	IEC0bits.T1IE = 1;

	/* set Timer period register */
	TMR1 = 0;
    PR1 = Valor_PR;
     
	T1CONbits.TON = 1; 	//habilito Timer
}


void boot(void)
{
    init();
    confReloj();
    return;
}

void planificador(void) 
{     
    unsigned int *puntero_pila = WREG15;
    puntero_pila = puntero_pila - PILA_TIMER;
    switch(estado)
    {
        int i;
        case'a':                      
            for(i=0;i<Cant_Val;i++)
            {
               estadoA[i]= *puntero_pila;           
               *puntero_pila = estadoB[i];
               puntero_pila++;           
            }
         estado++; 
         break;
        case 'b':        
            for(i=0;i<Cant_Val;i++)
            {
               estadoB[i]= *puntero_pila;           
               *puntero_pila = estadoC[i];
               puntero_pila++;           
            }
            estado++; 
            break;
        case'c':        
            for(i=0;i<Cant_Val;i++)
            {
               estadoC[i]= *puntero_pila;           
               *puntero_pila = estadoA[i];
               puntero_pila++;           
            }
            estado='a';  
            break;
    }
}






void __attribute__((interrupt, auto_psv)) _T1Interrupt( void )
{
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 0;  //desactiva timer
    quantum++;
    if (quantum==Max_Quantum){ //cada 2 interrup. cambia proceso
        quantum=0;
        planificador();
    }
    TMR1 = 0;
    T1CONbits.TON = 1;  //activa timer
}
