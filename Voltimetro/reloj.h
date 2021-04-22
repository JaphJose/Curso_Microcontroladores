#ifndef __RELOJ_H
#define __RELOJ_H

#define FCY         (10000000UL/2)                 // esto es igual a: Fcy = Fosc/2
#define delay_ms(x) __delay32((FCY/1000)*x) //macro para generar retardos en milisegundos

#endif /* __RELOJ_H*/
