#include "p18f8722.inc"
; CONFIG1H
  CONFIG  OSC = HSPLL, FCMEN = OFF, IESO = OFF
; CONFIG2L
  CONFIG  PWRT = OFF, BOREN = OFF, BORV = 3
; CONFIG2H
  CONFIG  WDT = OFF, WDTPS = 32768
; CONFIG3L
  CONFIG  MODE = MC, ADDRBW = ADDR20BIT, DATABW = DATA16BIT, WAIT = OFF
; CONFIG3H
  CONFIG  CCP2MX = PORTC, ECCPMX = PORTE, LPT1OSC = OFF, MCLRE = ON
; CONFIG4L
  CONFIG  STVREN = ON, LVP = OFF, BBSIZ = BB2K, XINST = OFF
; CONFIG5L
  CONFIG  CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF, CP4 = OFF, CP5 = OFF
  CONFIG  CP6 = OFF, CP7 = OFF
; CONFIG5H
  CONFIG  CPB = OFF, CPD = OFF
; CONFIG6L
  CONFIG  WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF, WRT4 = OFF
  CONFIG  WRT5 = OFF, WRT6 = OFF, WRT7 = OFF
; CONFIG6H
  CONFIG  WRTC = OFF, WRTB = OFF, WRTD = OFF
; CONFIG7L
  CONFIG  EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTR4 = OFF
  CONFIG  EBTR5 = OFF, EBTR6 = OFF, EBTR7 = OFF
; CONFIG7H
  CONFIG  EBTRB = OFF

; TODO INSERT ISR HERE
UDATA_ACS   
s_1_up res 1 ; button_state_for_player_1 up
s_1_down res 1 ; button_state_for_player_1 down
s_2_up res 1 ; button_state_for_player_2 up
s_2_down res 1     ; button_state_for_player_2 down
move_up_1 res 1    ; Toggle flag to move the paddle1 up
move_down_1 res 1  ; Toggle flag to move the paddle1 down
move_up_2 res 1    ; Toggle flag to move the paddle2 up
move_down_2 res 1  ; Toggle flag to move the paddle2 down
counter res 1;     ; Counter for 300 ms (It will count up to 46)
Yok_olan46 res 1;
move_ball_flag res 1;

RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

ORG 0X008
    GOTO HIGH_ISR
    
;*******************************************************************************
; MAIN PROGRAM
;*******************************************************************************         

HIGH_ISR:
    bcf  INTCON, 2 ; CLEAR INTERRUPT FLAG
    incf counter
    movf counter,w
    cpfseq Yok_olan46
    retfie
    clrf counter
    bsf  move_ball_flag, 0
    retfie
    
START
    call INIT    

main:
    call paddle_1
    call paddle_2
    call move_ball
    call display
    goto main

move_ball:

display:

; CHECK WHETHER THE BUTTONS RG1 AND RG0 IS PRESSED    
paddle_1: 
    ;CHECK RG1 IS PRESSED
    btfsc s_1_up,0
    bra RG1_pressed
    RG1_released: ; IT WAS NOT PRESSED
	btfss PORTG,1 ; IT IS NOW PRESSED ?
	bra RG1_end
	bsf move_up_1,0
	bsf s_1_up,0
	bra RG1_end
    
    RG1_pressed: ; IT WAS PRESSED
	btfsc PORTG,1 ; IT IS NOW RELEASED ?
	bra RG1_end
	bcf s_1_up,0
	
    RG1_end:
    ;CHECK RG0 IS PRESSED
    btfsc s_1_down,0
    bra RG0_pressed
    
    RG0_released: ; IN THE PREVIOUS STATE RG0 IS NOT PRESSED
	btfss PORTG,0
	bra RGA_end
	bsf move_down_1,0
	bsf s_1_down,0
	bra RGA_end
    
    RG0_pressed:  ; IN THE PREVIOUS STATE RG0 IS  PRESSED
	btfsc PORTG,0
	bra RGA_end
	bcf s_1_down,0
	
    RGA_end:
    
    call paddle_1_led_task ; MOVE THE PADDLE UP OR DOWN
    
    return 

    paddle_1_led_task:            ;MOVE THE PADDLE1 ACCORDING TO THE FLAGS
	btfss move_up_1, 0
	goto  move_paddle1_down   ; MOVE UP FLAG IS NOT SET CHECK THE DOWN
	move_paddle1_up:
	    btfsc PORTA, 0
	    goto  move_paddle1_end; PADDLE CANNOT MOVE UP FURTHER
	    rrncf PORTA           ; MOVE THE PADDLE UP
	    goto  move_paddle1_end
	    
	move_paddle1_down:
	    btfss move_down_1,0  ; CHECK WHETHER THE DOWN FLAG IS SET
	    goto  move_paddle1_end
	    btfsc PORTA, 5       ; PADDLE1 CANNOT MOVE DOWN FURTHER
	    goto  move_paddle1_end
	    rlncf PORTA          ; MOVE THE PADDLE DOWN

        move_paddle1_end:
	    bcf   move_down_1, 0 ; WE CLEAR UP_DOWN FLAGS SO THAT WE WILL WAIT ANOTHER PRESS TO MOVE UP OR DOWN
	    bcf   move_up_1, 0   ; WE WILL IGNORE THE PREVIOUS RG1(UP) PRESS
	    return
	    
