
/**********************************************************************
 FileName:        main.c
 Dependencies:    p33FJ256GP710.h
 Processor:       dsPIC33F
 Compiler:        MPLAB® C30 v2.01 or higher

 Ejemplo de funcionamiento de:
 	Timer1
 	LCD
 	PORT A Output
 	PORT D Input

 REVISION HISTORY:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Author            Date      Comments on this revision
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Richard Fischer   07/14/05  Initial Release
 Priyabrata Sinha  01/27/06  Ported to non-prototype devices 
 Ricardo López     05/24/10  Adaptación para uso general
 Sebastián Wahler  08/05/13  Nuevas adaptaciones
 Marcelo Gómez     09/03/14  Portado a MPLAB.X
 Sebastián Wahler  04/06/19  Simplificado sin Placa

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**********************************************************************/
#include "common.h"
#include <xc.h>
#include "confbits.h"
#include "p33FJ256GP710.h"
#include "config.h"



t_vehiculo auxV[MAX];				//capaAplicacion 
t_vehiculo trafico[MAX_V] = {};		//resetV
tHora hora;							//updateClock + main
int qty, checksum, sum, cant,k, in, horaMax, horaMin, cantV=0, counterTog=0 , hayVehiculo=0, contVel=0, estadoTrama=0;
/* Variables usadas en main.c
qty = Se utiliza en armarPaq para contar la cantidad de datos a enviar
checksum = placeholder para almacenar el dato de checksum al enviar paquete
sum = variable para modificar datos en cargarVehiculos
cant = Se utiliza en capaAplicacion para como indice para las listas, y en cargarVehiculos para agregar minutos
k = Se utiliza en capaAplicacion como variable para el caso C, y en cargarVehiculos para setear la cantidad de ejes que tiene un vehiculo
in = Se usa como indice para cargar listas de vehiculos en armarPaq y en capaAplicacion, en cargarVehiculos se utiliza para setear la velocidad
cantV = Define la cantidad de vehiculos que se cargan en cargarVehiculos, se utiliza en el resto de operaciones
counterTog = Se utiliza para el clock
hayVehiculo = Se usa en CNinterrupt para definir si hay o no un vehiculo
contVel = se usa en cninterrupt y en calcularVelocidad
estadoTrama = se usa para ver si la trama ha terminado
*/

void resetV(void){
    for (in=0;in<cantV;in++){
        trafico[in].hora.hour = 0;
        trafico[in].hora.min = 0;
        trafico[in].hora.seg = 0;
        trafico[in].velocidad = 0;
        trafico[in].cantEjes = 0;
    }
    cantV = 0;
}

/*resetV
in = indice iniciado en 0 para borrar los datos que haya en la lista de tráfico, borrará según cantV
trafico = lista de vehiculos cargados en cargarVehiculos
*/


void armarPaq(char arg, char com){
    
    /*Funcion que arma el paquete nuevo para enviar información*/
    
    in=0;
    transmit[SOF]=VAL_SOF;      //Guardas FE en la primera posición
    transmit[DST]=DIR_PC;       //Guardas la dirección de destino
    transmit[SRC]=DIR_PLACA;    //Guardas la dirección de fuente
    transmit[SEC]=VAL_SEC;      //Valor entre 40 y 80, dejas en 80
    transmit[COM]=com;          //Comando que llegó
    qty = 6;                    //se asume que son 6 por los datos ingresados anteriormente, + QTY que se sumará después.
    switch (com){
        case 'A':
            transmit[qty] = arg;
            break;
        case 'C':
            transmit[qty]=arg;
            break;
        case 'D':
            if (cant > 0){
                for(in=0;in<cant;in++){                 //Pasa todos los datos de los autos constantemente para enviar.
                    transmit[qty]= auxV[in].hora.hour; 
                    qty++;
                    transmit[qty]= auxV[in].hora.min; 
                    qty++;
                    transmit[qty]= auxV[in].hora.seg; 
                    qty++;
                    transmit[qty]= auxV[in].velocidad; 
                    qty++;
                    transmit[qty]= auxV[in].cantEjes; 
                    qty++; 
                }  
                qty--; 
            }
            break;
        case 'H':
            if (cant > 0){
                for(in=0;in<cant;in++){                 //Pasa todos los datos de los autos constantemente para enviar.
                    transmit[qty]= auxV[in].hora.hour; 
                    qty++;
                    transmit[qty]= auxV[in].hora.min; 
                    qty++;
                    transmit[qty]= auxV[in].hora.seg; 
                    qty++;
                    transmit[qty]= auxV[in].velocidad; 
                    qty++;
                    transmit[qty]= auxV[in].cantEjes; 
                    qty++; 
                }  
                qty--; 
            }
            break;
        case 'I':
            transmit[qty] = arg;
            break;
            
        default: 
            transmit[COM]=arg; 
            qty--; 
            break; 
    }
    qty++;
    transmit[QTY]=qty+2;                //al valor del QTY le sumamos el checksum
    checksum = calcularChecksum(qty);   //se genera el checksum
    
    transmit[qty]= checksum>>8;          //Se asigna los dos primeros caracteres del checksum (BCCH)
    qty++; 
    transmit[qty]= checksum;            //se asigna checksum a la posición (BCCL)
}   

