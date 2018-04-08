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
s_1_up res 1 	   	  ; button_state_for_player_1 up
s_1_down res 1     	  ; button_state_for_player_1 down
s_2_up res 1 	   	  ; button_state_for_player_2 up
s_2_down res 1     	  ; button_state_for_player_2 down
move_up_1 res 1    	  ; Toggle flag to move the paddle1 up
move_down_1 res 1  	  ; Toggle flag to move the paddle1 down
move_up_2 res 1    	  ; Toggle flag to move the paddle2 up
move_down_2 res 1  	  ; Toggle flag to move the paddle2 down
counter res 1;     	  ; Counter for 300 ms (It will count up to 46)
Yok_olan46 res 1   	  ; Constant which is 46
move_ball_flag res 1      ; We should wait 300ms to raise this flag so that we can move the ball
direction res 1    	  ; IN WHICH DIRECTION THE BALL IS MOVING LEFT=1, RIGHT=0
column_ball res 1  	  ; the column of the ball,COLUMN=0-> PORTA, COLUMN=1 -> PORTB...
row res 1     		  ; the row of the ball
left_score res 1	  ; Holds the score of the left one
left_score_flag res 1	  ;
right_score res 1	  ; Holds the score of the right one
right_score_flag res 1
table_wreg res 1
t1 res 1
t2 res 1
t3 res 1
RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

ORG 0X0008
    GOTO HIGH_ISR

ORG 0X0018
    nop
    retfie


;*******************************************************************************
; MAIN PROGRAM
;*******************************************************************************         

    TABLE
    MOVF    PCL, F  ; A simple read of PCL will update PCLATH, PCLATU
    RLNCF   WREG, W ; multiply index X2
    ADDWF   PCL, F  ; modify program counter
    RETLW b'00111111' ;0 representation in 7-seg. disp. portJ
    RETLW b'00000110' ;1 representation in 7-seg. disp. portJ
    RETLW b'01011011' ;2 representation in 7-seg. disp. portJ
    RETLW b'01001111' ;3 representation in 7-seg. disp. portJ
    RETLW b'01100110' ;4 representation in 7-seg. disp. portJ
    RETLW b'01101101' ;5 representation in 7-seg. disp. portJ
    RETLW b'01111101' ;6 representation in 7-seg. disp. portJ
    RETLW b'00000111' ;7 representation in 7-seg. disp. portJ
    RETLW b'01111111' ;8 representation in 7-seg. disp. portJ
    RETLW b'01100111' ;9 representation in 7-seg. disp. portJ


