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

;*******************************************************************************
; Variables & Constants
;*******************************************************************************
UDATA_ACS
  t1	res 1	; used in delay
  t2	res 1	; used in delay
  t3	res 1	; used in delay
  state res 1	; controlled by RB0 button
  rb5_state res 1
  ra4_state res 1
  turn	    res 1; If turn is 1, then snake takes the turn
  clockwise res 1; If clockwise is 1, then system is in state1 clockwise moving
;*******************************************************************************
; Reset Vector
;*******************************************************************************

RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

;*******************************************************************************
; MAIN PROGRAM
;*******************************************************************************

; ***** MY STATE BY STATE VIEW TO THE BOARD *****
;
;    ________________
;   | 11/0 | 1 | 2\3 |
;   | 10   | - |  4  |
;   | 9\8  | 7 | 5/6 |
;    ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ??
;   
;   ******************
MAIN_PROG CODE	; let linker place main program

START
    call INIT	; initialize variables and ports
    nop

MAIN_LOOP        
    
    state_c_0:
	MOVLW	b'00000001'
	MOVWF	PORTA
	MOVWF	PORTB
	call	DELAY750
	BTFSC	PORTA,4 ; CHEK IF RA4 IS RELEASED
	GOTO	state_c_1
	BTFSS	clockwise,0
	GOTO	state_cc_0 ; GO INTO STATE_2, SNAKE MOVES IN COUNTER CLOCKWISE DIRECTION
	GOTO	state_c_1
    
    state_c_1:
	CLRF	turn
	CLRF	PORTA
	BSF	PORTC,0
	call	DELAY750
	BTFSC	PORTA,4 ; CHEK IF RA4 IS RELEASED
	GOTO	state_c_2	
	BTFSS	clockwise,0
	GOTO	state_cc_1
	GOTO	state_c_2
	
    state_c_2:
	CLRF	PORTB
	BSF	PORTD,0
	call	DELAY750_TURN
	turn_c_2:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4 ; CHEK IF RA4 IS RELEASED
	GOTO	wait_c_2		
	BTFSS	clockwise,0
	GOTO	state_cc_2
	    wait_c_2:
		BTFSC	PORTB,5     ; CHECK IF THE RB5 IS RELEASED
		GOTO	turn_c_2
		BTFSS	turn,0
		GOTO	turn_c_2
		GOTO	state_c_3
	;GOTO	state_c_3

    state_c_3:
	CLRF	turn
	CLRF	PORTC
	BSF	PORTD,1
	call	DELAY750
	BTFSC	PORTA,4
	GOTO	state_c_4
	BTFSS	clockwise,0
	GOTO	state_cc_3
	GOTO	state_c_4
	
    state_c_4:
	CLRF	turn
	MOVLW	b'00000110'
	MOVWF	PORTD
	call	DELAY750
	BTFSC	PORTA,4
	GOTO	state_c_5
	BTFSS	clockwise,0
	GOTO	state_cc_4
	GOTO	state_c_5
	
    state_c_5:
	MOVLW	b'00001100'
	MOVWF	PORTD
	call	DELAY750_TURN
	turn_c_5:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4
	GOTO	wait_c_5
	BTFSS	clockwise,0
	GOTO	state_cc_5
	    wait_c_5:
		BTFSC	PORTB,5
		GOTO	turn_c_5
		BTFSS	turn,0
		GOTO	turn_c_5
		GOTO	state_c_6
		

    state_c_6:
	CLRF	turn
	CLRF	PORTD
	BSF	PORTD,3
	BSF	PORTC,3
	call	DELAY750
	BTFSC	PORTA,4
	GOTO	state_c_7	
	BTFSS	clockwise,0
	GOTO	state_cc_6
	GOTO	state_c_7

    state_c_7:
	CLRF	PORTD
	BSF	PORTB,3
	call	DELAY750	
	BTFSC	PORTA,4
	GOTO	state_c_8
	BTFSS	clockwise,0
	GOTO	state_cc_7
	GOTO	state_c_8
    
    state_c_8:
	CLRF	PORTC
	BSF	PORTA,3
	call	DELAY750_TURN
	turn_c_8:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4
	GOTO	wait_c_8
	BTFSS	clockwise,0
	GOTO	state_cc_8
	    wait_c_8:
    		BTFSC	PORTB,5
		GOTO	turn_c_8
		BTFSS	turn,0
		GOTO	turn_c_8
		GOTO	state_c_9
	
    state_c_9:
	CLRF	turn
	CLRF	PORTB
	BSF	PORTA,2
	call	DELAY750
	BTFSC	PORTA,4
	GOTO	state_c_10
	BTFSS	clockwise,0
	GOTO	state_cc_9
	GOTO	state_c_10
	
    state_c_10:
	;CLRF	PORTA
	MOVLW	b'00000110'
	MOVWF	PORTA
	call	DELAY750
	BTFSC	PORTA,4
	GOTO	state_c_11
	BTFSS	clockwise,0
	GOTO	state_cc_10
	GOTO	state_c_11
	
    state_c_11:
	;CLRF PORTA
	MOVLW	b'00000011'
	MOVWF	PORTA
	call	DELAY750_TURN
	turn_c_11:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4
	GOTO	wait_c_11
	BTFSS   clockwise,0
	GOTO    state_cc_11
	    wait_c_11:
		BTFSC	PORTB,5
		GOTO	turn_c_11
		BTFSS	turn,0
		GOTO	state_c_11
	        GOTO    state_c_0
    
		
