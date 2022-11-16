/* 
Ejercicio multitasking con interrupciones.
 */


#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include "kernel.h"

extern unsigned int direB, direC;

void procesoA(void)
{
    int x,y,z;

    x = 2;
    y = 4;
    while(1) {
        z = x + y;
        x = x + 2;
        y = z;
    }
}

void procesoB(void)
{
    int a,b,c;

    a = 1;
    b = 3;
    while(1) {
            c = a * b;
        a = a + 2;
    }
}

void procesoC(void)
{
    int r,s,t;

    r = 1;
    s = r;
    while(1) {
        t = r + s;
        t >>= 1;
        r = r + 1;
        s = s + 4;
    }
}

void guardado (void){ //obtiene dirección de las funciones.
    direC=procesoC;
    direB=procesoB;
}

int main(int argc, char** argv) {
    guardado();
    boot();
    procesoA();
    return (EXIT_SUCCESS);
}
