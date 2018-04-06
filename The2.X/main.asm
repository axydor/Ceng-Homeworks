#include "p18f8722.inc"

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
direction res 1    ; IN WHICH DIRECTION THE BALL IS MOVING LEFT=1, RIGHT=0
column_ball res 1  ; the column of the ball   COLUMN=0-> PORTA , COLUMN=1 -> PORTB
row res 1     ; the row of the ball
left_score res 1   ; Holds the score of the left one
right_score res 1   ; Holds the score of the right one

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
    btfss move_ball_flag,0 
    return
    clrf  move_ball_flag ; CLEAR FLAG SO THAT BALL GET INTO THIS FUNCTION ONLY AFTER 300MS
    btfss direction, 0
    goto  moving_to_right

    moving_to_left:  ;  SWICH CASES
        movf  column_ball, w
        xorlw 1 ; BALL AT PORT B
        btfsc STATUS, Z
        goto  left_a  ; BALL MOVES FROM PORTB TO PORTA
        xorlw 2^1 ; 1 ; BALL AT PORTC;
        btfsc STATUS, 2
        goto  left_b
        xorlw 3^2 ;   ; BALL AT PORTD;
        btfsc STATUS, 2
        goto  left_c
        xorlw 4^3     ; BALL AT PORTE;
        btfsc STATUS, 2
        goto  left_d
        xorlw 5^4   ; BALL AT PORTF;
        btfsc STATUS, 2
        goto  left_e

        left_a: ;  BALL MOVES FROM PORTB TO PORTA
            clrf   column_ball
            clrf   PORTB
            btfss  TMR0L, 0
            goto   a_first_bit_0
            btfss  TMR0L, 1
            goto   a_bit_01
            goto   a_bit_00_11

            a_first_bit_0:
                btfss TMR0L, 1
                goto a_bit_00_11
                goto a_bit_10

            a_bit_00_11: ; MOVE AT THE SAME LINE
                movf  row, w
                andwf PORTA, w
                btfsc STATUS, 2
                goto right_scored
                btg  direction, 0   
                goto right_b  ;      BALL HITS THE PEDAL AND CHANGES DIRECTION

            a_bit_01: ; MOVE UP
                rrncf row
                btfss row,0     ; TEST IF BALL AT THE UPPER-BORDER
                incf  row
                movf  row,w
                andwf PORTA, w
                btfsc STATUS, 2
                goto  right_scored    
                goto  right_b  ;      BALL HITS THE PEDAL AND MOVES

            a_bit_10: ; MOVE DOWN
                rlncf row
                movlw d'32'
                btfsc row, 6   ; TEST IF BALL AT THE LOWER-BORDER
                movlw d'32'    ; TURN ON BOTTOM LED
                movf  row, w   ;
                andwf PORTA, w ;
                btfsc STATUS, 2;
                goto right_scored
                btg  direction, 0
                goto right_b  ;      BALL HITS THE PEDAL AND CHANGES DIRECTION

        left_b:
            clrf   column_ball
            clrf   PORTC
            btfss  TMR0L, 0
            goto   b_first_bit_0
            btfss  TMR0L, 1
            goto   b_bit_01
            goto   b_bit_00_11

            b_first_bit_0:
                btfss TMR0L, 1
                goto b_bit_00_11
                goto b_bit_10

            b_bit_00_11: ; MOVE AT THE SAME LINE
                movff row, PORTB
                goto  left_end

            b_bit_01: ; MOVE UP
                rrncf row
                btfsc row,0     ; TEST IF BALL AT THE UPPER-BORDER
                movff row, PORTB
                movlw d'1'      ; TURN ON UPPER MOST LED
                movwf PORTB
                goto  left_end

            b_bit_10: ; MOVE DOWN
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTB
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTB
                goto  left_end

        left_c:
            movlw  d'1'
            movwf  column_ball
            clrf   PORTD
            btfss  TMR0L, 0
            goto   c_first_bit_0
            btfss  TMR0L, 1
            goto   c_bit_01
            goto   c_bit_00_11

            c_first_bit_0:
                btfss TMR0L, 1
                goto c_bit_00_11
                goto c_bit_10

            c_bit_00_11:
                movff row, PORTC
                goto  left_end

            c_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTC
                movlw d'1'
                movwf PORTC
                goto  left_end

            c_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTC
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTC
                goto  left_end

        left_d:
            movlw  d'2'
            movwf  column_ball
            clrf   PORTE
            btfss  TMR0L, 0
            goto   d_first_bit_0
            btfss  TMR0L, 1
            goto   d_bit_01
            goto   d_bit_00_11

            d_first_bit_0:
                btfss TMR0L, 1
                goto  d_bit_00_11
                goto  d_bit_10

            d_bit_00_11:
                movff row, PORTD

            d_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTD
                movlw d'1'
                movwf PORTD
                goto  left_end

            d_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTD
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTD
                goto  left_end

        left_e:
            movlw  d'3'
            movwf  column_ball
            clrf   PORTF
            btfss  TMR0L, 0
            goto   e_first_bit_0
            btfss  TMR0L, 1
            goto   e_bit_01
            goto   e_bit_00_11

            e_first_bit_0:
                btfss TMR0L, 1
                goto  e_bit_00_11
                goto  e_bit_10

            e_bit_00_11:
                movff row, PORTE

            e_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTE
                movlw d'1'
                movwf PORTE
                goto  left_end

            e_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTE
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTE
                goto  left_end


    moving_to_right:
        movf  column_ball, w
        xorlw 0 ; BALL AT PORT A
        btfsc STATUS, 2
        goto  right_b ; BALL MOVES FROM PORTA TO PORTB
        xorlw 1^0 ; 1 ; BALL AT PORTC;
        btfsc STATUS, 2
        goto  right_c ; BALL MOVES FROM PORTB TO PORTC
        xorlw 2^1 ;   ; BALL AT PORTD;
        btfsc STATUS, 2
        goto  right_d ; BALL MOVES FROM PORTC TO PORTD
        xorlw 3^2     ; BALL AT PORTE;
        btfsc STATUS, 2
        goto  right_e ; BALL MOVES FROM PORTD TO PORTE
        xorlw 4^3     ; BALL AT PORTF;
        btfsc STATUS, 2
        goto  right_f ; BALL MOVES FROM PORTE TO PORTF

        right_b:      ; BALL MOVES FROM PORTA TO PORTB
            movlw  d'1'
            movwf  column_ball
            btfss  TMR0L, 0
            goto   rb_first_bit_0
            btfss  TMR0L, 1
            goto   rb_bit_01
            goto   rb_bit_00_11

            rb_first_bit_0:
                btfss TMR0L, 1
                goto  rb_bit_00_11
                goto  rb_bit_10

            rb_bit_00_11:
                movff row, PORTB

            rb_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTB
                movlw d'1'
                movwf PORTB
                goto  left_end

            rb_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTB
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTB
                goto  left_end

        right_c:      ; BALL MOVES FROM PORTB TO PORTC
            movlw  d'2'
            movwf  column_ball
            btfss  TMR0L, 0
            goto   rc_first_bit_0
            btfss  TMR0L, 1
            goto   rc_bit_01
            goto   rc_bit_00_11

            rc_first_bit_0:
                btfss TMR0L, 1
                goto  rc_bit_00_11
                goto  rc_bit_10

            rc_bit_00_11:
                movff row, PORTC

            rc_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTC
                movlw d'1'
                movwf PORTC
                goto  left_end

            rc_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTC
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTC
                goto  left_end

        right_d:      ; BALL MOVES FROM PORTC TO PORTD
            movlw  d'3'
            movwf  column_ball
            btfss  TMR0L, 0
            goto   rd_first_bit_0
            btfss  TMR0L, 1
            goto   rd_bit_01
            goto   rd_bit_00_11

            rd_first_bit_0:
                btfss TMR0L, 1
                goto  rd_bit_00_11
                goto  rd_bit_10

            rd_bit_00_11:
                movff row, PORTD

            rd_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTD
                movlw d'1'
                movwf PORTD
                goto  left_end

            rd_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTD
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTD
                goto  left_end

        right_e:      ; BALL MOVES FROM PORTD TO PORTE
            movlw  d'4'
            movwf  column_ball
            btfss  TMR0L, 0
            goto   re_first_bit_0
            btfss  TMR0L, 1
            goto   re_bit_01
            goto   re_bit_00_11

            re_first_bit_0:
                btfss TMR0L, 1
                goto  re_bit_00_11
                goto  re_bit_10

            re_bit_00_11:
                movff row, PORTE

            re_bit_01:
                rrncf row
                btfsc row,0
                movff row, PORTE
                movlw d'1'
                movwf PORTE
                goto  left_end

            re_bit_10:
                rlncf row
                btfsc row, 6   ; ; TEST IF BALL AT THE LOWER-BORDER
                movff row,PORTE
                movlw d'32' ; TURN ON BOTTOM LED
                movff row, PORTE
                goto  left_end

        right_f:      ; BALL MOVES FROM PORTE TO PORTF
            movlw  d'5'
            movwf  column_ball
            btfss  TMR0L, 0
            goto   rf_first_bit_0
            btfss  TMR0L, 1
            goto   rf_bit_01
            goto   rf_bit_00_11

            rf_first_bit_0:
                btfss TMR0L, 1
                goto  rf_bit_00_11
                goto  rf_bit_10

            rf_bit_00_11:
                movf   row, w
                andwf  PORTF, w
                btfsc  STATUS, 2
                goto   left_scored
                btg    direction,0
                goto   left_e

            rf_bit_01:
                rrncf row
                btfsc row,0
                incf  row
                movf  row, w
                andwf PORTF, w
                btfsc STATUS, 2
                goto  left_scored
                btg   direction, 0
                goto  left_e

            rf_bit_10:
                rlncf row
                movlw d'32'
                btfsc row, 6   ; TEST IF BALL AT THE LOWER-BORDER
                movwf row
                movf  row, w
                andwf PORTF, w
                btfsc STATUS, 2
                goto  left_scored
                btg   direction,0
                goto  left_e

    left_end

    return
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

left_scored:
	    
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
    CLRF    left_score
    CLRF    right_score
    MOVLW   d'46'
    MOVWF   Yok_olan46
    CLRF    INTCON ;
    MOVLW   b'11000111'
    MOVWF   T0CON
    BSF     INTCON, 5
    BSF	    INTCON, 7; ENABLE INTERRUPTS
    return
    
    END