;		STATE_CC_X  ---  SNAKE MOVES IN COUNTER CLOCKWISE		
		
		
    state_cc_0:
	CLRF	turn
	CLRF	PORTC
	BSF     PORTA,0
	BSF     PORTB,0
	call    DELAY400_TURN ;check rb_5 ?
	turn_cc_0:
	call	CHECK_RB5
	call	CHECK_RA4
	BTFSC	PORTA,4
	GOTO	wait_cc_0
	BTFSC	clockwise,0
	GOTO	state_c_0
	    wait_cc_0:
		BTFSC	PORTB,5
		GOTO	turn_cc_0
		BTFSS	turn,0
		GOTO	turn_cc_0
		GOTO	state_cc_11
	
    state_cc_1:
	CLRF	PORTD
	BSF	PORTB,0
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_0
	BTFSC	clockwise,0
	GOTO	state_c_1
	GOTO	state_cc_0

    state_cc_2:
	CLRF	turn
	CLRF	PORTD
	BSF	PORTC,0
	BSF	PORTD,0
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_1	
	BTFSC	clockwise,0
	GOTO	state_c_2
	GOTO	state_cc_1
	
    state_cc_3:
	CLRF PORTD
	MOVLW	b'00000011'
	MOVWF	PORTD
	call	DELAY400_TURN
	turn_cc_3:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4
	GOTO	wait_cc_3	
	BTFSC	clockwise,0
	GOTO	state_c_3
	    wait_cc_3:
		BTFSC	PORTB,5
		GOTO	turn_cc_3
		BTFSS	turn,0
		GOTO	turn_cc_3
		GOTO	state_cc_2
		
    state_cc_4:
	CLRF	PORTD
	MOVLW	b'00000110'
	MOVWF	PORTD
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_3	
	BTFSC	clockwise,0
	GOTO	state_c_4
	GOTO	state_cc_3
	
    state_cc_5:
	CLRF	turn    
    	CLRF	PORTC
	BSF	PORTD,2
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_4	
	BTFSC	clockwise,0
	GOTO	state_c_5
	GOTO	state_cc_4
	
    state_cc_6:
    	CLRF	PORTB
	BSF	PORTD,3
	call	DELAY400_TURN
	turn_cc_6:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4
	GOTO	wait_cc_6	
	BTFSC	clockwise,0
	GOTO	state_c_6
	    wait_cc_6:
		BTFSC	PORTB,5
		GOTO	turn_cc_6 ; RA4 CAN BE PRESSED
		BTFSS	turn,0
		GOTO	turn_cc_6
		GOTO	state_cc_5
    
    state_cc_7:
    	CLRF	PORTA
	BSF	PORTC,3
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_6	
	BTFSC	clockwise,0
	GOTO	state_c_7
	GOTO	state_cc_6
	
    state_cc_8:
	CLRF	turn
	CLRF	PORTA
	BSF	PORTA,3
	BSF	PORTB,3
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_7		
	BTFSC	clockwise,0
	GOTO	state_c_8
	GOTO	state_cc_7
	
    state_cc_9:
	;CLRF	PORTA
	MOVLW	b'00001100'
	MOVWF	PORTA
	call	DELAY400_TURN
        turn_cc_9:
	call	CHECK_RA4
	call	CHECK_RB5
	BTFSC	PORTA,4
	GOTO	wait_cc_9
	BTFSC	clockwise,0
	GOTO	state_c_9
	    wait_cc_9:
		BTFSS	PORTB,5
		GOTO	turn_cc_9
		BTFSS	turn,0
		GOTO	turn_cc_9
		GOTO	state_cc_8
	
    state_cc_10:
	CLRF	turn
	;CLRF	PORTA
	MOVLW	b'00000110'
	MOVWF	PORTA	
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_9
	BTFSC	clockwise,0
	GOTO	state_c_10
	GOTO	state_cc_9

    state_cc_11: ;SNAKE CAN COME FROM CC_0 TO CC_11
	CLRF	turn
	CLRF	PORTB
	MOVLW	b'00000011'
	MOVWF	PORTA	
	call	DELAY400
	BTFSC	PORTA,4
	GOTO	state_cc_10
	BTFSC	clockwise,0
	GOTO	state_c_11
	GOTO	state_cc_10
	
    state_c_end:
	
    GOTO MAIN_LOOP  ; loop forever
  
