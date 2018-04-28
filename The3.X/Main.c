#include <p18cxxx.h>
#include <p18f8722.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

#include "Includes.h"
#include "LCD.h"

int waited_3_sec = 0, re1_pressed = 0, set_a_pin = 0;

void updateLCD();
void wait_RE1(); // Wait for RE1 button press and release
void delay_3();  // It delays 3 seconds
void delay_ms(int k); // It delays n miliseconds when it is called
void set_a_pin(); 
// Main Function
void main(void)
{
    InitLCD();			// Initialize LCD in 4bit mode
    ClearLCDScreen();           // Clear LCD screen
 
    WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
    WriteStringToLCD(" $>Very Safe<$ ");	// Write Hello World on LCD
    while(1){
        //set_a_pin();
        wait_RE1();
        updateLCD();
    }
}
void delay_ms(int n)
{
    for(int k=0; k < n; k++)
    {
      for(int i=0; i < 26;i++)
          for(int j=0; j < 30; j++)
            {;}
    }
 }
void delay_3()
{   
    for(int i=0; i < 3000; i++)
        delay_ms(1);
}
void set_a_pin()
{
    if(waited_3_sec == 3 && set_a_pin == 0)
    {
        set_a_pin  = 1;
    }
}
void wait_RE1()
{
    // Check whether the we moved into the another phase( taking input from user )
    if( waited_3_sec == 0)
    {
        if( PORTEbits.RE1 == 0)// We pushed the RE1
        {
            re1_pressed = 1;
        }
        else // We released the RE1 or It has never been pushed
        {
            if(re1_pressed == 1)
            {
                waited_3_sec = 1; // So we will not wait for RE1 button anymore
                re1_pressed = 0;
            }
        }
    }
}
void updateLCD() {
    if(waited_3_sec == 1)
    {
        waited_3_sec = 3; // So it will not go into wait_RE1() and here again, It will enter inside of the set_a_pin()
        set_a_pin = 1;
        delay_3();
        return;
    }
    if( set_a_pin == 1)
    {
        ClearLCDScreen();           // Clear LCD screen
        WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
        WriteStringToLCD(" Set a pin:#### ");	// Write Hello World on LCD
        if( blink_flag == 1)
        {
            ClearLCDScreen();           // Clear LCD screen
            WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
            WriteStringToLCD(" Set a pin: ### ");	// Write Hello World on LCD
            // CALCULATE THE TIME
        }
        set_a_pin = 2;
    }
    else
    {
        WriteCommandToLCD(0xC0); // Goto to the beginning of the second line
        WriteStringToLCD(" $$$$$$$$$$$$$ ");	// Write Hello World on LCD
    }

    //__delay_ms(2);

}
