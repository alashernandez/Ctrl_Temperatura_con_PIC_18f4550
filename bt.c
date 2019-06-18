#include <xc.h> 
#include <plib/adc.h> 
#include <plib/delays.h> 
#include "stdlib.h" 
#include <plib/xlcd.h> 
#include <stdio.h>
#include "USART_Header_File.h"
#include "I2C.h"
 
//Bits de configuración para Fosc = 48Mhz 
#pragma config PLLDIV = 5, CPUDIV = OSC1_PLL2, USBDIV = 2 
#pragma config FOSC = HSPLL_HS, FCMEN = OFF, IESO = OFF 
#pragma config PWRT = OFF, BOR = OFF, VREGEN = OFF 
#pragma config WDT = OFF, WDTPS = 32768 
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = OFF 
#pragma config STVREN = ON, LVP = OFF, ICPRT = OFF, XINST = OFF 

#define _XTAL_FREQ 48000000

void retardo(int v);
void fan(char c);
void writeEEPROM(unsigned int address, unsigned char data);
unsigned char readEEPROM(unsigned int address);
unsigned char getValue();
unsigned char value;

char temp[5];               //Variable tipo char donde se almacenara la temperatura la cual se enviara para que se muestre en la App
float resultado;            //variable que almacena el valor que se lee en ADC

void main()
{
    OSCCON=0x72;            //frecuencia interna del oscilador que se establece en MHz
                                 
                                 
    char data_in;           //variable en la que se almacena el dato del bluetooth
    TRISD = 0;              //puerto D configurado como salida
    PORTD=0;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;
    PORTAbits.RA3 = 0;
    USART_Init(9600);       //baudrate tasa de transferencia del modulo bluet00th
    
    OpenADC(                //configuracion ADC trabajamos con canal analogico 0
            ADC_FOSC_64 &             
            ADC_RIGHT_JUST &             
            ADC_16_TAD,             
            ADC_CH0 &             
            ADC_INT_OFF &             
            ADC_VREFPLUS_VDD &             
            ADC_VREFMINUS_VSS,             
            14); 
    
    I2CSetup(100000);  //Start bus speed at 100Khz
 
    //Retardo de 50 Tcy     
    Delay10TCYx(5); 
    
    while(1)
    {
        //Iniciar la conversión         
        ConvertADC(); 
 
        //Espera para que se complete la conversión         
        while(BusyADC()); 
        resultado = ReadADC()/2.04;
        sprintf(temp, "%.2f",resultado);    
        USART_SendString(temp);
        retardo(30);
        
        if(resultado>=40){                      //condicion en la se activan los 3 fan al llegar a una temperatura critica 
            LATAbits.LATA1 = 1; 
            LATAbits.LATA2 = 1;
            LATAbits.LATA3 = 1;
        }
        
        data_in=USART_ReceiveChar();                //leemos dato enviado por bluetooth
        if(data_in != 0){   
            fan(data_in);
            data_in =0;
        }
}
}
void fan(char data_in){

        if(data_in=='A')
        {   
            LATAbits.LATA1 = 1;                    //fan1 on 
        }
        else if(data_in=='B')
                
        {
            LATAbits.LATA1 = 0;                    //fan1 off
        }
        else if(data_in=='C')
                
        {
            LATAbits.LATA2 = 1;                    //fan2 on
        }
        else if(data_in=='D')
                
        {
            LATAbits.LATA2 = 0;                    //fan2 off
        }
        else if(data_in=='E')
                
        {
            LATAbits.LATA3 = 1;                    //fan3 on
        }
        else if(data_in=='F')
                
        {
            LATAbits.LATA3 = 0;                    //fan3 off
        }
}

void retardo(int v) {
 for(int i = 0; i <= v; i++)
 {
 __delay_ms(10);
 }
}

void writeEEPROM(unsigned int address, unsigned char data){
    I2CStart();
    I2CWrite(0xa0);
    I2CWrite(address >> 8);
    I2CWrite(address);
    I2CWrite(data);
    I2CStop();
}

unsigned char readEEPROM(unsigned int address){
    unsigned char data;
    I2CStart();
    I2CWrite(0xa0);
    I2CWrite(address >> 8);
    I2CWrite(address);
    I2CRestart();
    I2CWrite(0xa1);
    data = I2CRead();
    I2CStop();
    return data;
}
