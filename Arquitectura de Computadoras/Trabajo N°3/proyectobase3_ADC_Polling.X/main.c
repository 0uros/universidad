#include <p33FJ256GP710A.h>
#define suma 3
#define espera_sam 300

int sum, sum_an2, sum_an5, sum_an6, val_ADC1;       //declaramos variables
unsigned int x, i, z;

unsigned int buffer_an2[suma],  buffer_an6[suma];
int buffer_an5[suma], buffer_x[suma];

typedef union {
    struct {
        unsigned int : 6;
        unsigned int pulsador_6 : 1;
        int pulsador_7 : 1;
        unsigned int : 5;
        unsigned int pulsador_13 : 1;
        unsigned int : 2;
    };
    unsigned int completo;
} port_D;
port_D puerto_d;

void config(void) 
{
    AD1PCFGL = 0xFF9B; // configuracion del puerto AN2, AN5, AN6.
    AD1CON1 = 0; //convertidor automatico.    
    AD1CON2 = 0;
    AD1CSSL = 0;
    AD1CON3 = 0x0002;    //(ADCS<7:0>+1)=3 Tcy=Tad
    AD1CON1bits.ADON = 1;

    TRISD = 0xFFFF;
    x = 0;  
    
    sum_an2 = 0;
    sum_an5 = 0;
    sum_an6 = 0;
    sum = 0;
}

int convertidor(unsigned int pulsador, unsigned int ANx, unsigned int ad12b, unsigned int form, unsigned int buffer_x[x])
{      
    sum=0;
    PORTD = pulsador;
    AD1CHS0 = ANx;
    AD1CON1bits.AD12B= ad12b;
    AD1CON1bits.FORM = form;   
    if(x == suma) //buffer circular                           
        x = 0;                             
    for (i = 0; i < suma; ++i)
        {                   
            AD1CON1bits.SAMP = 1; //inicio sampleo
            for (z = 0; z < espera_sam; ++z);
            AD1CON1bits.SAMP = 0; //inicio de conversion
            while(!AD1CON1bits.DONE);                       
            val_ADC1=ADC1BUF0;
            buffer_x[i] = val_ADC1;
            sum += buffer_x[x];    
            x++;
        }       
    return sum;    
}

int main(void) {
    config();
    while (1) {
    puerto_d.completo = PORTD;  
    if(puerto_d.pulsador_6)
        sum_an2 = convertidor(puerto_d.pulsador_6, 0010, 0, 0, buffer_an2);    
    if(puerto_d.pulsador_7)
        sum_an5 = convertidor(puerto_d.pulsador_7, 0101, 1, 1, buffer_an5);    
    if(puerto_d.pulsador_13)
        sum_an6 = convertidor(puerto_d.pulsador_13, 0110, 1, 0, buffer_an6);        
    } 
}