HIGH_ISR:
    bcf    INTCON, 2   ; CLEAR INTERRUPT FLAG
    movwf  table_wreg  ; SAVING THE CONTENT OF THE WREG
    incf   counter
    movf   counter,w
    cpfseq Yok_olan46
    bra    high_end
    clrf   counter
    bsf    move_ball_flag, 0
    
    high_end:
    movf  table_wreg, w
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
    btfss move_ball_flag, 0 ; CHECK WHETHER IT IS GOOD TIME TO ROCKET THE BALL
    return
    clrf  move_ball_flag   ; CLEAR FLAG SO THAT BALL GET INTO THIS FUNCTION ONLY AFTER 300MS
    btfss direction, 0     ; "DON'T STOP ME NOW"
    goto  moving_to_right

    moving_to_left:  ;  SWITCH CASES TO CHOOSE WHERE ARE WE NOW?
        movf  column_ball, w
        xorlw 1 ;       BALL IS AT PORTB, MOVING TO THE PORT A
        btfsc STATUS, Z
        goto  left_a  ; IF(COLUMN==1) BALL MOVES FROM PORTB TO PORTA
        xorlw 2^1 
        btfsc STATUS, 2
        goto  left_b  ; IF(COLUMN==2) BALL MOVES FROM PORTC TO PORTB
        xorlw 3^2
        btfsc STATUS, 2
        goto  left_c  ; BALL MOVES FROM PORTD TO PORTC
        xorlw 4^3 
        btfsc STATUS, 2
        goto  left_d  ; BALL MOVES FROM PORTE TO PORTD
        xorlw 5^4  
        btfsc STATUS, 2
        goto  left_e  ; BALL MOVES FROM PORTF TO PORTE
        goto  no_where

        left_a: ;  BALL MOVES FROM PORTB TO PORTA
            clrf   column_ball
            clrf   PORTB
            btfss  TMR1L, 0
            goto   a_first_bit_0
            btfss  TMR1L, 1
            goto   a_bit_01
            goto   a_bit_00_11

            a_first_bit_0:
                btfss TMR1L, 1
                goto a_bit_00_11
                goto a_bit_10

            a_bit_00_11: ; MOVE AT THE SAME LINE
                movf  row, w
                andwf PORTA, w
                btfsc STATUS, 2
                goto  right_scored    ; RIGHT ONE SCORED
                btg   direction, 0    ; RIGHT ONE COULD NOT SCORED
                goto  right_b         ; BALL HITS THE PEDAL AND CHANGES DIRECTION

            a_bit_01: ; MOVE UP
                rrncf  row
		movlw  0     ; TEST IF BALL AT THE UPPER-BORDER
                cpfsgt row  ; IF ( ROW == 0) { INCREMENT ROW BY 1}
                incf   row
                movf   row,w
                andwf  PORTA, w
                btfsc  STATUS, 2
                goto   right_scored    
                goto   right_b  ;      BALL HITS THE PEDAL AND MOVES

            a_bit_10: ; MOVE DOWN
                rlncf  row
                movlw  d'64'
                cpfslt row      ; TEST IF BALL AT THE LOWER-BORDER
                rrncf  row      ; BALL EXCEEDS THE BORDER SO PUT IT IN BORDER
                movf   row, w   ;
                andwf  PORTA, w ;
                btfsc  STATUS, 2;
                goto   right_scored
                btg    direction, 0
                goto   right_b  ;      BALL HITS THE PEDAL AND CHANGES DIRECTION

        left_b:
	    movlw  d'1'       ; WE WILL GO TO THE PORT B SO SET COLUMN=1 -> PORTB
            movwf  column_ball
            clrf   PORTC
            btfss  TMR1L, 0
            goto   b_first_bit_0
            btfss  TMR1L, 1
            goto   b_bit_01
            goto   b_bit_00_11

            b_first_bit_0:
                btfss TMR1L, 1
                goto b_bit_00_11
                goto b_bit_10

            b_bit_00_11: ; MOVE AT THE SAME LINE
                movff row, PORTB
                goto  _end

            b_bit_01: ; MOVE UP
                rrncf  row
                movlw  0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
                movff  row, PORTB
                goto  _end

            b_bit_10: ; MOVE DOWN
                rlncf  row
                movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff  row, PORTB
                goto  _end

        left_c:
            movlw  d'2'       ; WE WILL GO TO THE PORTC SO SET COLUMN=2 -> PORTC
            movwf  column_ball
            clrf   PORTD
            btfss  TMR1L, 0
            goto   c_first_bit_0
            btfss  TMR1L, 1
            goto   c_bit_01
            goto   c_bit_00_11

            c_first_bit_0:
                btfss TMR1L, 1
                goto c_bit_00_11
                goto c_bit_10

            c_bit_00_11:
                movff row, PORTC
                goto  _end

            c_bit_01:
                rrncf row
                movlw 0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
                movff  row, PORTC
                goto   _end

            c_bit_10:
                rlncf  row
                movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff  row, PORTC
                goto   _end

        left_d:
            movlw  d'3'
            movwf  column_ball
            clrf   PORTE
            btfss  TMR1L, 0
            goto   d_first_bit_0
            btfss  TMR1L, 1
            goto   d_bit_01
            goto   d_bit_00_11

            d_first_bit_0:
                btfss TMR1L, 1
                goto  d_bit_00_11
                goto  d_bit_10

            d_bit_00_11:
                movff row, PORTD
		goto  _end

            d_bit_01:
                rrncf row
		movlw 0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
		movff  row, PORTD
                goto  _end

            d_bit_10:
                rlncf  row
		movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff row, PORTD
                goto  _end

        left_e:  ; WE ALREADY CHANGE THE PATH IN RIGHT_F SO HERE JUST MOVE LEFT
            movlw  d'4'
            movwf  column_ball

                movff row, PORTE
                goto _end

                goto  _end


    moving_to_right:
        movf  column_ball, w
        xorlw 0 ; BALL AT PORT A
        btfsc STATUS, 2
        goto  right_b ; IF(COLUMN==0) BALL MOVES FROM PORTA TO PORTB
        xorlw 1^0 
        btfsc STATUS, 2 
        goto  right_c ; IF(COLUMN==1) BALL MOVES FROM PORTB TO PORTC
        xorlw 2^1 ;   
        btfsc STATUS, 2
        goto  right_d ; IF(COLUMN==2) BALL MOVES FROM PORTC TO PORTD
        xorlw 3^2     ;
        btfsc STATUS, 2
        goto  right_e ; IF(COLUMN==3) BALL MOVES FROM PORTD TO PORTE
        xorlw 4^3     ;
        btfsc STATUS, 2
        goto  right_f ; BALL MOVES FROM PORTE TO PORTF
        goto  no_where

        right_b:      ; BALL MOVES FROM PORTA TO PORTB
            movlw  d'1'  ; WE WILL GO TO THE PORTB SO SET COLUMN=1 -> PORTB
            movwf  column_ball
            btfss  TMR1L, 0
            goto   rb_first_bit_0
            btfss  TMR1L, 1
            goto   rb_bit_01
            goto   rb_bit_00_11

            rb_first_bit_0:
                btfss TMR1L, 1
                goto  rb_bit_00_11
                goto  rb_bit_10

            rb_bit_00_11:
                movff row, PORTB
                goto  _end
		
            rb_bit_01:         ;  BALL MOVES UP
                rrncf  row
                movlw  0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
                movff  row, PORTB
                goto  _end

            rb_bit_10:
                rlncf  row
                movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff  row, PORTB
                goto   _end

        right_c:      ; BALL MOVES FROM PORTB TO PORTC
            clrf   PORTB
            movlw  d'2'
            movwf  column_ball
            btfss  TMR1L, 0
            goto   rc_first_bit_0
            btfss  TMR1L, 1
            goto   rc_bit_01
            goto   rc_bit_00_11

            rc_first_bit_0:
                btfss TMR1L, 1
                goto  rc_bit_00_11
                goto  rc_bit_10

            rc_bit_00_11:
                movff row, PORTC
                goto  _end

            rc_bit_01:
                rrncf row
                movlw 0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
                movff  row, PORTC
                goto  _end

            rc_bit_10:
                rlncf  row
                movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff  row, PORTC
                goto  _end

        right_d:      ; BALL MOVES FROM PORTC TO PORTD
            clrf   PORTC
            movlw  d'3'
            movwf  column_ball
            btfss  TMR1L, 0
            goto   rd_first_bit_0
            btfss  TMR1L, 1
            goto   rd_bit_01
            goto   rd_bit_00_11

            rd_first_bit_0:
                btfss TMR1L, 1
                goto  rd_bit_00_11
                goto  rd_bit_10

            rd_bit_00_11:
                movff row, PORTD
                goto  _end

            rd_bit_01:
                rrncf row
                movlw 0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
                movff  row, PORTD
                goto  _end
		
            rd_bit_10:
                rlncf  row
                movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff  row, PORTC
                goto  _end

        right_e:      ; BALL MOVES FROM PORTD TO PORTE
            clrf   PORTD
            movlw  d'4'
            movwf  column_ball
            btfss  TMR1L, 0
            goto   re_first_bit_0
            btfss  TMR1L, 1
            goto   re_bit_01
            goto   re_bit_00_11

            re_first_bit_0:
                btfss TMR1L, 1
                goto  re_bit_00_11
                goto  re_bit_10

            re_bit_00_11:  ; this path was choosen this
                movff row, PORTE
                goto  _end

            re_bit_01:   ; BALL MOVES  UPWARD
                rrncf  row
                movlw  0
                cpfsgt row     ; TEST IF BALL AT THE UPPER-BORDER
                incf   row     ; IF ROW=0, INCREMENT
                movff  row, PORTE
                goto  _end

            re_bit_10:   ; MOVE DOWNWARD
                rlncf  row
                movlw  d'64'
                cpfslt row   ; IF (ROW == 64), DIVIDE BY 2 -> TURN ON PORTX,5TH LED
                rrncf  row
                movff  row, PORTE
                goto  _end

        right_f:         ; BALL MOVES FROM PORTE TO PORTF
    	    clrf   PORTE
            movlw  d'4'
            movwf  column_ball  
            btfss  TMR1L, 0        ; WE TEST ACCORDING TO THE PDF BUT WITH TIMER1
            goto   rf_first_bit_0  
            btfss  TMR1L, 1
            goto   rf_bit_01
            goto   rf_bit_00_11

            rf_first_bit_0:
                btfss TMR1L, 1
                goto  rf_bit_00_11
                goto  rf_bit_10

            rf_bit_00_11:  ;this path was choosen MOVES ON SAME LINE
                movf   row, w
                andwf  PORTF, w
                btfsc  STATUS, 2
                goto   left_scored
                btg    direction,0
                goto   left_e

            rf_bit_01:   ; BALL MOVES UPWARD
                rrncf  row
		movlw  0 
                cpfsgt row   ; IF IT IS OUT OF BORDER
                incf   row   ; MAKE IT IN BORDER BY TURNING ON 1'ST LED
                movf   row, w
                andwf  PORTF, w  ; CHECK THE GOAL STATUS
                btfsc  STATUS, 2
                goto   left_scored
                btg    direction, 0 ; GOAL KEEPER SAVED
                goto   left_e

            rf_bit_10:   ; BALL MOVES DOWNWARD
                rlncf  row
		movlw  d'64' 
                cpfslt row   ; IF IT IS OUT OF BORDER
                rrncf  row   ; MAKE IT IN BORDER BY TURNING ON 5'TH LED
                movf   row, w
                andwf  PORTF, w  ; CHECK THE GOAL STATUS
                btfsc  STATUS, 2
                goto   left_scored
                btg    direction, 0 ; GOAL KEEPER SAVED
                goto   left_e

    _end

    return   ; return of the move_ball

    no_where:
        nop
        nop


