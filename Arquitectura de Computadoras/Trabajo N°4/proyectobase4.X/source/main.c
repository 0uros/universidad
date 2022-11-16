#include "common.h"
#include <xc.h>
#include "confbits.h"
#include "p33FJ256GP710.h"
#include "config.h"

#define TRUE	1

    int counterTog = 0;

    void UpdateClock (void)
    {
        counterTog = 0;
    
        /* Actualizar Hora */
         seconds=seconds+1;
        if (seconds > 59)
        {
            seconds=0;
            minutes=minutes+1;
        }
        if (minutes > 59)
        {
            minutes=0;
            hours=hours+1;
        }
        if (hours == 24)
        {
            hours=0;
        }      
    }

int main ( void )
{
    config();
    /* Loop infinito */
    while ( TRUE )
    {        
        if (counterTog > 3)
           UpdateClock();  /*Actualizar hora del dispositivo*/
    }    
}


