#include "p33FJ256GP710.h"
#include "config.h"

#define max_p 10
#define max_c 5
#define ValPR1 500

char buffer_p[max_p]; //buffer de productor.
char tabla_cons[max_c]; //buffer de consumidor.
extern int counterINT0;
extern int counterTimer;
extern int punt, punt_aux, cont, nuevo_caracter;

typedef union{
    struct{
        unsigned int baja:8; // bit 0/7
        unsigned int alta:8; //bit 8/15
    }parte;
    unsigned completo:16;
}t_puertoA;
t_puertoA puertoA; //almacena en memeoria datos del PORTA

void control_productor(){
    puertoA.completo=PORTA; //deposita valores de PORTA en memoria.
    if(punt > max_p - 1)
        punt=0;//resetea el puntero para sobrescribir
    buffer_p[punt]=puertoA.parte.baja; //asigna los valores del PORTA en el buffer   
    punt++;
    nuevo_caracter=1;    
}

void control_consumidor(){                          
            while(cont != punt){
                if (cont>max_p-1)
                    cont=0;
                if(punt_aux>max_c -1)
                    punt_aux=0;
                tabla_cons[punt_aux]=buffer_p[cont]; 
                punt_aux++;
                cont++; //ingresa al while                        
            }            
            PR1=ValPR1; //reseteo espera de timer.
    nuevo_caracter=0;                                    
}

/*
 * Rutina de Atención de la interrupción externa INT0
 */
void __attribute__((interrupt, auto_psv)) _INT0Interrupt( void )
{
	/* reset INT0 interrupt flag */
 	IFS0bits.INT0IF = 0;
    counterINT0++;            
    control_productor();
    }   
/*
 * Rutina de Atención de la interrupción del Timer1
 */
void __attribute__((interrupt, auto_psv)) _T1Interrupt( void ){
 	IFS0bits.T1IF = 0;          //limpia el t1Interrupt  	
	counterTimer ++;            //Se suma uno al CounterTimer
    if(nuevo_caracter==0){      //Si el nuevo_caracter es igual a 0
        if(PR1 < 2000)
            PR1=PR1*2;       
    }
}

/*---------------------------------------------------------------------
  Function Name: Init_Timer1
  Description:   Initialize Timer1
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Init_Timer1( void )
{
    TMR1=0;
	/* ensure Timer 1 is in reset state */	
	T1CONbits.TCKPS = 1; //1//Prescaler 64.       
    
	/* reset Timer 1 interrupt flag */
 	IFS0bits.T1IF = 0;
    /*fuente de relog del temporizador*/
    
    T1CONbits.TCS = 0; //internal clock(Fcy).
    
 	/* set Timer interrupt priority level */
	IPC0bits.T1IP = 5;

	/* enable Timer interrupt */
 	IEC0bits.T1IE = 1;    
    
	/* set Timer period register */
	PR1 = ValPR1;     //PR1 cuenta ciclos de intruccion.
    
	T1CONbits.TON = 1; 	//habilito Timer    
}

void Init_INT0( void )
{
    /* reset INT0 interrupt flag */
    IFS0bits.INT0IF = 0;

    /* set INT0 interrupt priority level */
    IPC0bits.INT0IP = 5;

    /* enable INT0 interrupt */
    IEC0bits.INT0IE = 1;
    
    /*interrumpe en flanco descendente*/
    INTCON2bits.INT0EP=1;
}

void config( void )
{
    AD1PCFGL=0xFFFF;
    AD1PCFGH=0xFFFF;
    AD2PCFGL = 0xFFFF;
	//Inicialización de PORTS I/O	
	TRISA = 0xFFFF;
	LATA  = 0xFFFF;           
    /* Inicializar Interrupción Externa INT0 */
    Init_INT0();
	/* Inicializar Timers necesarios */
	Init_Timer1();

}

 