; CHECK WHETHER THE BUTTONS RG1 AND RG0 IS PRESSED    
paddle_1:;CHECK RG1 IS PRESSED
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
	
    RG1_end:;CHECK RG0 IS PRESSED
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
	
    RG3_end:        ;   CHECK RG2 IS PRESSED
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

right_scored:
    incf   right_score
    movlw  d'5'
    cpfslt right_score
    goto   game_over
    call   display
    goto   after_goal
    
left_scored:
    incf   left_score
    movlw  d'5'
    cpfslt left_score
    goto   game_over    
    call   display
    goto   after_goal
    
after_goal:
    clrf    direction
    MOVLW   b'00011100' ; MAKE THE PADDLES IN INITIAL CONDITION
    MOVWF   PORTA
    MOVWF   PORTF
    MOVLW   b'00001000' ; TURN ON THE BALL
    MOVWF   PORTD
    MOVWF   row
    MOVLW   d'3'
    MOVWF   column_ball    
    goto    main

display:
    right_goals:
    	clrf   PORTH              ; BELOW PART IS FOR SETTING THE LEDS
        bsf    PORTH, 3
    	movf   right_score, w
        call   TABLE
        movwf  PORTJ
    	call   DELAY
	
    left_goals:
        clrf   PORTH
    	bsf    PORTH, 1
    	movf   left_score, w
        call   TABLE
        movwf  PORTJ
        call   DELAY

   display_end:
        return 
	
