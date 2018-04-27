#include <p18cxxx.h>
#include <p18f8722.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

#include "Includes.h"
#include "LCD.h"




unsigned char c1;
unsigned int a =3 , b = 3, c = 6;



void updateLCD();

// Main Function
void main(void)
{
    __delay_ms(15);
    __delay_ms(15);
    __delay_ms(15);
    __delay_ms(15);

    InitLCD();			// Initialize LCD in 4bit mode


    ClearLCDScreen();           // Clear LCD screen
    WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
    WriteStringToLCD("CENG");	// Write Hello World on LCD


    while(1){

            updateLCD();

    }
}


void updateLCD() {
  
    WriteCommandToLCD(0xC0); // Goto to the beginning of the second line
    c1 = (char)(((int)'0') + a);
    WriteDataToLCD(c1);
    c1 = (char)(((int)'0') + b);
    WriteDataToLCD(c1);
    c1 = (char)(((int)'0') + c);
    WriteDataToLCD(c1);
    __delay_ms(2);

}