/*
 * File:   main.c
 * Author: juanp
 *
 * Created on 22 de marzo de 2022, 19:10
 */


#include "xc.h"
#include <p33FJ256GP710.h>


    
int Suma_Acc; // variable de los accesos
int temp; // variable del termostato 
int sald; // variable de salida

typedef union {
	struct {
		unsigned int Acceso:5;  // entrada
		unsigned int Gas:1;     // entrada
		unsigned int Energia:1; // entrada
		unsigned int Motor:1;   // salida
        unsigned int Valvula:1; // salida
		unsigned int :7; 
    }Cond;
    struct{
		unsigned int A1:1;      // estructura de los accesos individuales 
		unsigned int A2:1;
		unsigned int A3:1;
		unsigned int A4:1;
		unsigned int A5:1;
	}Acc;
	unsigned int completo;
	} t_valv;

t_valv ctrl;

void configuracion (void){ 
TRISD = 0x007F; // puerto D
TRISB = 0xFFFF; // Puerto del termostato 
ctrl.Cond.Motor = 0; // inicio del programa con el motor y la valvula apagados
ctrl.Cond.Valvula = 0;
}
	
void lectura(void) {
		ctrl.Acc.A1 = PORTDbits.RD0;
        ctrl.Acc.A2 = PORTDbits.RD1;
        ctrl.Acc.A3 = PORTDbits.RD2;
        ctrl.Acc.A4 = PORTDbits.RD3;
        ctrl.Acc.A5 = PORTDbits.RD4;
        ctrl.Cond.Gas = PORTDbits.RD5;
        ctrl.Cond.Energia = PORTDbits.RD6;
        
		Suma_Acc = ctrl.Acc.A1 + ctrl.Acc.A2 + ctrl.Acc.A3 + ctrl.Acc.A4 + ctrl.Acc.A5;
} 

void Prog2(void) {
    temp = PORTB; //Copia el valor del termostato/variable para simular con stimulus
		if ((temp < 0x3852)&&(Suma_Acc > 2)&&(ctrl.Cond.Energia == 1)&&(ctrl.Cond.Gas == 1)) // Condiciones para que activar la salida   
		{
			sald = 1; // variable de salida llamada de salida 
		}
		else
		{
			sald = 0;
        }		
}
// la doble verificacion es para que si estan apagado o prendido y no hace falta cambiar de estado, queden como estan	
void salida (void) {
		if ((sald == 1) && (ctrl.Cond.Motor == 0) && (ctrl.Cond.Valvula == 0)) // Condiciones para que se prenda o apague el motor y la valvula     
		{
			ctrl.Cond.Motor = 1; //si hay requisistos para prender el motor y el motor esta apagado lo prende, si no procede
            ctrl.Cond.Valvula = 1;
            PORTDbits.RD7 = 1;
            PORTDbits.RD8 = 1;
		}
		if ((sald == 0) && (ctrl.Cond.Motor == 1) && (ctrl.Cond.Valvula == 1)) 
		{
			ctrl.Cond.Motor = 0; //si hay requisistos para apagar el motor y el motor esta prendido lo apaga, si no procede
            ctrl.Cond.Valvula = 0;
            PORTDbits.RD7 = 0;
            PORTDbits.RD8 = 0;
        }		
}



int main(void) {    
    
    AD1PCFGH = 0xFFFF;
    AD1PCFGL = 0xFFFF;
    
	configuracion ();
    
	while(1)
    {
        lectura();
        
        Prog2();
        
        salida(); 
	}
}