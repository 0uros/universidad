#include "xc.h"
#define max 8

unsigned int buffer_a[max] __attribute__((space(dma)));

void initPORTD(void){
    TRISD = 0xFFFF; // Puerto D arranca como entrada
}

void initADC(void) {
    AD1PCFGL = 0xFFFC;	// RD0 y RD1 analog.   
    AD1CON1bits.FORM = 0; //unsigned int
    AD1CON1bits.SSRC = 7; //auto convert
    AD1CON1bits.ASAM = 1; //automático
    AD1CON1bits.AD12B=1; //12 bits
    AD1CON2bits.CSCNA = 1; //intercala ANx   
    AD1CSSL =  0x0003;
    AD1CSSH = 0x0000; 
    AD1CON3bits.ADCS= 63; //config tad
    AD1CON3bits.SAMC=2;   //cant tads.
    AD1CHS0bits.CH0SA = 0; 
    AD1CON1bits.ADON = 1; //ADC on
}

void initDMA(void) {
    DMA0REQbits.IRQSEL = 13; //vinculo ADC1
    DMA0PAD = (unsigned int)&ADC1BUF0;
    DMA0STA = __builtin_dmaoffset(buffer_a);;    
    DMA0CNT = max -1;
    IFS0bits.DMA0IF = 0;
    IEC0bits.DMA0IE = 1;
    DMA0CONbits.MODE = 2;
    DMA0CONbits.CHEN = 1; 
}

void config(void) {
    initPORTD();
    initADC();
    initDMA();
}