/*armarPaq
in = se pone en cero porque se usa en funciones anteriores (se puede cambiar)
transmit = lista en la que se almacenará lo que se enviará
qty = la cantidad de información que se enviará, depende de qué se vaya a realizar, puede cambiar
arg = ???
com = comando a enviarse
*/


int calcularVelocidad(int contVel){
    if (contVel != 0)
        return (DIST_CABLES*KMxH)/contVel; 
    else
        return 0;
}

/*calcularVelocidad
Simplemente verifica que el contador de velocidad sea distinto de 0, para hacer el calculo.*/

void accionarCamara(void){
    PORTAbits.RA0 = 1;
    //retardo
    PORTAbits.RA0 = 0;
}

/*accionarCamara
Se acciona la cámara y luego se apaga*/

void capaAplicacion(char com){
    cant=0;
    sum =0;    
    switch (com){
            case 'A':
                /*Consultar cantidad de vehiculos hasta el momento*/
                armarPaq(cantV,com);
                break;
                
            case 'B':
                /*Resetear cantidad de vehiculos y borrar registros*/
                resetV();
                armarPaq('F', com);
                break;   
            case 'C': 
                /*Consultar la cantidad de vehiculos con más de dos ejes*/
                for (k=0;k<cantV;k++){
                    if (trafico[k].cantEjes > 2)
                        sum = sum++;    
                }
                armarPaq(sum,com);
                break;
                
            case 'D':
                /*Consulta detallada de vehiculos que pasaron en una determinada hora*/
                k = recive[COM+1];              //se guarda lo que está en la posición del comando + 1, o sea, el dato del comando
                horaMax = recive[COM+2];        //se guarda la hora máxima que se envió
                for (in=0;in<cantV;in++){       //se recorre la cantidad de vehiculos desde 0 hasta el tope
                    if (trafico[in].hora.hour == k){        //Si tráfico (arreglo) en la posición x, en la hora, es igual a... entonces... 
                        auxV[cant].hora = trafico[in].hora;     //arreglo auxiliar para guardar la información
                        auxV[cant].velocidad = trafico[in].velocidad;
                        auxV[cant].cantEjes = trafico[in].cantEjes;
                        cant++;
                    }                        
                }
                armarPaq(cant, com);
                break;
                
            case 'E':
                /*Accionar camara fotografica*/
                if (PORTAbits.RA0 == 1)
                    PORTAbits.RA0 == 0;
                else
                    PORTAbits.RA0 == 1;
                armarPaq('F', com);
                break;
                
            case 'F':
                /*ACK*/
                armarPaq('F', com);
                break;
                
            case 'G':
                /*NACK*/
                armarPaq('G', com);
                break;
			
			case 'H':
				/*Consulta detallada de vehiculos que pasaron entre dos horas*/
                horaMin = recive[COM+1];              //se guarda lo que está en la posición del comando + 1, o sea, el dato del comando
                horaMax = recive[COM+2];        //se guarda la hora máxima que se envió
                for (in=0;in<cantV;in++){       //se recorre la cantidad de vehiculos desde 0 hasta el tope
                    if (trafico[in].hora.hour >= horaMin){        //Si tráfico (arreglo) en la posición x, en la hora, es igual a... entonces... 
                        if (trafico[in].hora.hour <= horaMax){
                            auxV[cant].hora = trafico[in].hora;     //arreglo auxiliar para guardar la información
                            auxV[cant].velocidad = trafico[in].velocidad;
                            auxV[cant].cantEjes = trafico[in].cantEjes;
                            cant++;
                        }
                    }                        
                }
                armarPaq(cant, com);
				break;
				
			case 'I':
				/*Consultar la cantidad de vehiculos que superaron la velocidad máxima de 60km/h*/
                for (k=0;k<cantV;k++){
                    if (trafico[k].velocidad > 60)
                        sum = sum++;    
                }
                armarPaq(sum,com);
				break;
    }
    comTx = TRUE;
    estadoTrama = FALSE;            //terminamos de armar la trama
    cant=0;
    IEC1bits.U2RXIE = 0;            //Apago el U2RXREG
    IFS1bits.U2TXIF = 1;            //Se fuerza la transmisión de la trama
    
	
}

