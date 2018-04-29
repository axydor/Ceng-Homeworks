#include <p18cxxx.h>
#include <p18f8722.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

#include "Includes.h"
#include "LCD.h"

int waited_3_sec = 0;
int re1_pressed = 0, set_flag = 0;
int a = 0; // dummy, but very dummy variable
char no; // Each number of the password

void updateLCD();
void wait_RE1(); // Wait for RE1 button press and release
void delay_3(); // It delays 3 seconds
void delay_ms(int k); // It delays k milliseconds when it is called
void set_pin();      // Starts the Initialization of Interrupts
void wink(int i);      // Blink the i'th cell of the password
void init_interrupts();
void up_write(char* str); // Writes the given* String to the Upper Side of LCD. (*): ' Set a pin:####'

int counter = 0; // Counter for counting 100 ms in the TIMER0
int blink_c = 0; // Counter for setting blink_flag
int blink_flag = 0; // Flag to blink '#' at every 250 ms
int result = 0; // The result of the A/D conversion
int result1 = 0; // The result of the A/D conversion
int result2 = 0;
int blink_index = 0;
int n_of_pins_setted = 0; // Counter up to 4
char pass[5] = "####"; // 
char blink[5] = "1111"; // Whether we will blink the related blink_index or not

void interrupt isr(void) {
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0;
        counter++;
        blink_c++;
        if (counter == 20) { // 100 ms has passed
            counter = 0;
            ADCON0bits.GO = 1; // We start the A/D conversions
        }
        if (blink_c == 50) // 250 ms has passed
        {
            blink_c = 0;
            blink_flag = 1;
        }
        TMR0L = 61;
    }
    if (PIR1bits.ADIF == 1) {
        PIR1bits.ADIF = 0;
        result1 = ADRES;
        if (result1 != result2) {
            blink[blink_index] = '0'; // We will not blink that number

            result = result1;
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

            pass[blink_index] = no;
            blink[blink_index]= '0';
        }
        result2 = result1;
    }
    // CHECK RB6 - RB7 pushes
    if (INTCONbits.RBIF == 1) {
        // CLEAR THE FLAG
        INTCONbits.RBIF = 0;
        // RB6 FOR CHANGING CURRENTLY ACTIVE DIGIT
        if (PORTBbits.RB6 == 1) {
             // BE SURE THAT IT WAS PRESSED 
            delay_ms(20);
            if (PORTBbits.RB6 == 1) {
                if (blink_index < 4) {
                    blink_index++;
                } else {
                    blink_index = 0;
                }
            }
        } else { // RB7 FOR SETTING THE PIN
            delay_ms(20);
            if (PORTBbits.RB7 == 1) {
                // WHAT WILL HAPPEN IF USER PRESSED RB7 WITHOUT CHANGING POT
                n_of_pins_setted++;
                blink[blink_index] = '0'; // So it will not blink, It is set
            }
        }
    }
}

void main(void) {
    InitLCD(); // Initialize LCD in 4bit mode
    ClearLCDScreen(); // Clear LCD screen
    init_interrupts();
    /*
    WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
    WriteStringToLCD(" $>Very Safe<$ ");	
    WriteCommandToLCD(0xC0); // Goto to the beginning of the second line
    WriteStringToLCD(" I DON'T THINK SO  ");
    */
    set_flag = 1;
    while (1) {
        //set_pin();
        //show_pass();
        updateLCD();
        //a = 1;
    }
}

void set_pin() {
    if (set_flag == 1){
        if(a == 0) {
            init_interrupts();
            a = 1;            
        }
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
// Writes the String to the upper side of LCD
void up_write(char* str) {
    WriteCommandToLCD(0x80); // Goto to the beginning of the first line
    WriteStringToLCD(" Set a pin:");
    WriteStringToLCD(str);
    WriteDataToLCD(' ');
}

// Blink The i'th cell
void wink(int i) {
    char temp[5];
    if (blink[i] == '1') {
        if (blink_flag == 1) {
            blink_flag = 0;
            for (a = 0; a < 4; a++)
                temp[a] = pass[a];
            temp[i] = ' ';
            up_write(temp);
            return;
        }
    }    
    up_write(pass);
}

void updateLCD() {
    if (waited_3_sec == 1) {
        waited_3_sec = 3; // So it will not go into wait_RE1() and here again, It will enter inside of the set_a_pin()
        set_flag = 1;
        delay_3();
        return;
    }
    if (set_flag == 1 && (n_of_pins_setted < 4)) {
        switch (blink_index) {
            case 0:
                wink(0);
                break;
            case 1:
                wink(1);
                break;
            case 2:
                wink(2);
                break;
            case 3:
                wink(3);
                break;
        }
    } else {
        WriteCommandToLCD(0xC0); // Goto to the beginning of the second line
        WriteStringToLCD(" $$$$$$$$$$ ");
    }
    delay_ms(2);
}

void init_interrupts() {
    ADCON1 = 0b00000000; // channel = 12 
    ADCON0 = 0b00110001; // Go = 0, ADON is enabled, channel = 12
    ADCON2 = 0b10001010; // Acquisition time = 2 Tad, Right Justified, A/D Clock conversion Fosc/32
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.TMR0IF = 0;
    T0CON = 0b11010111;
    TMR0L = 61;

    INTCONbits.RBIE = 1; // ENABLE PORTB INTERRUPTS
    INTCONbits.TMR0IE = 1; // ENABLE TIMER0 INTERRUPTS
    INTCONbits.GIE = 1; // ENABLE GLOBAL INTERRUPTS
}
