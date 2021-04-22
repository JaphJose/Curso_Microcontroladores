#include "config1.h"
#include <libpic30.h>
#include "reloj.h"
#include "xlcd.h"
#include <stdio.h>
#include <p33FJ32MC202.h>

void ADC_Config(void);
void TIMER_Config(void);
int con;

uint16_t data;          //LEER EL ADC
float volt;  

int main(void) {
    
    // Los pines 6 y 7 van a controlar el contador
    // Declaramos como entradas
    TRISBbits.TRISB7=1;    
    TRISBbits.TRISB6=1;
    AD1PCFGL=0XFFFF;
    XLCDInit();
    
    
    WriteCmdXLCD(CLEAR_XLCD);
    XLCDgotoXY(0,0);
    printf("*** UNAC FIEE ***");
    delay_ms(200);
    WriteCmdXLCD(CLEAR_XLCD);
    XLCDgotoXY(1,0);
    printf("  ELECTRONICA");
    delay_ms(200);
    WriteCmdXLCD(CLEAR_XLCD);
    TIMER_Config();
    ADC_Config();
    
    
    while(1){
        volt = (float)(50.0/4095.0)*data;
        XLCDgotoXY(0,0);
        printf("Volti: %.2f",volt);
        XLCDgotoXY(1,0);
        printf("CONTADOR: %04d",data);
        
        delay_ms(100);
    }
 
}

void TIMER_Config(void){
    T1CON = 0;
    T1CONbits.TCKPS = 0x2;      //PSC->8
    TMR1=0;
    PR1 = 31250;                 //1s
    //INTERRUPCIONES DEL TIMER
    IFS0bits.T1IF = 0;
    IPC0bits.T1IP = 1;          //PRIORIDAD 1
    IEC0bits.T1IE = 1;          //SE ACTIVA LA INTERRUPCION POR TIMER
    
    T1CONbits.TON = 1;          //SE ACTIVA EL CONTEO
}



//RUTINA DE SERIVICIO DE LA INTERRUPCION
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void){
    
    AD1CON1bits.SAMP = 0; // Start converting
    while (!AD1CON1bits.DONE); // Conversion done?
    AD1CON1bits.DONE = 0; // Clear conversion done status bit
    data = ADC1BUF0;
    IFS0bits.T1IF = 0;
}



/*
 Configuracion ADC
 */
void ADC_Config(void){
    AD1PCFGL = 0xFFFF;      //inicialmente se desactiva todas los pines adc
    /*0. Se selecciona 10-bit o 12-bit*/
    AD1CON1bits.AD12B = 1;  //se selecciona 12 bit
    /*1. Seleccione los pines del puerto como entradas analógicas (AD1PCFGH <15: 0> o AD1PCFGL <15: 0>).*/
    AD1PCFGLbits.PCFG0 = 0; //AN0
    TRISAbits.TRISA0 = 1;   //entrada analogica
    /*2. Seleccione la fuente de referencia de voltaje para que coincida con el rango 
     * esperado en las entradas analógicas (AD1CON2 <15:13>).*/
    AD1CON2bits.VCFG = 0;   //AVDD-AVss 
    /*3. Seleccione el reloj de conversión analógico para que coincida con la 
     * velocidad de datos deseada con el reloj del procesador (AD1CON3 <7: 0>).*/
    AD1CON3bits.ADCS = 1;   //2 · TCY = TAD
    /*4. Determine cuántos canales de muestreo y retención se utilizarán 
     * (AD1CON2 <9: 8> y AD1PCFGH <15: 0> o AD1PCFGL <15: 0>).*/
    AD1CON2bits.CHPS = 0;   
    /*5. Seleccione la muestra apropiada/secuencia de conversión (AD1CON1 <7: 5> y AD1CON3 <12: 8>).*/
    AD1CON1bits.SSRC = 0;  //CONVERSION MANUAL
    AD1CON1bits.ASAM = 1;  //MUESTREO AUTOMATICO
    /*6. Seleccione la forma en que se presentan los resultados de conversión 
     * en el búfer (AD1CON1 <9: 8>).*/
    AD1CON1bits.FORM = 0;   //enteros
    /*7. SELECCIÓN DE ENTRADA ANALÓGICA PARA CONVERSIÓN*/
    AD1CHS0bits.CH0SA = 0;  // Channel 0 positive input is AN0
    AD1CHS0bits.CH0NA = 0;  //0 = Channel 0 negative input is VREF
    /*8. Encienda el módulo ADC (AD1CON1 <15>).*/
    AD1CON1bits.ADON = 1;
    /*9. Configure la interrupción de ADC (si es necesario):
        a) Borrar el bit AD1IF.
        b) Seleccione la prioridad de interrupción de ADC.*/
}

/*****************************************************************************
 *      ESTA FUNCION SIRVE PARA EL USO DE LA FUNCION PRINTF                  *
 ****************************************************************************/
int    write(int handle, void *buffer, unsigned int len)
{
  int i;
   switch (handle)
  {
      case 0:        // handle 0 corresponds to stdout
      case 1:        // handle 1 corresponds to stdin
      case 2:        // handle 2 corresponds to stderr
      default:
          for (i=0; i<len; i++)
              
              WriteDataXLCD(*(char*)buffer++);
  }
  return(len);
}

