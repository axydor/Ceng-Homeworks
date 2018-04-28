#include <p18cxxx.h>
#include <p18f8722.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

#include "Includes.h"
#include "LCD.h"

int waited_3_sec = 0, re1_pressed = 0, set_a_pin = 0;
int a = 0;
char no;

void updateLCD();
void wait_RE1(); // Wait for RE1 button press and release
void delay_3(); // It delays 3 seconds
void delay_ms(int k); // It delays n miliseconds when it is called
void set_apin();
void Init();

int counter = 0;
int blink_c = 0;
int blink_flag=0;
int result = 0;

void interrupt isr(void) {
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0;
        counter++;
        if (counter == 20) {
            counter = 0;
            ADCON0bits.GO = 1;
        }
        TMR0L = 61;
    }
    if (PIR1bits.ADIF == 1) {
        PIR1bits.ADIF = 0;
        result = ADRES;

        if ((0 <= result) && (result <= 99))
            no = '0';
        else if (result > 99 && result < 200)
            no = '1';
        else if (result > 199 && result < 300)
            no = '2';
        else if (result > 299 && result < 400)
            no = '3';
        else if (result > 399 && result < 500)
            no = '4';
        else if (result > 499 && result < 600)
            no = '5';
        else if (result > 599 && result < 700)
            no = '6';
        else if (result > 699 && result < 800)
            no = '7';
        else if (result > 799 && result < 900)
            no = '8';
        else
            no = '9';
        
        ClearLCDScreen(); // Clear LCD screen
        delay_ms(1000);
        WriteCommandToLCD(0x80); // Goto to the beginning of the first line
        WriteDataToLCD(no); // Write Hello World on LCD
        delay_ms(1000);
    }
}


// Main Function

void main(void) {
    InitLCD(); // Initialize LCD in 4bit mode
    ClearLCDScreen(); // Clear LCD screen
    Init();

    //WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
    //WriteStringToLCD(" $>Very Safe<$ ");	
    while (1) {
        //set_a_pin();
        a = 1;
    }
}

void delay_ms(int n) {
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < 26; i++)
            for (int j = 0; j < 30; j++) {
                ;
            }
    }
}

void delay_3() {
    for (int i = 0; i < 3000; i++)
        delay_ms(1);
}

void set_apin() {
    if (waited_3_sec == 3 && set_a_pin == 0) {
        set_a_pin = 1;
    }
}

void wait_RE1() {
    // Check whether the we moved into the another phase( taking input from user )
    if (waited_3_sec == 0) {
        if (PORTEbits.RE1 == 0)// We pushed the RE1
        {
            re1_pressed = 1;
        } else // We released the RE1 or It has never been pushed
        {
            if (re1_pressed == 1) {
                waited_3_sec = 1; // So we will not wait for RE1 button anymore
                re1_pressed = 0;
            }
        }
    }
}

void updateLCD() {
    if (waited_3_sec == 1) {
        waited_3_sec = 3; // So it will not go into wait_RE1() and here again, It will enter inside of the set_a_pin()
        set_a_pin = 1;
        delay_3();
        return;
    }
    if (set_a_pin == 1) {
        ClearLCDScreen(); // Clear LCD screen
        WriteCommandToLCD(0x80); // Goto to the beginning of the first line
        WriteStringToLCD(" Set a pin:#### "); // Write Hello World on LCD
        set_a_pin = 2;
    } else {
        WriteCommandToLCD(0xC0); // Goto to the beginning of the second line
        WriteStringToLCD(" $$$$$$$$$$$$$ "); // Write Hello World on LCD
    }
    //__delay_ms(2);
}

void Init() {
    ADCON1 = 0b00000000; // channel = 12 
    ADCON0 = 0b00110001; // Go = 0, ADON is enabled, channel = 12
    ADCON2 = 0b10001010; // Acquisition time = 2 Tad, Right Justified, A/D Clock conversion Fosc/32
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.TMR0IF = 0;
    T0CON = 0b11010111;
    TMR0L = 61;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;
}