; CHECK WHETHER THE BUTTONS RG3 AND RG2 IS PRESSED	
paddle_2:
    ;CHECK RG3 IS PRESSED
    btfsc s_2_up,0
    bra RG3_pressed
    RG3_released:
	btfss PORTG,3
	bra RG3_end
	bsf move_up_2,0
	bsf s_2_up,0
	bra RG3_end
    
    RG3_pressed:
	btfsc PORTG,3
	bra RG3_end
	bcf s_2_up,0 ; RG3 IS RELEASED SO STATE FOR RG3 = 0
	
    RG3_end:
    ;CHECK RG2 IS PRESSED
    btfsc s_2_down,0
    bra RG2_pressed
    
    RG2_released: ; IN THE PREVIOUS STATE RG2 IS NOT PRESSED
	btfss PORTG,2
	bra RGF_end
	bsf move_down_2,0
	bsf s_2_down,0
	bra RGF_end
    
    RG2_pressed:  ; IN THE PREVIOUS STATE RG2 IS  PRESSED
	btfsc PORTG,2
	bra RGF_end
	bcf s_2_down,0 ; RG2 IS RELEASED SO STATE=0
	
    RGF_end:
    
    call paddle_2_led_task ; MOVE THE PADDLE UP OR DOWN
    
    return 

    paddle_2_led_task: ;MOVE THE PADDLE2 ACCORDING TO THE FLAGS
	btfss move_up_2, 0
	goto  move_paddle2_down ; MOVE UP FLAG IS NOT SET, CHECK THE DOWN
	move_paddle2_up:
	    btfsc PORTF, 0
	    goto  move_paddle2_end;  PADDLE CANNOT MOVE UP FURTHER
	    rrncf PORTF ; MOVE THE PADDLE UP
	    goto  move_paddle2_end
	    
	move_paddle2_down:
	    btfss move_down_2,0 ; CHECK WHETHER THE DOWN FLAG IS SET
	    goto  move_paddle2_end
	    btfsc PORTF, 5       ; PADDLE CANNOT MOVE DOWN FURTHER
	    goto  move_paddle2_end
	    rlncf PORTF         ; MOVE THE PADDLE DOWN
	    
        move_paddle2_end:
    	    bcf   move_up_2, 0   ; WE CLEAR UP_DOWN FLAGS SO THAT WE WILL WAIT ANOTHER PRESS TO MOVE UP OR DOWN
	    bcf	  move_down_2, 0 ; WE WILL IGNORE THE PREVIOUS RG2 PRESS
	    return
	    
INIT
    MOVLW   b'00001111'
    MOVWF   TRISG ; MAKE RG0-RG1-RG2-RG3 PORTS(PINS) INPUT
    CLRF    TRISA  ; MAKE PORTA AS OUTPUT
    CLRF    TRISF  ; MAKE PORTF AS OUTPUT

    MOVLW   0X0F
    MOVWF   ADCON1 ; MAKE PORTA DIGITAL OUTPUT
    MOVLW   b'00011100' ; TURN ON THE FIRST LEDS
    MOVWF   PORTA
    MOVWF   PORTF
    CLRF    move_ball_flag
    CLRF    s_1_up
    CLRF    s_2_up   
    CLRF    s_1_down 
    CLRF    s_2_down
    CLRF    move_up_1
    CLRF    move_down_1
    CLRF    move_up_2
    CLRF    move_down_2
    MOVLW   d'46'
    MOVWF   Yok_olan46
    CLRF    INTCON ;
    MOVLW   b'11000111'
    MOVWF   T0CON
    BSF     INTCON, 5
    BSF	    INTCON, 7; ENABLE INTERRUPTS
    return
    
    END