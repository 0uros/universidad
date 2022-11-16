#include "p33FJ256GP710.h"
#include "config.h"

int punt, punt_aux, cont, nuevo_caracter;       //Asignacion de variables
int counterTimer, counterINT0 = 0;
extern void control_consumidor();

void iniciar(){
    nuevo_caracter=0;
    cont=0;
    punt=0;
    punt_aux=0;                
}

/*
 * Programa Principal
 */
int main(void) {
    iniciar(); //Iniciar variables enteras
    config();

    while(TRUE)
    {        
        if (counterTimer > 9 )          //si counterTimer es mayor a nueve...
        {
            LATA = counterINT0;
            counterTimer = 0;
            counterINT0 = 0;                        
        }  
        if (nuevo_caracter==1) //enciende flag de ingreso de caracter.
            control_consumidor();        
    }   
}