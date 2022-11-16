#include "xc.h"
#include "config.h"

#define buffer_cargado 2
#define max 8
#define mitad_max 4

typedef union 
{ 
    struct 
    { 
        unsigned int:6;
        unsigned int pulsador_mayor:1;  //RD6
        unsigned int pulsador_promedio:1; //RD7
        unsigned int:5;
        unsigned int pulsador_menor:1; //RD13
        unsigned int:2;
    };
    unsigned int completo;
}t_puerto; 
t_puerto puerto_d;

extern unsigned int buffer_a[max];
unsigned int buffer_aux[max];
int an, y;
unsigned int menor_an0, menor_an1,      //declaramos variables
mayor_an0, mayor_an1, 
prom_an0, prom_an1,
valores, buffers_listos;

void __attribute__((interrupt, auto_psv)) _DMA0Interrupt(void) 
{       
    IFS0bits.DMA0IF = 0;      
    if(an==0) //posiciones impares para AN0
    {
        for(y=0;y <= max-1;y++)    
        {               
            buffer_aux[y] = buffer_a[y];                                                                  
            y++;

        }       
        buffers_listos = buffers_listos + 1;
    }
    else //posiciones pares para AN1
    {
        for(y=1;y <= max-1;y++)    
        {               
            buffer_aux[y] = buffer_a[y];                                                                  
            y++;
        }       
        buffers_listos = buffers_listos + 1;
    }
    an^= 1;     
}    
    
void init(void)
{
    an=0;
    buffers_listos = 0;    
    menor_an0 = 0; 
    menor_an1 = 0; 
    mayor_an0 = 0;
    mayor_an1 = 0;
    prom_an0 = 0;
    prom_an1 = 0;      
}

void logica_an0(void)
{
    unsigned int y;
    /*compara la primera posición*/
    y = 0;
    menor_an0 = buffer_aux[y]; 
    mayor_an0 = buffer_aux[y];     
    prom_an0 = 0;
    for(y = 0; y <= max-1; y++)
    {
        if (buffer_aux[y] <= menor_an0)
        {
            menor_an0 = buffer_aux[y];
        }
        else        
        {
            mayor_an0 = buffer_aux[y];
        }    
        prom_an0+=buffer_aux[y];            
        prom_an0 = prom_an0/mitad_max;      
        y++;
    }    
}

void logica_an1(void)
{
    unsigned int y = 1;   
    menor_an1 = buffer_aux[y]; 
    mayor_an1 = buffer_aux[y];         
    for(y = 1; y <= max-1; y++)
    {
        if (buffer_aux[y] <= menor_an1)
        {
            menor_an1 = buffer_aux[y];
        }
        else        
        {
            mayor_an1 = buffer_aux[y];
        }        
        prom_an1+=buffer_aux[y];            
        prom_an1 = prom_an1/mitad_max;      
        y++;
    }
}


int main(void) {
    init();
    config();    
    while(1) 
    {
        if (buffers_listos == buffer_cargado)
        {
            buffers_listos = 0;            
            logica_an0();  //calculo AN0
            logica_an1(); //calculo AN1
        }  
        puerto_d.completo= PORTD;       
        if (puerto_d.pulsador_mayor) //RD6
        {
            TRISD = 0x0000;
            PORTD= mayor_an0;
            PORTD= mayor_an1;
            TRISD= 0xFFFF;
        }
        if (puerto_d.pulsador_promedio) //RD7
        {
            TRISD = 0x0000;
            PORTD= prom_an0;
            PORTD= prom_an1;
            TRISD= 0xFFFF;
        }         
         if (puerto_d.pulsador_menor) //RD13
        {
            TRISD = 0x0000;
            PORTD= menor_an0;
            PORTD= menor_an1;
            TRISD= 0xFFFF;
        }
    }       
    return 0;
}