CHECK_RB5
    BTFSC rb5_state,0
    GOTO _state_2

    _state_1:
        BTFSS PORTB,5
        GOTO _state_en
        MOVLW 0X01
        MOVWF rb5_state
        MOVWF turn
        GOTO _state_en

    _state_2:
	BTFSC WREG,5
        GOTO _state_en
        CLRF rb5_state

    _state_en:
        return

CHECK_RA4
    BTFSC ra4_state,0
    GOTO __state_2

    __state_1:
        BTFSS	PORTA,4
        GOTO	__state_en
        MOVLW	0X01
        MOVWF	ra4_state
        XORWF	clockwise
        GOTO	__state_en

    __state_2:
        BTFSC	PORTA,4
        GOTO	__state_en
        CLRF	ra4_state

    __state_en:
        return
	
	
DELAY	; Time Delay Routine with 3 nested loops
    MOVLW 82	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    _loop3:
	MOVLW 0xA0  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	_loop2:
	    MOVLW 0x9F	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    _loop1:
		decfsz t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO _loop1 ; ELSE Keep counting down
		decfsz t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO _loop2 ; ELSE Keep counting down
		decfsz t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO _loop3 ; ELSE Keep counting down
		return
	
DELAY750	; Time Delay Routine with 3 nested loops
    MOVLW 61	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    sloop3:
	MOVLW 0xA0  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	sloop2:
	    call  CHECK_RA4
	    MOVLW 0x9C	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    sloop1:
		decfsz t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO sloop1 ; ELSE Keep counting down
		decfsz t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO sloop2 ; ELSE Keep counting down
		decfsz t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO sloop3 ; ELSE Keep counting down
		return	

DELAY750_TURN	; Time Delay Routine with 3 nested loops
    MOVLW 61	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    stloop3:
	MOVLW 0xA0  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	stloop2:
	    call  CHECK_RA4
	    call  CHECK_RB5
	    MOVLW 0x9A	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    stloop1:
		decfsz t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO stloop1 ; ELSE Keep counting down
		decfsz t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO stloop2 ; ELSE Keep counting down
		decfsz t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO stloop3 ; ELSE Keep counting down
		return			
		
DELAY400_TURN	; Time Delay Routine with 3 nested loops
    MOVLW 3A	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    loop3:
	MOVLW 0x90  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	loop2:
	    call  CHECK_RA4
	    call  CHECK_RB5
	    MOVLW 0x9A	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    loop1:
		decfsz t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO loop1 ; ELSE Keep counting down
		decfsz t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO loop2 ; ELSE Keep counting down
		decfsz t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO loop3 ; ELSE Keep counting down
		return
		
  
DELAY400	; Time Delay Routine with 3 nested loops
    MOVLW 3A	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    xloop3:
	MOVLW 0x90  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	xloop2:
	    call  CHECK_RA4
	    MOVLW 0x9A	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    xloop1:
		decfsz t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO xloop1 ; ELSE Keep counting down
		decfsz t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO xloop2 ; ELSE Keep counting down
		decfsz t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO xloop3 ; ELSE Keep counting down
		return

INIT
    MOVLW	0X01
    MOVWF	clockwise
    CLRF	turn
    CLRF	rb5_state
    CLRF	ra4_state		
    CLRF    state
    MOVLW   0X0F
    MOVWF   ADCON1; MAKE PORTA DIGITAL (PORTA CAN BE EITHER ANALOG OR DIGITAL)
    MOVLW   0x0F  ; TURN ON THE PINS [0-3]
    MOVWF   LATA
    MOVWF   LATB
    MOVWF   LATC
    MOVWF   LATD
    MOVLW   0X10
    MOVWF   TRISA ; RA4 IS INPUT
    MOVLW   0X20  ; RB5 IS INPUT
    MOVWF   TRISB
    MOVLW   0XF0
    MOVWF   TRISC
    MOVWF   TRISD
    call    DELAY	; wait a second
    call    DELAY	; wait a second
    CLRF	LATA
    CLRF	LATB
    CLRF	LATC	; turn off all portC leds
    CLRF	LATD	; turn off all portF leds
    return
    
    
BUTTON_TASK ; very primitive button task
    BTFSS PORTB,0
    return
    _debounce:
	BTFSC PORTB,0
	goto _debounce	; busy waiting. FIXME !!!
	COMF state, 1
	return
    
    
END