/*capaAplicacion
cant = se setea en cero para volver a utilizarlo como indice para auxV
sum = Se setea en cero para usarlo como contador en case C
*/


void __attribute__((interrupt, auto_psv)) _CNInterrupt( void ) {
    
    IFS1bits.CNIF = 0;              //se desactiva el change notification por apretar el RD13
    if ((PORTDbits.RD13)&(hayVehiculo == FALSE)){
        hayVehiculo = TRUE;
        trafico[cantV].hora = hora;
        }             
    else{
        if ((PORTDbits.RD13 == FALSE)&(hayVehiculo==TRUE)){
            hayVehiculo = FALSE;								//de esta forma, ya está en la siguiente posición para el siguiente vehiculo
            cantV++;
            }
        }    
    

    if ((PORTDbits.RD6)&(hayVehiculo)){
        //PORTDbits.RD6 = 0;
        trafico[cantV].cantEjes = trafico[cantV].cantEjes + 1;
        if (trafico[cantV].cantEjes == 1)
            T6CONbits.TON = 1; 	//habilito Timer para calcular la velocidad usando RD6 y RD7 
        }
        
        
        
    
    if ((PORTDbits.RD7)&(hayVehiculo)){
            //PORTDbits.RD7 = 0;
            if (T6CONbits.TON == 1)
                T6CONbits.TON = 0;	
            if (trafico[cantV].cantEjes == 1){          //ya estaría tocando RD7, por lo que hay que calcular la velocidad
                trafico[cantV].velocidad = calcularVelocidad(contVel);
                if (trafico[cantV].velocidad > 60){
                    accionarCamara();
                }
            }
    }
}

/*_CNInterrupt
hayVehiculo = Se activa en true cuando hay un vehiculo
*/

void cargarVehiculos (void){
    
    /*Se utiliza un arreglo para cargar los vehiculos*/
    k = 1;
    in = 63;
    cant=10;
    sum=0;
    for (cantV=0;cantV <6;cantV++){         //cantV es la cantidad de vehiculos y la posición en la lista, lo setea en cero para luego ir de 0 a 6
    trafico[cantV].hora.hour = 13+sum;
    trafico[cantV].hora.min = cant;
    trafico[cantV].hora.seg = cant;
    trafico[cantV].velocidad = in;
    trafico[cantV].cantEjes = k + 1;
    cant = cant+5;
    if (cantV==1){
        k++;
        in=58;
        sum++;
    }
    }
    
}


void UpdateClock (void)
{
    counterTog = 0;
    hora.seg = (hora.seg +1);
    if (hora.seg > 59)
    {
        hora.seg=0;
        hora.min = (hora.min+1);
    }
    if (hora.min > 59)
    {
        hora.min=0;
        hora.hour=(hora.hour+1);
    }
    if (hora.hour == 24)
    {
        hora.hour=0;
    }

}


int main ( void )
{
    hora.hour = 0;
    hora.min = 0;
    hora.seg = 0;
    config();
    cargarVehiculos();      //Cargamos vehículos con valores simples

    /* Loop infinito */
    while ( TRUE )
    {
        if (estadoTrama == RECIBIDO)
            capaTransporte();
    }
}


