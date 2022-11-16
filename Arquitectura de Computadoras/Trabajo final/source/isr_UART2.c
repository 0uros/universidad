/* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Ricardo López    11/6/10  UART2C module interrupt handler
* Marcelo Gómez    09/03/14 Portado a MPLAB.X
* Marcelo Gómez    20/05/15 Portado a MPLAB.X
* Sebastián Wahler 04/06/19 Simplificado
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
*
**********************************************************************/
#include "config.h"
#include "common.h"
#include "p33FJ256GP710.h"

/* Variables usadas en isr_UART2.c
comTrama = Se utiliza en U2RXInterrupt para declarar el comienzo de la trama
comTx = se verifica que esté en uno para un procedimiento en _U2TXInterrupt
i = Indice para los U2 interrupt
j = Indice para los checksum
comando = variable de capaTransporte
uart_update = ???
recive = Lista para guardar lo que entre por señal
transmit = Lista para enviar información
aux = variable auxiliar usado en Checksum
*/


char recive[MAX] = {}, transmit[MAX]={};
int comTrama= 0, comTx=0,j,i=0, comando;   
volatile unsigned char uart_update;
int aux;



int calcularChecksum(int sum){
    checksum = 0;
    for (j=0;j<sum;j++){
        aux = transmit[j]<<8;
        j++; 
        if (j != sum)           
            aux = aux + transmit[j];
        else
            j--;
        checksum = checksum + aux;          
    }
    return checksum;
}

int verificarChecksum(void){
    
    /*En la variable aux se guardará los bits de información de a dos, desplazando 8 bits el primero para que 
     entre el segundo en los otros 8 bits, de esta forma, se sumarán hasta que sean todos los bits de información
     y se compara con el checksum enviado en el mensaje*/

    checksum = 0;
    sum = recive[QTY]-2;        //Resta dos a la cantidad total de elementos que tiene la trama, para omitir el checksum de la suma
    for (j=0;j<sum;j++){        //desde una variable inicializada en cero, hasta la cantidad de elementos menos el checksum.
        aux = recive[j]<<8;     // 0x0056 -> 0x5600. Se corre 8 bytes a la izquierda
        j++; 
        if (j != sum)           //Para verificar los números pares o impares
            aux = aux + recive[j]; //se suma la siguiente posición, quedando 0x5602 por ejemplo
        else
            j--;
        checksum = checksum + aux;          
    }
    aux=recive[j]<<8;
    aux += recive[j+1];         //se agrega el checksum que se recibió en la variable aux
    if(checksum == aux)         //Se verifica que el checksum sumado sea igual que el recibido
        return CORRECTO;
    else
        return INCORRECTO;   
}








void capaTransporte(void){
    
    if ((recive[DST] == DIR_PLACA)&&(recive[SRC] == DIR_PC)){       //Si en el arreglo de la trama que recibí, el destinatario es la placa, y la fuente es la PC, entonces...
        if (verificarChecksum()){                                   //si el checksum es correcto, entrará
            comando = recive[COM];                                  //se asigna a la variable comando, el valor recibido en la trama
            capaAplicacion(comando);
        }
        else
            capaAplicacion('G');}
    else
        capaAplicacion('G');

}
/*---------------------------------------------------------------------
  Function Name: UART2Interrupt
  Description:   UART2 Interrupt Handler
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
//Interrupciones de la UART2

void __attribute__((interrupt, auto_psv)) _U2RXInterrupt( void )
{
        IFS1bits.U2RXIF = 0;            //Breakpoint aca, interrumpe cuando U2RXREG recibe un dato (normalemnte FE)
        if (U2RXREG == VAL_SOF)         //Verifica que el valor ingresado sea FE
            comTrama = 1;               //Declara comienzo de la trama, levanta bandera
        if (comTrama == 1){             //Si se comienza la trama, se ingresa al if
            recive[i] = U2RXREG;        //Deja FE y el resto de valores en la primera posición y siguientes de la cadena
            if (i == QTY)               //la primera vez será la posición cero, por lo que la segunda pasada lo hará, sirve para saber la cantidad de caracteres que llegarán
                cant = recive[i];
            i++;                      
        }
        if (i==cant){
            comTrama = 0;               //Se setea a cero porque se termina la trama
            i=0;                        //Se setea a cero para utilizarlo después
            cant=0;                     //idem variable i
            estadoTrama = RECIBIDO;     //Recibido es una constante inicializada en 1
        }
        
    }

//Rutina de INT para transmisión (no hace nada)
void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void)
{
	//Atención. Se debe modificar para que no emita reiteradamente
	//caracteres
    IFS1bits.U2TXIF = 0;
    
    if(comTx == 1)
    {
        if (cant == 0){
            cant = transmit[QTY];           //Se agrega el valor de QTY a cant, que es el valor total de los caracteres a enviar
            i=0;}
        U2TXREG = transmit[i];              //Transmite los caracteres
        --cant;
        i++;
        if (cant== 0){
            comTx = 0; 
            i=0;
            IEC1bits.U2RXIE = 1;            //habilito U2RXREG
            
        }
    }
}

/*---------------------------------------------------------------------
  Function Name: InitUART2
  Description:   Inicializar UART2
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void InitUART2(void)
{
	// The HPC16 board has a DB9 connector wired to UART2, 
	// so we will be configuring this port only
	// configure U2MODE
	U2MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	U2MODEbits.RTSMD = 1;	// Bit11 Simplex Mode

	// Load a value into Baud Rate Generator.  Example is for 9600.
	U2BRG = BRGVAL;	// 40Mhz osc, 9600 Baud

	IPC7 = 0x4400;	// Mid Range Interrupt Priority level, no urgent reason

	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC1bits.U2RXIE = 1;	// Enable Recieve Interrupts

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
	U2STAbits.UTXEN = 1;	// Empieza a transmitir. Se dispara el Flag TXIF

	IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
	IEC1bits.U2TXIE = 1;	// Enable Transmit Interrupts
}


void InitCN(void)
{

    IEC1bits.CNIE = 1;
    IPC4bits.CNIP = 5;      //configuración del change notifiaction
    CNEN1bits.CN15IE = 1;
    CNEN2bits.CN16IE = 1;
    CNEN2bits.CN19IE = 1;
}