game_over:
    clrf  LATH
    bsf   LATH,3
    movf  right_score, w
    call  TABLE
    movwf LATJ
    call  DELAY

    clrf  LATH
    bsf   LATH,1
    movf  left_score, w
    call  TABLE
    movwf LATJ
    call  DELAY

    goto  game_over
    
DELAY	; Time Delay Routine with 3 nested loops
	movlw  0xff
    movwf  t1
    loop:
    decfsz t1,F ; Decrement t1. If 0 Skip next instruction
	GOTO   loop; ELSE Keep counting down
	return
	
INIT
    MOVLW   b'00001111'
    MOVWF   TRISG  ; MAKE RG0-RG1-RG2-RG3 PORTS(PINS) INPUT
    MOVLW   0X0F
    MOVWF   ADCON1 ; MAKE PORTA DIGITAL OUTPUT
    
    CLRF    PORTA
    CLRF    PORTB
    CLRF    PORTC
    CLRF    PORTD
    CLRF    PORTE
    CLRF    PORTF
    CLRF    PORTH
    CLRF    PORTJ
    
    CLRF    TRISA  ; MAKE PORTA AS OUTPUT
    CLRF    TRISB  ; MAKE PORTB AS OUTPUT
    CLRF    TRISC  ; MAKE PORTC AS OUTPUT
    CLRF    TRISD  ; MAKE PORTD AS OUTPUT
    CLRF    TRISE  ; MAKE PORTE AS OUTPUT
    CLRF    TRISF  ; MAKE PORTF AS OUTPUT
    CLRF    TRISH
    CLRF    TRISJ

    MOVLW   b'00011100' ; TURN ON THE FIRST LEDS
    MOVWF   PORTA
    MOVWF   PORTF
    
    MOVLW   b'00001000' ; TURN ON THE BALL
    MOVWF   PORTD
    MOVWF   row
    MOVLW   d'3'
    MOVWF   column_ball
    CLRF    counter
    CLRF    move_ball_flag
    CLRF    s_1_up
    CLRF    s_2_up   
    CLRF    s_1_down 
    CLRF    s_2_down
    CLRF    move_up_1
    CLRF    move_down_1
    CLRF    move_up_2
    CLRF    move_down_2
    CLRF    left_score
    CLRF    right_score
    CLRF    left_score_flag
    CLRF    right_score_flag
    CLRF    table_wreg,

    MOVLW    d'1'
    MOVWF    direction   ; INITIALLY BALL MOVES LEFT
    ;CLRF    PIE1 ; DONT USE TIMER1 AS AN INTERRUPT SOURCE
    ;CLRF    PIE1
    MOVLW   d'46'
    MOVWF   Yok_olan46
    
    CLRF    INTCON ;
    MOVLW   b'11000111'
    MOVWF   T0CON
    MOVLW   b'10000001' ; T1 ENABLE
    MOVWF   T1CON

    BSF     INTCON, 5
    BSF	    INTCON, 7; ENABLE INTERRUPTS
    return


END
